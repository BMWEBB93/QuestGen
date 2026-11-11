// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "LocationType.h"

#include  "Plate.h"


// Sets default values
AWorldGenerator::AWorldGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Get the player and set position in middle of chunk
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // get the player	
}



// Called every frame
void AWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get player if it isn't set
	if (!Player)
	{
		Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		if (Player)
		{
			PlayerChunkCoord = FIntPoint(
				FMath::FloorToInt(Player->GetActorLocation().X / ChunkSize),
				FMath::FloorToInt(Player->GetActorLocation().Y / ChunkSize)
			);
			LastPlayerCoord = PlayerChunkCoord; 
		}
	}

	if (Player)
	{
				// get the players current chunk position
		PlayerChunkCoord = FIntPoint(
			FMath::FloorToInt(Player->GetActorLocation().X / ChunkSize),
			FMath::FloorToInt(Player->GetActorLocation().Y / ChunkSize)
			);
		
		// find if the player has moved to another chunk
		FIntPoint ChunkShiftDirection = PlayerChunkCoord - LastPlayerCoord;	
		ShiftsToMake += ChunkShiftDirection;
		LastPlayerCoord = PlayerChunkCoord;
		
		// shift the chunks towards the direction the player moves
		HandleChunkShifting();
		
		
		double StartTime = FPlatformTime::Seconds();
		
		// Generate queued chunks (Land)
		
		// get max chunks to build this tick
		int ChunksToBuild = FMath::Min(BuildsPerTick, PendingChunkRebuilds.Num());
		
		if (FPlatformTime::Seconds() - StartTime  < TimePerTick) // if within time limit
		{
			for (int i = 0; i < ChunksToBuild; i++)  // loop through build queue
			{
				// stop generating if exceeded the time limit
				if (FPlatformTime::Seconds() - StartTime >= TimePerTick) break;

				// Check if chunk is in game world
				FChunkData* data = nullptr;			
				data = ChunkData.Find(PendingChunkRebuilds[0]->WorldCoord);			
				PendingChunkRebuilds[0]->bIsInGameWorld = (data != nullptr);
				
				
				// Beach value is used to create sand on chunk space close to water
				// value is a float as it is passed into material colours
				float BeachValue = 1.f;
				if (PendingChunkRebuilds[0]->Location != ELocationType::Ocean && PendingChunkRebuilds[0]->Location != ELocationType::Beach)
				{
					BeachValue = 0.f;
				}
				
				// generate the chunk
				PendingChunkRebuilds[0]->GenerateChunk(data, TerrainMaterial, PendingChunkRebuilds[0]->LODBoundary, BeachValue);	
				if (data)
				{
					PendingChunkRebuilds[0]->CreateFoliage(*data);
					if (data->bHasTown && !data->bTownIsSpawned)
					{
						SpawnTown(*data);
					}
				}
				
				// update location data in chunk
				if (data)
				{
					PendingChunkRebuilds[0]->Location = data->Location;
				}
				else
				{
					PendingChunkRebuilds[0]->Location = ELocationType::Ocean;
				}

				// remove from build queue
				PendingChunkRebuilds.RemoveAt(0);
			}
		}
		

		// Generate queued chunks (Ocean)
		
		// get max chunks to build this tick
		int OceanChunksToBuild = FMath::Min(BuildsPerTick, PendingOceanRebuilds.Num());
		
		if (FPlatformTime::Seconds() - StartTime <= TimePerTick) // if within time limit
		{
			for (int i = 0; i < OceanChunksToBuild; i++)  // loop through build queue
			{
				// if time has been exceeded stop generating chunks
				if (FPlatformTime::Seconds() - StartTime > TimePerTick) break;		
				
				PendingOceanRebuilds[0]->GenerateChunk(OceanMaterial);
				PendingOceanRebuilds.RemoveAt(0); 
			}
		}
	}

}


void AWorldGenerator::SetNeighbourLocation(
	FChunkData* ThisChunk,
	FChunkData* NeighbourChunk,
	ENeighbourDirections Direction)
{
	if (NeighbourChunk)
	{
		if (NeighbourChunk->Location == ELocationType::Ocean)
		{
			ThisChunk->Location = ELocationType::Beach;

			ThisChunk->NeighbourChunks[Direction] = ELocationType::Ocean;
		}
		else
		{
			ThisChunk->NeighbourChunks[Direction] = NeighbourChunk->Location;
		}
	}
	else
	{
		ThisChunk->Location = ELocationType::Beach;

		ThisChunk->NeighbourChunks[Direction] = ELocationType::Ocean;
	}
}

void AWorldGenerator::UpdateChunkNeighbours()
{
	for (int y = 0; y < WorldSize; y++)
	{
		for (int x = 0; x < WorldSize; x++)
		{		
			
			FChunkData* ThisChunk = ChunkData.Find(FIntPoint(x - WorldSize / 2,  y - WorldSize / 2));
			
			if (ThisChunk->Location == ELocationType::Ocean) continue;
						
			FChunkData* NorthChunk = ChunkData.Find(FIntPoint(x - WorldSize / 2,  y + 1 - WorldSize / 2));
			FChunkData* EastChunk = ChunkData.Find(FIntPoint(x + 1 - WorldSize / 2,  y - WorldSize / 2));
			FChunkData* SouthChunk = ChunkData.Find(FIntPoint(x - WorldSize / 2,  y - 1 - WorldSize / 2));
			FChunkData* WestChunk = ChunkData.Find(FIntPoint(x - 1 - WorldSize / 2,  y - WorldSize / 2));
			FChunkData* NorthEastChunk = ChunkData.Find(FIntPoint(x + 1 - WorldSize / 2,  y + 1 - WorldSize / 2));
			FChunkData* SouthEastChunk = ChunkData.Find(FIntPoint(x + 1 - WorldSize / 2,  y - 1 - WorldSize / 2));
			FChunkData* NorthWestChunk = ChunkData.Find(FIntPoint(x - 1 -  WorldSize / 2,  y + 1 - WorldSize / 2));
			FChunkData* SouthWestChunk = ChunkData.Find(FIntPoint(x - 1 - WorldSize / 2,  y - 1 - WorldSize / 2));

			SetNeighbourLocation(ThisChunk, NorthChunk, ENeighbourDirections::N);
			SetNeighbourLocation(ThisChunk, EastChunk, ENeighbourDirections::E);
			SetNeighbourLocation(ThisChunk, SouthChunk, ENeighbourDirections::S);
			SetNeighbourLocation(ThisChunk, WestChunk, ENeighbourDirections::W);

			SetNeighbourLocation(ThisChunk, NorthEastChunk, ENeighbourDirections::NE);
			SetNeighbourLocation(ThisChunk, SouthEastChunk, ENeighbourDirections::SE);
			SetNeighbourLocation(ThisChunk, NorthWestChunk, ENeighbourDirections::NW);
			SetNeighbourLocation(ThisChunk, SouthWestChunk, ENeighbourDirections::SW);
		}
	}
}

void AWorldGenerator::DetermineMountains()
{
	// uses voronoi to create plates in game world which determines mountain ranges
	int PlateAmount = WorldSize / RegionRate;
	int PlateWidth = 1;
	int MinPlateDistance = 2;
	FRandomStream RandomStream(Seed);

	// create plates
	TArray<FPlate> Plates;
	for (int i = 0; i < PlateAmount; i++)
	{
		FPlate Plate;
		bool bIsPointValid = false;
		int maxIteration = 1000;
		while (!bIsPointValid)
		{
			// fail-safe to stop infinite looping
			maxIteration--;
			if (maxIteration <= 0) break;
			
			// pick a random point within game world
			FIntPoint RandomLocation = FIntPoint(
				RandomStream.RandRange(0, WorldSize -1),
				RandomStream.RandRange(0, WorldSize - 1));
			
			Plate.PlateLocation = FIntPoint(
				RandomLocation.X - WorldSize / 2, 
				RandomLocation.Y - WorldSize / 2);
			
			// find the data at the point
			FChunkData* Chunk = ChunkData.Find(Plate.PlateLocation);
			
			// check location is on land
			if (Chunk->Location != ELocationType::Ocean && Chunk->Location != ELocationType::Beach)
			{
				// check distance from existing plates
				bIsPointValid = true;
				for (const FPlate& ExistingPlate : Plates)
				{
					FIntPoint Distance2D = Plate.PlateLocation - ExistingPlate.PlateLocation;
					int Distance = FMath::Abs(Distance2D.X) + FMath::Abs(Distance2D.Y);
					
					if (Distance < MinPlateDistance)
					{
						bIsPointValid = false;
						break;
					}
				}
				if (bIsPointValid) 
				{
					FIntPoint Direction = FIntPoint(0, 0);
					
					// pick a random direction for plate that isnt 0
					while (Direction.X == 0 && Direction.Y == 0)
					{
						Direction = FIntPoint(
						RandomStream.RandRange(-1, 1),
						RandomStream.RandRange(-1, 1));
					}					
					Plate.MovementDirection = Direction;
					Plates.Add(Plate);
				}				
			}
		}		
	}
	
	// loop through chunks and determine mountains
	for (int y = 0; y < WorldSize; y++)
	{
		for (int x = 0; x < WorldSize; x++)
		{
			// get the chunk data
			FChunkData* ThisChunk = ChunkData.Find(FIntPoint(x - WorldSize / 2,  y - WorldSize / 2));
			
			// get closest and second-closest plate
			int ClosestDistance = WorldSize * 2;
			int SecondClosestDistance = WorldSize * 2;
			
			FPlate* ClosestPlate = nullptr;
			FPlate* SecondClosestPlate = nullptr;
			
			// loop through each plate
			for (FPlate& Plate : Plates)
			{
				// compare distance from chunk to plate
				FIntPoint Distance2D = ThisChunk->WorldCoord - Plate.PlateLocation;
				int Distance = FMath::Abs(Distance2D.X) + FMath::Abs(Distance2D.Y);
				
				if (Distance < ClosestDistance)
				{
					SecondClosestDistance = ClosestDistance;
					SecondClosestPlate = ClosestPlate;
					
					ClosestDistance = Distance;
					ClosestPlate = &Plate;
				}
				else if (Distance < SecondClosestDistance)
				{
					SecondClosestDistance = Distance;
					SecondClosestPlate = &Plate;
				}				
			}
			
			// if on a boundary
			if (SecondClosestDistance - ClosestDistance <= PlateWidth) 
			{
				// if chunk isn't ocean or beach
				if (ThisChunk->Location != ELocationType::Beach && ThisChunk->Location != ELocationType::Ocean)
				{
					// check plates are converging
					FIntPoint PlateDirection = SecondClosestPlate->PlateLocation - ClosestPlate->PlateLocation;
					PlateDirection.X = FMath::Clamp(PlateDirection.X, -1, 1);
					PlateDirection.Y = FMath::Clamp(PlateDirection.Y, -1, 1);
					
					int DotA =
						ClosestPlate->MovementDirection.X * PlateDirection.X +
						ClosestPlate->MovementDirection.Y * PlateDirection.Y;

					int DotB =
						SecondClosestPlate->MovementDirection.X * -PlateDirection.X +
						SecondClosestPlate->MovementDirection.Y * -PlateDirection.Y;
					
					// if plates are moving away, create mountain
					if (DotA > 0 || DotB > 0)
					{
						ThisChunk->Location = ELocationType::Mountain;
					}
				}				
			}
		}
	}	
}

void AWorldGenerator::DetermineBiomes()
{
	for (int y = 0; y < WorldSize; y++)
	{
		for (int x = 0; x < WorldSize; x++)
		{
			// get the chunk data
			FChunkData* ThisChunk = ChunkData.Find(FIntPoint(x - WorldSize / 2,  y - WorldSize / 2));
			
			// skip chunks that already have locations assigned
			if (ThisChunk->Location != ELocationType::NONE)
			{
				continue;	
			}		
			
			// Check the chunk neighbours a mountain
			bool bNeighboursMountain = false;
			for (const auto& Neighbour : ThisChunk->NeighbourChunks)
			{
				if (Neighbour.Value == ELocationType::Mountain)
				{
					bNeighboursMountain = true;
					break;
				}
			}			
			
			// Generate humidity noise
			float Humidity = FMath::PerlinNoise2D(
				FVector2D(
					ThisChunk->WorldCoord.X + SeedOffsetX,
					ThisChunk->WorldCoord.Y + SeedOffsetY
				) * HumidityFrequency
			);

			// Convert from -1 to 1 into 0 to 1
			Humidity = (Humidity + 1.f) / 2.f;
			
			if (Humidity > 0.6f )
			{
				ThisChunk->Location = ELocationType::Swamp;
			}	
			else if (Humidity > 0.4f)
			{
				ThisChunk->Location = ELocationType::Forest;
			}
			else
			{
				ThisChunk->Location = ELocationType::Grassland;
			}			
		}
	}
}

void AWorldGenerator::CreateRiverSources()
{	
	// clear rivers array
	Rivers.Empty();
	
	for (int i = 0; i < RiverCount; i++)
	{
		// Find best River Source
		int BestScore = 0;
		FIntPoint BestSource = FIntPoint(0, 0);
		for (int y = 0; y < WorldSize; y++)
		{
			for (int x = 0; x < WorldSize; x++)
			{			
				// get the chunk data
				FChunkData* Chunk = ChunkData.Find(FIntPoint(x - WorldSize / 2,  y - WorldSize / 2));
			
				// skip chunks on mountain, ocean or swamp
				if (Chunk->Location == ELocationType::Mountain ||
					Chunk->Location == ELocationType::Ocean ||
					Chunk->Location == ELocationType::Swamp)
				{
					continue;
				}
			
				// create river score for this chunk
				int ChunkScore = 0;
			
				// loop through chunk neighbours and increment based on biomes
				for (const TPair<ENeighbourDirections, ELocationType>& Neighbour : Chunk->NeighbourChunks)
				{
					ELocationType Location = Neighbour.Value;

					if (Location == ELocationType::Mountain)
					{
						ChunkScore += 2;
					}
					if (Location == ELocationType::Swamp || Location == ELocationType::Forest)
					{
						ChunkScore += 1;
					}
					if (Location == ELocationType::Ocean || Location == ELocationType::Beach)
					{
						ChunkScore += -100;
					}
				}
			
				// get the closest river source
				int ClosestRiverSource = WorldSize * 2;
				for (int j = 0; j < Rivers.Num(); j++)
				{
					int XDistance = Rivers[j].Source.X - Chunk->WorldCoord.X;
					int YDistance = Rivers[j].Source.Y - Chunk->WorldCoord.Y;
					int Distance = FMath::Abs(XDistance) + FMath::Abs(YDistance);
				
					if (Distance < ClosestRiverSource)
					{
						ClosestRiverSource = Distance;
					}					
				}			

				// Reduce score if close to another source
				int ScoreAdjustment = FMath::Clamp(ClosestRiverSource, 0 , 10);
				ChunkScore += ScoreAdjustment;
			
				// store best chunk score and river source coordinate
				if (ChunkScore > BestScore)
				{				
					BestScore = ChunkScore;
					BestSource = Chunk->WorldCoord;
				}
			}
		}
		FRiver NewRiver; 
		NewRiver.Source = BestSource;
		Rivers.Add(NewRiver);	
	}
}

void AWorldGenerator::CreateRiverPaths()
{
	FRandomStream RiverStream(Seed + 12345);
	for (int i = 0; i < Rivers.Num(); i++)
	{
		// get the coord of the river source
		int x = Rivers[i].Source.X;
		int y = Rivers[i].Source.Y;
		
		// get the source of the river and add to the river path
		FChunkData* SourceChunk = ChunkData.Find(FIntPoint(x,  y));	
		Rivers[i].RiverPath.Add(SourceChunk->WorldCoord);
		
		// while river has not reached the ocean
		while (!Rivers[i].bReachedOcean)
		{
			// get the last point in the river path
			FIntPoint LastPoint =  Rivers[i].RiverPath.Last();			
			
			// create directions the river can travel
			TArray<FIntPoint> PossibleDirections =
			{
				FIntPoint(0, 1),   // North
				FIntPoint(0, -1),  // South
				FIntPoint(1, 0),   // East
				FIntPoint(-1, 0)   // West
			};
			TArray<FIntPoint> ValidDirections;
			
				
			// get the direction towards the coast
			FIntPoint FavouredDirection = FIntPoint(0, 0);
			int xDir = FMath::Sign(LastPoint.X);
			int yDir = FMath::Sign(LastPoint.Y);
			
			if (FMath::Abs(LastPoint.X) >= FMath::Abs(LastPoint.Y))
			{
				FavouredDirection = FIntPoint(xDir, 0);
			}			
			else
			{
				FavouredDirection = FIntPoint(0, yDir);
			}	
			
			// check possible directions for valid next river path
			for (int j = 0; j < PossibleDirections.Num(); j++)
			{
				// get coordinate
				FIntPoint CheckedDirectionCoord = PossibleDirections[j] + LastPoint;
				
				// if already in river skip it
				if (Rivers[i].RiverPath.Contains(CheckedDirectionCoord))
				{
					continue;
				}
				
				// get the chunk data
				FChunkData* CheckedChunk = ChunkData.Find(CheckedDirectionCoord);
				if (!CheckedChunk)
				{					
					Rivers[i].bReachedOcean = true;	
				}
				if (CheckedChunk->Location == ELocationType::Ocean)
				{
					Rivers[i].bReachedOcean = true;	
				}
				// rivers dont path in mountains
				if (CheckedChunk->Location != ELocationType::Mountain)
				{
					ValidDirections.Add(PossibleDirections[j]);					
				}
			}						
			
			// random value for river path
			int Choice = RiverStream.RandRange(0, 10);
			
			FIntPoint NextCoord = FIntPoint(0,0);			
			// if best direction is valid add that 
			if (ValidDirections.Contains(FavouredDirection) && Choice > 3)
			{
				NextCoord = FavouredDirection + LastPoint;
			}
			else if (ValidDirections.Num() > 0)// add a random other direction
			{
				int RandomIndex = RiverStream.RandRange(0, ValidDirections.Num() - 1);
				NextCoord = ValidDirections[RandomIndex] + LastPoint;
			}
			else
			{
				Rivers[i].bReachedOcean = true;	
				Rivers[i].bOnlyLake = true;
			}
			// Add river direction and coordinate
			Rivers[i].RiverDirection.Add(LastPoint - NextCoord);
			Rivers[i].RiverPath.Add(NextCoord);
		}
		// if river path failed remove path
		if (Rivers[i].bOnlyLake)
		{
			Rivers[i].RiverDirection.Empty();
			Rivers[i].RiverPath.Empty();
			Rivers[i].RiverPath.Add(Rivers[i].Source);
		}
	}
}

void AWorldGenerator::CarvePointInRiver(FChunkData* Chunk, int x, int y)
{		
	// Radius of the water part of the river
    float RiverRadius = RiverWidth / 2.f;
	// Radius of the flat bank next to the river
    float FloodRadius = RiverRadius + FloodPlainWidth;	
	// Height of the bank next to the river
    float FloodPlainHeight = 20.f;

	// loop through each vertex in the river radius
    for (int OffsetX = -FMath::CeilToInt(FloodRadius); OffsetX <= FMath::CeilToInt(FloodRadius ); OffsetX++)
    {
    	for (int OffsetY = -FMath::CeilToInt(FloodRadius); OffsetY <= FMath::CeilToInt(FloodRadius); OffsetY++)
    	{
    		// get the vertex
    		int VertexX = x + OffsetX;
    		int VertexY = y + OffsetY;

    		// if vertex is out of bounds, skip it
    		if (VertexX < 0 || VertexX > ChunkResolution ||
				VertexY < 0 || VertexY > ChunkResolution )
    		{
    			continue;
    		}

    		// get the distance from the river centre
    		float Distance = FMath::Sqrt(
				FMath::Square((float)OffsetX) +
				FMath::Square((float)OffsetY));    	

    		// get the index of the vertex
    		int32 Index = VertexX + VertexY * (ChunkResolution + 1);
    		
    		float OriginalHeight = Chunk->Heights[Index];
    		float TargetHeight;

    		
    		
    		if (Distance <= RiverRadius)
    		{
    			// River bed
    			TargetHeight = RiverDepth;
    		}
    		else
    		{
    			// Flood plain slope
    			float Alpha = (Distance - RiverRadius) / FloodPlainWidth;
    			Alpha = FMath::SmoothStep(0.f, 1.f, Alpha);

    			TargetHeight = FMath::Lerp(
					FloodPlainHeight,
					OriginalHeight,
					Alpha
				);
    		}   	
    		
    		Chunk->Heights[Index] = FMath::Min(
				OriginalHeight,
				TargetHeight
			);
    	}
    }
}

void AWorldGenerator::CreateLake(int HalfChunkLength, FRiver& River)
{
	FChunkData* SourceChunk = ChunkData.Find(River.RiverPath[0]);

	if (SourceChunk)
	{
		int CentreX = HalfChunkLength;
		int CentreY = HalfChunkLength;

		for (int x = 0; x <= ChunkResolution; x++)
		{
			for (int y = 0; y <= ChunkResolution; y++)
			{
				float Distance = FVector2D::Distance(
					FVector2D(x, y),
					FVector2D(CentreX, CentreY)
				);

				if (Distance > LakeRadius)
					continue;

				int32 Index = x + y * (ChunkResolution + 1);

				float Alpha = Distance / LakeRadius;
				Alpha = FMath::SmoothStep(0.f, 1.f, Alpha);

				float OriginalHeight = SourceChunk->Heights[Index];

				float TargetHeight = FMath::Lerp(
					-LakeDepth,
					OriginalHeight,
					Alpha
				);

				SourceChunk->Heights[Index] =
					FMath::Min(OriginalHeight, TargetHeight);
			}
		}
	}
}

void AWorldGenerator::CarveRivers()
{
	int HalfChunkLength = ChunkResolution / 2;
	
	for (int i = 0; i < Rivers.Num(); i++) // for every river
	{
		FRiver& River = Rivers[i];
		
		// create lake at beginning of river
		CreateLake(HalfChunkLength, River);		
		
		if (!River.bOnlyLake)
		{
			for (int j = 0; j < River.RiverPath.Num(); j++) // for every point on river
			{
				FChunkData* Chunk = ChunkData.Find(River.RiverPath[j]);
				
				int x = HalfChunkLength;
				int y = HalfChunkLength;
				int VertexIndex = x + y * (ChunkResolution + 1);
				
				// centre of the chunk is set to river height
				if (RiverDepth < Chunk->Heights[VertexIndex])
				{
					Chunk->Heights[VertexIndex] = RiverDepth;	
				}
				
				// if there was a previous point			
				if (j > 0)
				{
					// get the direction the river came from
					FIntPoint RiverSourceDirection = River.RiverPath[j] - River.RiverPath[j - 1];	
					
					// loop until the river reaches the edge of the chunk				
					int Progress = 0;
					
					bool bWithinChunk = true;
					while (bWithinChunk)
					{			
						int Halfway = ChunkResolution / 4;		
						
						float Curve = (float)Progress / Halfway;
						Curve = FMath::Sin(Curve * PI);						

						int Meander = FMath::RoundToInt(Curve * 1.f);	
						
						// Determine perpendicular direction
						if (RiverSourceDirection.X != 0)
						{
							x -= RiverSourceDirection.X;
							if (x <= 0 || x >= ChunkResolution)
							{
								y = HalfChunkLength;
							}
							else
							{
								y += Meander;
							}
						}
						else
						{
							y -= RiverSourceDirection.Y;		
							if (y <= 0 || y >= ChunkResolution)
							{
								x = HalfChunkLength;
							}
							else
							{
								x += Meander;
							}
						}
						
						// is index within the chunk
						if (x < 0 || x >= ChunkResolution + 1 || y < 0 || y >= ChunkResolution + 1)
						{
							bWithinChunk = false;
						}
						else
						{
							// vertex towards river direction			
							CarvePointInRiver(Chunk, x, y);					
						}				
						Progress++;
					}			
				}		
				
				// if not at the last point
				if (j != River.RiverPath.Num() -1)
				{
					// reset x and y
					x = HalfChunkLength;
					y = HalfChunkLength;
					
					// the direction the river is travelling
					FIntPoint RiverDirection = River.RiverDirection[j];
					
					// loop until the river reaches the edge of the chunk
					bool bWithinChunk = true;
					int Progress = 0;

					while (bWithinChunk)
					{
						int Halfway = ChunkResolution / 4;

						float Curve = (float)Progress / Halfway;
						Curve = FMath::Sin(Curve * PI);
										
						int Meander = FMath::RoundToInt(Curve * 1.f);

						// Move towards the exit
						if (RiverDirection.X != 0)
						{
							x -= RiverDirection.X;

							if (x <= 0 || x >= ChunkResolution)
							{
								y = HalfChunkLength;
							}
							else
							{
								y += Meander;
							}
						}
						else
						{
							y -= RiverDirection.Y;

							if (y <= 0 || y >= ChunkResolution)
							{
								x = HalfChunkLength;
							}
							else
							{
								x += Meander;
							}
						}

						if (x < 0 || x >= ChunkResolution + 1 ||
							y < 0 || y >= ChunkResolution + 1)
						{
							bWithinChunk = false;
						}
						else
						{
							CarvePointInRiver(Chunk, x, y);
						}

						Progress++;
					}
				}
			}
		}
	}
		
}

bool AWorldGenerator::PositionOnRiver(FIntPoint Coord)
{
	// get the chunk data
	FChunkData* Chunk = ChunkData.Find(Coord);
	
	bool bOnRiver = false;
	for (const FRiver& River : Rivers) // loop every river
	{
		if (River.RiverPath.Contains(Chunk->WorldCoord)) // if location is in river
		{			
			bOnRiver = true;
			break;
		}
	}
	return bOnRiver; // location not on river
}

void AWorldGenerator::FindTownLocations()
{
	// clear Towns array
	Towns.Empty();
	
	for (int i = 0; i < TownsCount; i++)
	{
		// Find best Town Location
		int BestScore = 0;
		FIntPoint BestLocation;
		FIntPoint BestSource = FIntPoint(0, 0);
		for (int y = 0; y < WorldSize; y++)
		{
			for (int x = 0; x < WorldSize; x++)
			{			
				// get the chunk data
				FChunkData* Chunk = ChunkData.Find(FIntPoint(x - WorldSize / 2,  y - WorldSize / 2));
			
				// check the chunk is on the river
				bool bOnRiver = PositionOnRiver(Chunk->WorldCoord);
				
				// skip chunks on river
				if (bOnRiver)
				{
					continue;
				}
				
				// skip chunks on mountain, ocean or swamp
				if (Chunk->Location == ELocationType::Mountain ||
					Chunk->Location == ELocationType::Ocean ||
					Chunk->Location == ELocationType::Swamp || 
					Chunk->Location == ELocationType::Forest)					
				{
					continue;
				}
				
				
				// skip locations already occupied
				bool bLocationOccupied = false;
				for (FTown town : Towns)
				{
					if (town.WorldPosition == Chunk->WorldCoord)
					{
						bLocationOccupied = true;
					}
				}
				if (bLocationOccupied)
				{
					continue;
				}			
			
				// create Town score for this chunk
				int ChunkScore = 0;
				int CoastCount = 0;
			
				if (Chunk->Location == ELocationType::Grassland)
				{
					ChunkScore += 2;	
				}
				
				
				// loop through chunk neighbours and increment based on biomes
				for (const TPair<ENeighbourDirections, ELocationType>& Neighbour : Chunk->NeighbourChunks)
				{
					ELocationType Location = Neighbour.Value;				
					if (Location == ELocationType::Ocean)
					{
						CoastCount += 1;
					}
				}
				if (CoastCount > 1 && CoastCount < 4)
				{
					ChunkScore += 10;
				}
				
				// check chunk borders river
				for (int a = -1; a <= 1; a++)
				{
					for (int b = -1; b <= 1; b++)
					{
						if (PositionOnRiver(Chunk->WorldCoord + FIntPoint(a, b)))
						{
							if (a == 0 && b == 0) continue; // skip middle chunk
							ChunkScore += 2;
						}
					}
				}
			
				// get the closest Town
				int ClosestTown = WorldSize * 2;
				for (int j = 0; j < Towns.Num(); j++)
				{
					int XDistance = Towns[j].WorldPosition.X - Chunk->WorldCoord.X;
					int YDistance = Towns[j].WorldPosition.Y - Chunk->WorldCoord.Y;
					int Distance = FMath::Abs(XDistance) + FMath::Abs(YDistance);
				
					if (Distance < ClosestTown)
					{
						ClosestTown = Distance;
					}					
				}			

				// Reduce score if close to another town
				int ScoreAdjustment = FMath::Clamp(ClosestTown, 0 , 15);
				ChunkScore += ScoreAdjustment  *3;
			
				// store best chunk score and river source coordinate
				if (ChunkScore > BestScore)
				{				
					BestScore = ChunkScore;
					BestLocation = Chunk->WorldCoord;
				}
			}
		}
		FTown NewTown; 
		NewTown.WorldPosition = BestLocation;
		Towns.Add(NewTown);	
		
		// get the chunk data and add the new town
		FChunkData* Chunk = ChunkData.Find(BestLocation);
		Chunk->Town = NewTown;
		Chunk->bHasTown = true;
	}
}



void AWorldGenerator::GenerateWorld()
{
	
	ClearWorld();
	
	// Ensure resolution is 32, 64, 128 ect.
	ChunkResolution = FMath::Max(ChunkResolution, 1);
	ChunkResolution = 1 << FMath::CeilLogTwo(ChunkResolution);
	ChunkSize = ChunkScale * ChunkResolution;
	
	
	// Create noise offset from world seed
	FRandomStream SeedStream(Seed);
	SeedOffsetX = SeedStream.FRandRange(-100000.f, 100000.f);
	SeedOffsetY = SeedStream.FRandRange(-100000.f, 100000.f);
	
	// Clear chunk data 
	ChunkData.Empty();
	
	// create world data
	for (int y = 0; y < WorldSize; y++)
	{
		for (int x = 0; x < WorldSize; x++)
		{
			// create chunk coordinate
			FIntPoint ChunkCoord = FIntPoint(x - WorldSize / 2, y - WorldSize / 2);

			// create chunk data
			FChunkData NewChunkData;
			NewChunkData.WorldCoord = ChunkCoord;
			
			// Get distance from world edge
			int MinCoord = -(WorldSize / 2);
			int MaxCoord = (WorldSize - 1) / 2;
			float DistanceToEdge = FMath::Min(
				FMath::Min(ChunkCoord.X - MinCoord, MaxCoord - ChunkCoord.X),
				FMath::Min(ChunkCoord.Y - MinCoord, MaxCoord - ChunkCoord.Y)
				);
			
			// create noise for offsetting the ocean edge
			float Noise = FMath::PerlinNoise2D(FVector2D(ChunkCoord.X + SeedOffsetX, ChunkCoord.Y + SeedOffsetY) * CoastFrequency);			
			Noise = (Noise + 1) /2;
			if (Noise > (DistanceToEdge / 4)) 
			{
				NewChunkData.Location = ELocationType::Ocean; // set chunk as an ocean chunk
			}
			// Add data to map
			ChunkData.Add(ChunkCoord, NewChunkData);			
		}
	}
	
	// update chunks so beaches are placed
	UpdateChunkNeighbours();
	
	// determine Mountains using voronoi avoiding coast and sea
	DetermineMountains();
	
	// update chunks so neighbour chunks are known
	UpdateChunkNeighbours();
	
	// loop through chunks and determine Biomes
	DetermineBiomes();	
	
	// update chunks so neighbour chunks are known
	UpdateChunkNeighbours();
	
	// Create River Sources
	CreateRiverSources();
	
	// Create River Paths	
	CreateRiverPaths();
	
	// find locations for towns
	FindTownLocations();
	
	SetTownData();
	
	// generate height map
	for (int y = 0; y < WorldSize; y++)
	{
		for (int x = 0; x < WorldSize; x++)
		{			
			FChunkData* Chunk = ChunkData.Find(FIntPoint(x - WorldSize / 2,  y - WorldSize / 2));				
			GenerateMapData(*Chunk);		
		}
	}
	
	//Carve Rivers	
	CarveRivers();
	
	
	
	// generate Foliage
	for (int y = 0; y < WorldSize; y++)
	{
		for (int x = 0; x < WorldSize; x++)
		{			
			FChunkData* Chunk = ChunkData.Find(FIntPoint(x - WorldSize / 2,  y - WorldSize / 2));				
			GenerateFoliageData(*Chunk);
		}
	}
	
	// Get amount of chunks loaded along 1 axis
	int TotalLoadedChunksSize = (
		(LOD1RenderDistance + LOD2RenderDistance + LOD3RenderDistance + LOD4RenderDistance)
		* 2)
		+ 1;
	
	
	
	// Start creating chunks
	for (int y = 0; y < TotalLoadedChunksSize; y++)
	{
		for (int x = 0; x < TotalLoadedChunksSize; x++)
		{		
			
			FIntPoint Coord = FIntPoint(x - TotalLoadedChunksSize / 2, y - TotalLoadedChunksSize / 2);
			
			// Spawn Chunk
			FVector SpawnLocation(
		   Coord.X * ChunkSize,
		   Coord.Y * ChunkSize,
		   0.0f);
			
			AChunkVisualiser* Chunk = 
				GetWorld()->SpawnActor<AChunkVisualiser>(
					AChunkVisualiser::StaticClass(),
					SpawnLocation,
					FRotator::ZeroRotator
				);
					
			Chunk->SetFolderPath(TEXT("WorldGeneration/Land"));
			
			// Find LOD of chunk
			Chunk->LOD = GetLOD(Coord);

			// Set chunk variables
			Chunk->Resolution = ChunkResolution;
			Chunk->Scale = ChunkScale;
			Chunk->ChunkUVScale = ChunkUVScale;
			
			Chunk->WorldCoord = Coord;
			Chunk->ChunkCoord = Coord;
			
			Chunk->OceanDepth = OceanDepth;
			
			// set the meshes in the chunks
			Chunk->TreeMesh = TreeMesh;
			Chunk->GrassMesh = GrassMesh;
			Chunk->PlantMesh = PlantMesh;
			Chunk->SetHISMs();
			
			// add chunk to map
			ChunkVisualisers.Add(FIntPoint(x, y), Chunk);			
			
			// get the direction of any LOD transitions
			FIntPoint LODBoundary = GetLODTransitionBoundary(Chunk->LOD, Coord);	
			Chunk->LODBoundary = LODBoundary;		
			
			// Get the chunk data
			FChunkData* Data = ChunkData.Find(Coord);
			
			Chunk->bIsInGameWorld = (Data != nullptr);
			Chunk->Location = Data->Location;
			
			// Beach value is used to create sand on chunk space close to water
			// value is a float as it is passed into material colours
			float BeachValue = 1.f;
			if (Data->Location != ELocationType::Ocean && Data->Location != ELocationType::Beach)
			{
				BeachValue = 0.f;
			}
			
			// Generate mesh with terrain data
			Chunk->GenerateChunk(Data, TerrainMaterial, LODBoundary, BeachValue);
			Chunk->CreateFoliage(*Data);
			// create the town
			if (Data->bHasTown && !Data->bTownIsSpawned)
			{
				SpawnTown(*Data);
			}
		}
	}	
	
	// Generate Ocean
	for (int y = 0; y < TotalLoadedChunksSize; y++)
	{
		for (int x = 0; x < TotalLoadedChunksSize; x++)
		{
			FIntPoint Coord = FIntPoint(x - TotalLoadedChunksSize / 2, y - TotalLoadedChunksSize / 2);
			
			// Spawn Chunk
			FVector SpawnLocation(
		   Coord.X * ChunkSize,
		   Coord.Y * ChunkSize,
		   0.0f);
			
			AOceanChunk* OceanChunk = 
				GetWorld()->SpawnActor<AOceanChunk>(
					AOceanChunk::StaticClass(),
					SpawnLocation,
					FRotator::ZeroRotator
				);
			
			OceanChunk->SetFolderPath(TEXT("WorldGeneration/Ocean"));
					
			OceanChunk->LOD = GetLOD(Coord);
			OceanChunk->WorldCoord = Coord;
			OceanChunk->ChunkCoord = Coord;
			
			OceanChunk->Resolution = ChunkResolution;
			OceanChunk->Scale = ChunkScale;
			OceanChunk->UVScale = ChunkUVScale;
			
			OceanChunks.Add(FIntPoint(x, y), OceanChunk);

			OceanChunk->GenerateChunk(OceanMaterial);
		}
	}	
}

void AWorldGenerator::ClearWorld()
{
	// Destroy all spawned town actors
	for (ATownGenerator* Town : TownGenerators)
	{
		if (IsValid(Town))
		{
			// Find the chunk data
			FChunkData* Data = ChunkData.Find(Town->TownData.WorldPosition);

			if (Data)
			{
				Data->Town = Town->TownData;
				Data->bTownIsSpawned = false;
				
				DespawnTown(Town, *Data);				
			}			
		}
	}
	
	TownGenerators.Empty();
	Towns.Empty();
	
	// Delete current chunks
	for (auto& Pair : ChunkVisualisers)
	{
		if (IsValid(Pair.Value))
		{
			Pair.Value->Destroy();
		}
	}
	ChunkVisualisers.Empty();	
	
	// Delete current ocean chunks
	for (auto& Pair : OceanChunks)
	{
		if (IsValid(Pair.Value))
		{
			Pair.Value->Destroy();
		}
	}
	OceanChunks.Empty();
}

float AWorldGenerator::AddHillsToMountainBorder(FChunkData& chunkData, int y, int x, float Noise2, float Noise1)
{
	// get ratio to determine closest neighbouring chunk 
	float NorthRatio = (float)y / ChunkResolution;
	float SouthRatio = 1.f - NorthRatio;
	float EastRatio = (float)x / ChunkResolution;
	float WestRatio = 1.f - EastRatio;
					
	float NorthEastRatio = NorthRatio * EastRatio;
	float SouthEastRatio = SouthRatio * EastRatio;
	float NorthWestRatio = NorthRatio * WestRatio;
	float SouthWestRatio = SouthRatio * WestRatio;
					
	// if no mountain is bordered, that ratio is invalid
	if (chunkData.NeighbourChunks[ENeighbourDirections::N] != ELocationType::Mountain)
		NorthRatio = 0.f;

	if (chunkData.NeighbourChunks[ENeighbourDirections::E] != ELocationType::Mountain)
		EastRatio = 0.f;

	if (chunkData.NeighbourChunks[ENeighbourDirections::S] != ELocationType::Mountain)
		SouthRatio = 0.f;

	if (chunkData.NeighbourChunks[ENeighbourDirections::W] != ELocationType::Mountain)
		WestRatio = 0.f;

	if (chunkData.NeighbourChunks[ENeighbourDirections::NE] != ELocationType::Mountain)
		NorthEastRatio = 0.f;

	if (chunkData.NeighbourChunks[ENeighbourDirections::SE] != ELocationType::Mountain)
		SouthEastRatio = 0.f;

	if (chunkData.NeighbourChunks[ENeighbourDirections::NW] != ELocationType::Mountain)
		NorthWestRatio = 0.f;

	if (chunkData.NeighbourChunks[ENeighbourDirections::SW] != ELocationType::Mountain)
		SouthWestRatio = 0.f;
					
						
	// get the closest neighbouring chunk
	float HillRatio = 0.f;
	HillRatio = FMath::Max(NorthRatio, HillRatio);
	HillRatio = FMath::Max(EastRatio, HillRatio);
	HillRatio = FMath::Max(SouthRatio, HillRatio);
	HillRatio = FMath::Max(WestRatio, HillRatio);
	HillRatio = FMath::Max(NorthEastRatio, HillRatio);
	HillRatio = FMath::Max(SouthEastRatio, HillRatio);
	HillRatio = FMath::Max(NorthWestRatio, HillRatio);
	HillRatio = FMath::Max(SouthWestRatio, HillRatio);
					
	float BlendRatio = FMath::Clamp((1 - HillRatio) * 2, 0.f, 1.f);
	float Hills = (1 - BlendRatio) * Noise2 * Noise2Scale * MountainHillScale;
	Hills += (1 - BlendRatio) * Noise1 * Noise1Scale/5 * MountainHillScale;
	return Hills;
}

void AWorldGenerator::GenerateMapData(FChunkData& chunkData)
{
	int VertexCount = ChunkResolution + 1;

	chunkData.Heights.Init(0.f, VertexCount * VertexCount); // Sets size of heightmap
	
	
	
	for (int y = 0; y <= ChunkResolution; y++)
	{
		for (int x = 0; x <= ChunkResolution; x++)
		{
			// get the coordinate of the vertex from the array
			int Index = x + (y * VertexCount);
			
			// get the world coordinate of vetices
			float WorldX = chunkData.WorldCoord.X * ChunkSize + x * ChunkScale;
			float WorldY = chunkData.WorldCoord.Y * ChunkSize + y * ChunkScale;
			
			switch (chunkData.Location)
			{
			case ELocationType::Ocean:
				{			
					float OceanNoise1 = (FMath::PerlinNoise2D(FVector2D(
						WorldX + SeedOffsetX, 
						WorldY + SeedOffsetY)  * OceanFloorNoiseFrequency) + 1) / 2;
				
				
					float OceanNoise2 = (FMath::PerlinNoise2D(FVector2D(
						WorldX + SeedOffsetX, 
						WorldY + SeedOffsetY)  * OceanFloorNoiseFrequency * 8) + 1) / 2;
				
					float NoiseHeight1 = OceanNoise1 * OceanFloorNoiseScale;
					float NoiseHeight2 = OceanNoise2 * OceanFloorNoiseScale /5;
					chunkData.Heights[Index] = -OceanDepth + NoiseHeight1 + NoiseHeight2;
					break;
				}
			
			case ELocationType::Mountain:
				{						
					// get ratio to determine proximity of other chunks
					float NorthRatio = (float)y / ChunkResolution;
					float SouthRatio = 1.f - NorthRatio;
					float EastRatio = (float)x / ChunkResolution;
					float WestRatio = 1.f - EastRatio;
					
					float NorthEastRatio = NorthRatio * EastRatio;
					float SouthEastRatio = SouthRatio * EastRatio;
					float NorthWestRatio = NorthRatio * WestRatio;
					float SouthWestRatio = SouthRatio * WestRatio;
					
					// if mountain is on border, that ratio is not counted
					if (chunkData.NeighbourChunks[ENeighbourDirections::N] == ELocationType::Mountain)
						NorthRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::E] == ELocationType::Mountain)
						EastRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::S] == ELocationType::Mountain) 
						SouthRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::W] == ELocationType::Mountain)
						WestRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::NE] == ELocationType::Mountain)
						NorthEastRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::SE] == ELocationType::Mountain)
						SouthEastRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::NW] == ELocationType::Mountain)
						NorthWestRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::SW] == ELocationType::Mountain) 
						SouthWestRatio = 0.f;
					
								
					// get the closest neighbour ratio that's not a mountain
					float MountainRatio = 0.f;
					MountainRatio = FMath::Max(NorthRatio, MountainRatio);
					MountainRatio = FMath::Max(EastRatio, MountainRatio);
					MountainRatio = FMath::Max(SouthRatio, MountainRatio);
					MountainRatio = FMath::Max(WestRatio, MountainRatio);
					MountainRatio = FMath::Max(NorthEastRatio, MountainRatio);
					MountainRatio = FMath::Max(SouthEastRatio, MountainRatio);
					MountainRatio = FMath::Max(NorthWestRatio, MountainRatio);
					MountainRatio = FMath::Max(SouthWestRatio, MountainRatio);			
					
					// create ratio to represent the mountain, 1 is the centre 0 is the edge
					float BlendRatio = FMath::Clamp((1 - MountainRatio) * 2.5, 0.f, 1.f);
					
					
					// Get the height of land					
					// add a large scale noise that shapes continents
					float Noise1 =
						FMath::PerlinNoise2D(
							FVector2D(
								WorldX + SeedOffsetX,
								WorldY + SeedOffsetY
							) * Noise1Frequency
						);			
					Noise1 = (Noise1 + 1) /2;			
			
					// add noise to continent height
					float ContinentHeight = ContinentBaseHeight + (Noise1 * Noise1Scale);
					
					// detail noise layers
					float Noise2 = FMath::PerlinNoise2D(FVector2D(
						WorldX + SeedOffsetX,
						WorldY + SeedOffsetY) * Noise2Frequency);
					
					float Noise3 = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * Noise3Frequency);
			
					// Add noise together to get height
					float LandHeight =  ContinentHeight + (Noise2 * Noise2Scale) + (Noise3 * Noise3Scale);	
					float Height = LandHeight;
					
					// hills 
					Height += Noise2 *Noise2Scale * MountainHillScale * (1 - BlendRatio); // hills on mountain borders
					Height += Noise1 *Noise1Scale/5 * MountainHillScale * (1 - BlendRatio); // hills on mountain borders

					// mountains
					float MountainMask = FMath::Pow(BlendRatio, MountainPower);
					float MountainNoise = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * MountainFrequency);
					MountainNoise = (MountainNoise + 1.f) * 0.5f;
					Height += MountainMask * (Noise1 * .5f * MountainHeight + MountainNoise * MountainHeight * 2);

					
					// ridges
					float RidgeNoise = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * RidgeNoiseFrequency);
					float Ridge = 1.f - FMath::Abs(RidgeNoise);
					Ridge = FMath::Pow(Ridge, RidgePowerRate) * BlendRatio;

					Height += Ridge * RidgeScale * MountainRatio;
					
					
					
					// set height in the chunk data heightmap
					chunkData.Heights[Index] = Height;
					break;
				}

			case ELocationType::Beach:
				{
					// Get the height of land
					// add a large scale noise that shapes continents
					float Noise1 =
						FMath::PerlinNoise2D(
							FVector2D(
								WorldX + SeedOffsetX,
								WorldY + SeedOffsetY
							) * Noise1Frequency
						);			
					Noise1 = (Noise1 + 1) /2;			
			
					float ContinentHeight = ContinentBaseHeight + (Noise1 * Noise1Scale);
					
					// detail noise layers
					float Noise2 = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * Noise2Frequency);
					float Noise3 = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * Noise3Frequency);
			
					// Add noise together to get height
					float LandHeight =  ContinentHeight + (Noise2 * Noise2Scale) + (Noise3 * Noise3Scale);		

					
					// get height of ocean 
					float OceanNoise1 = (FMath::PerlinNoise2D(FVector2D(
					WorldX + SeedOffsetX, 
					WorldY + SeedOffsetY)  * OceanFloorNoiseFrequency) + 1) / 2;				
				
					float OceanNoise2 = (FMath::PerlinNoise2D(FVector2D(
						WorldX + SeedOffsetX, 
						WorldY + SeedOffsetY)  * OceanFloorNoiseFrequency * 8) + 1) / 2;
				
					float NoiseHeight1 = OceanNoise1 * OceanFloorNoiseScale;
					float NoiseHeight2 = OceanNoise2 * OceanFloorNoiseScale /5;
					float OceanHeight = -OceanDepth + NoiseHeight1 + NoiseHeight2;
					
					// get ratio to determine proximity of closest valid beach
					float NorthRatio = (float)y / ChunkResolution;
					float SouthRatio = 1.f - NorthRatio;
					float EastRatio = (float)x / ChunkResolution;
					float WestRatio = 1.f - EastRatio;
					
					float NorthEastRatio = NorthRatio * EastRatio;
					float SouthEastRatio = SouthRatio * EastRatio;
					float NorthWestRatio = NorthRatio * WestRatio;
					float SouthWestRatio = SouthRatio * WestRatio;
					
					// if no beach is bordered, that ratio is invalid
					if (chunkData.NeighbourChunks[ENeighbourDirections::N] != ELocationType::Ocean) NorthRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::E] != ELocationType::Ocean) EastRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::S] != ELocationType::Ocean) SouthRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::W] != ELocationType::Ocean) WestRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::NE] != ELocationType::Ocean) NorthEastRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::SE] != ELocationType::Ocean) SouthEastRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::NW] != ELocationType::Ocean) NorthWestRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::SW] != ELocationType::Ocean) SouthWestRatio = 0.f;
						
					// get the closest beach
					float CoastRatio = 0.f;
					CoastRatio = FMath::Max(NorthRatio, CoastRatio);
					CoastRatio = FMath::Max(EastRatio, CoastRatio);
					CoastRatio = FMath::Max(SouthRatio, CoastRatio);
					CoastRatio = FMath::Max(WestRatio, CoastRatio);
					CoastRatio = FMath::Max(NorthEastRatio, CoastRatio);
					CoastRatio = FMath::Max(SouthEastRatio, CoastRatio);
					CoastRatio = FMath::Max(NorthWestRatio, CoastRatio);
					CoastRatio = FMath::Max(SouthWestRatio, CoastRatio);
					
					float BlendRatio = FMath::Clamp((1 - CoastRatio) * 3, 0.f, 1.f);
					float Height = FMath::Lerp(OceanHeight, LandHeight, BlendRatio); 
					
					// add hills to border of mountains
					float Hills = AddHillsToMountainBorder(chunkData, y, x, Noise2, Noise1);					
					Height += Hills;
					
					// set height in the chunk data heightmap
					chunkData.Heights[Index] = Height;
					break;
				}
			
			case ELocationType::Forest:
				{
					// add a large scale noise that shapes continents
					float Noise1 =
						FMath::PerlinNoise2D(
							FVector2D(
								WorldX + SeedOffsetX,
								WorldY + SeedOffsetY
							) * Noise1Frequency
						);			
					Noise1 = (Noise1 + 1) /2;
			
			
					float ContinentHeight = ContinentBaseHeight + (Noise1 * Noise1Scale);
					float Height = ContinentHeight;		

					// detail noise layers
					float Noise2 = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * Noise2Frequency);
					float Noise3 = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * Noise3Frequency);
			
					// Add noise together to get height
					Height += (Noise2 * Noise2Scale) + (Noise3 * Noise3Scale);							
					
					float Hills = AddHillsToMountainBorder(chunkData, y, x, Noise2, Noise1);					
					Height += Hills;
				
				
					// set height in the chunk data heightmap
					chunkData.Heights[Index] = Height;
					break;
				}
			
			case ELocationType::Swamp:
				{
					
						// get ratio to determine proximity of other chunks
					float NorthRatio = (float)y / ChunkResolution;
					float SouthRatio = 1.f - NorthRatio;
					float EastRatio = (float)x / ChunkResolution;
					float WestRatio = 1.f - EastRatio;
					
					float NorthEastRatio = NorthRatio * EastRatio;
					float SouthEastRatio = SouthRatio * EastRatio;
					float NorthWestRatio = NorthRatio * WestRatio;
					float SouthWestRatio = SouthRatio * WestRatio;
					
					// if swamp is on border, that ratio is not counted
					if (chunkData.NeighbourChunks[ENeighbourDirections::N] == ELocationType::Swamp)
						NorthRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::E] == ELocationType::Swamp)
						EastRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::S] == ELocationType::Swamp) 
						SouthRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::W] == ELocationType::Swamp)
						WestRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::NE] == ELocationType::Swamp)
						NorthEastRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::SE] == ELocationType::Swamp)
						SouthEastRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::NW] == ELocationType::Swamp)
						NorthWestRatio = 0.f;
					if (chunkData.NeighbourChunks[ENeighbourDirections::SW] == ELocationType::Swamp) 
						SouthWestRatio = 0.f;
					
								
					// get the closest neighbour ratio thats not a swamp
					float SwampRatio = 0.f;
					SwampRatio = FMath::Max(NorthRatio, SwampRatio);
					SwampRatio = FMath::Max(EastRatio, SwampRatio);
					SwampRatio = FMath::Max(SouthRatio, SwampRatio);
					SwampRatio = FMath::Max(WestRatio, SwampRatio);
					SwampRatio = FMath::Max(NorthEastRatio, SwampRatio);
					SwampRatio = FMath::Max(SouthEastRatio, SwampRatio);
					SwampRatio = FMath::Max(NorthWestRatio, SwampRatio);
					SwampRatio = FMath::Max(SouthWestRatio, SwampRatio);			
					
					// create ratio to represent the Swamp, 1 is the centre 0 is the edge
					float BlendRatio = FMath::Clamp((1 - SwampRatio) * 3, 0.f, 1.f);
					
					
					// add a large scale noise that shapes continents
					float Noise1 =
						FMath::PerlinNoise2D(
							FVector2D(
								WorldX + SeedOffsetX,
								WorldY + SeedOffsetY
							) * Noise1Frequency
						);			
					Noise1 = (Noise1 + 1) /2;
			
			
					float ContinentHeight = (
						ContinentBaseHeight + 
						Noise1 * Noise1Scale - 
						Noise1 * Noise1Scale * BlendRatio -
						ContinentBaseHeight * BlendRatio
						);
					
					float Height = ContinentHeight;		

					// detail noise layers
					float Noise2 = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * Noise2Frequency);
					float Noise3 = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * Noise3Frequency);
			
					// Add noise together to get height
					Height += (Noise2 * Noise2Scale) + (Noise3 * Noise3Scale);							
					Height += Noise3 * Noise3Scale * BlendRatio;
					
				
					float Hills = AddHillsToMountainBorder(chunkData, y, x, Noise2, Noise1);					
					Height += Hills;
					
					// set height in the chunk data heightmap
					chunkData.Heights[Index] = Height;
					break;
				}
				
			
			default:
				{							
					// add a large scale noise that shapes continents
					float Noise1 =
						FMath::PerlinNoise2D(
							FVector2D(
								WorldX + SeedOffsetX,
								WorldY + SeedOffsetY
							) * Noise1Frequency
						);			
					Noise1 = (Noise1 + 1) /2;
			
			
					float ContinentHeight = ContinentBaseHeight + (Noise1 * Noise1Scale);
					float Height = ContinentHeight;		

					// detail noise layers
					float Noise2 = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * Noise2Frequency);
					float Noise3 = FMath::PerlinNoise2D(FVector2D(WorldX + SeedOffsetX, WorldY + SeedOffsetY) * Noise3Frequency);
			
					// Add noise together to get height
					Height += (Noise2 * Noise2Scale) + (Noise3 * Noise3Scale);							
					
					float Hills = AddHillsToMountainBorder(chunkData, y, x, Noise2, Noise1);					
					Height += Hills;
					
					// set height in the chunk data heightmap
					chunkData.Heights[Index] = Height;
					break;
				}
			
			}		
			
			if (chunkData.bHasTown)
			{
				// get ratio to determine proximity of other chunks
				float NorthRatio = (float)y / ChunkResolution;
				float SouthRatio = 1.f - NorthRatio;
				float EastRatio = (float)x / ChunkResolution;
				float WestRatio = 1.f - EastRatio;
					
				float NorthEastRatio = NorthRatio * EastRatio;
				float SouthEastRatio = SouthRatio * EastRatio;
				float NorthWestRatio = NorthRatio * WestRatio;
				float SouthWestRatio = SouthRatio * WestRatio;
				
				// get the closest neighbour ratio
				float TownRatio = 0.f;
				TownRatio = FMath::Max(NorthRatio, TownRatio);
				TownRatio = FMath::Max(EastRatio, TownRatio);
				TownRatio = FMath::Max(SouthRatio, TownRatio);
				TownRatio = FMath::Max(WestRatio, TownRatio);
				TownRatio = FMath::Max(NorthEastRatio, TownRatio);
				TownRatio = FMath::Max(SouthEastRatio, TownRatio);
				TownRatio = FMath::Max(NorthWestRatio, TownRatio);
				TownRatio = FMath::Max(SouthWestRatio, TownRatio);			
					
				// create ratio to represent the town, 1 is the centre 0 is the edge
				float BlendRatio = FMath::Clamp((1 - TownRatio) * 4.f , 0.f, 1.f);
				
				// get the height of the chunk
				float Originalheight = chunkData.Heights[Index];
			
				// create a platform for the village
				float Height = FMath::Lerp(Originalheight, TownPlatformHeight, BlendRatio);
							
				// set height in the chunk data heightmap
				chunkData.Heights[Index] = Height;
			}
		}
	}

}

void AWorldGenerator::GenerateFoliageData(FChunkData& chunkData)
{
	
	ChunkSize = ChunkResolution * ChunkScale;
	
	int32 ChunkSeed = HashCombine(
	HashCombine(GetTypeHash(Seed), GetTypeHash(chunkData.WorldCoord.X)),
	GetTypeHash(chunkData.WorldCoord.Y));
	
	FRandomStream Stream(ChunkSeed);
	
	// loop vertices to place foliage
	for (int y = 1; y < ChunkResolution; y++)
	{
		for (int x = 1; x < ChunkResolution; x++)
		{
			// get the coordinate of the vertex from the array
			int Index = x + (y * (ChunkResolution + 1));
			FTransform Transform;

			float RandomOffsetX = Stream.FRandRange(-ChunkScale / 3, ChunkScale / 3);
			float RandomOffsetY = Stream.FRandRange(-ChunkScale / 3, ChunkScale / 3);			

			// get height
			float Height = chunkData.Heights[Index];			
			
			// set location in the transform
			Transform.SetLocation(FVector(x * ChunkScale, y * ChunkScale, Height));
			
			// get slope angle by comparing neighbouring heights
			float Offset = 100.f;
			
			// get neighbouring heights 
			int IndexL = Index; // left vertex index
			if (x > 0) IndexL =  (x - 1) + (y * (ChunkResolution + 1));
			float HL = chunkData.Heights[IndexL];
			
			int IndexR = Index; // right vertex index
			if (x < ChunkResolution)	IndexR =  (x + 1) + (y * (ChunkResolution + 1));
			float HR = chunkData.Heights[IndexR];
			
			int IndexD = Index; // south vertex index
			if (y > 0) IndexD =  x + ((y - 1) * (ChunkResolution + 1));
			float HD = chunkData.Heights[IndexD];
			
			int IndexU = Index; // north vertex index
			if (y < ChunkResolution) IndexU =  x + ((y + 1) * (ChunkResolution + 1));
			float HU = chunkData.Heights[IndexU];			
			
			// calculate slope angle
			FVector TangentX(2.f * Offset, 0.f, HR - HL);
			FVector TangentY(0.f, 2.f * Offset, HU - HD);
			FVector Normal = FVector::CrossProduct(TangentX, TangentY).GetSafeNormal();
			float Dot = FVector::DotProduct(Normal, FVector::UpVector);
			float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
			
			// random value that determines placement
			float Choice = Stream.FRand();
			
			// (x + y) % 2 == 0 this expression is used to create a checkerboard formation in foliage placement 
			
			switch (chunkData.Location)
			{
			case ELocationType::Forest:
				{
					// Spawn trees
					if (Choice < 0.2f && (x + y) % 2 == 0 && Angle <= 20.f && Height > 50.f)
					{
						// add random offset to foliage that are not cliffs	
						Transform.SetLocation(FVector(
							Transform.GetLocation().X + RandomOffsetX, 
							Transform.GetLocation().Y + RandomOffsetY,
							Height));
							
						// set a random rotation
						Transform.SetRotation(FRotator(
							Stream.FRandRange(-5.f, 5.f),
							Stream.FRandRange(0.f, 360.f), 
							Stream.FRandRange(-5.f, 5.f)).Quaternion());
							
						// set a random scale
						float xyScale = Stream.FRandRange(0.8, 1.2f);
						float zScale = Stream.FRandRange(0.8f, 1.5f);

						// apply scale
						Transform.SetScale3D(FVector(xyScale, xyScale, zScale));
						// add to data
						chunkData.TreeTransforms.Add(Transform);
					}		
					
					// Spawn plants
					else if (Choice < 0.3f && Angle <= 20.f && Height > 50.f)
					{
						// add random offset to foliage that are not cliffs	
						Transform.SetLocation(FVector(
							Transform.GetLocation().X + RandomOffsetX, 
							Transform.GetLocation().Y + RandomOffsetY,
							Height));
							
						// set a random rotation
						Transform.SetRotation(FRotator(
							Stream.FRandRange(-5.f, 5.f),
							Stream.FRandRange(0.f, 360.f), 
							Stream.FRandRange(-5.f, 5.f)).Quaternion());
							
						// set a random scale
						float xyScale = Stream.FRandRange(0.8, 1.2f);
						float zScale = Stream.FRandRange(0.8f, 1.5f);

						// apply scale
						Transform.SetScale3D(FVector(xyScale, xyScale, zScale));
						// add to data
						chunkData.PlantTransforms.Add(Transform);
					}					
								
					break;
				}
			
			case ELocationType::Swamp:
				{
					// Spawn plants
					if (Choice < 0.25f && Angle <= 10.f && Height > -20.f)
					{
						// add random offset to foliage that are not cliffs	
						Transform.SetLocation(FVector(
							Transform.GetLocation().X + RandomOffsetX, 
							Transform.GetLocation().Y + RandomOffsetY,
							Height));
							
						// set a random rotation
						Transform.SetRotation(FRotator(
							Stream.FRandRange(-5.f, 5.f),
							Stream.FRandRange(0.f, 360.f), 
							Stream.FRandRange(-5.f, 5.f)).Quaternion());
							
						// set a random scale
						float xyScale = Stream.FRandRange(1.f, 2.f);
						float zScale = Stream.FRandRange(1.f, 2.f);

						// apply scale
						Transform.SetScale3D(FVector(xyScale, xyScale, zScale));
						// add to data
						chunkData.PlantTransforms.Add(Transform);
					}					
					break;
				}
			
			}
		}
	}
}


int AWorldGenerator::GetLOD(FIntPoint Coord)
{	
	int LOD = 1;
	if (FMath::Abs(Coord.X) > LOD1RenderDistance || FMath::Abs(Coord.Y) > LOD1RenderDistance)
	{
		LOD = 2;
	}
			
	if (FMath::Abs(Coord.X) > LOD1RenderDistance + LOD2RenderDistance ||
		FMath::Abs(Coord.Y) > LOD1RenderDistance + LOD2RenderDistance)
	{
		LOD = 3;
	}

	if (FMath::Abs(Coord.X) > LOD1RenderDistance + LOD2RenderDistance + LOD3RenderDistance || 
		FMath::Abs(Coord.Y) > LOD1RenderDistance + LOD2RenderDistance + LOD3RenderDistance)
	{
		LOD = 4;
	}
	return LOD;
}

FIntPoint AWorldGenerator::GetLODTransitionBoundary(int LOD, FIntPoint Coord)
{
	int CurrentLODMaxDistance = 0;
	FIntPoint LODBoundary = FIntPoint(0, 0);

	if (LOD == 4) return LODBoundary; // LOD 4 doesnt need stitching

	// Get the distance required for chunk to be on edge of lod
	switch (LOD)
	{
	case 1: CurrentLODMaxDistance = LOD1RenderDistance;
		break;

	case 2: CurrentLODMaxDistance = LOD1RenderDistance + LOD2RenderDistance;
		break;

	case 3: CurrentLODMaxDistance = LOD1RenderDistance + LOD2RenderDistance + LOD3RenderDistance;
		break;
	}

	// check chunk is on the edge
	if (Coord.X ==  CurrentLODMaxDistance) LODBoundary.X =  1;
	if (Coord.X == -CurrentLODMaxDistance) LODBoundary.X = -1;

	if (Coord.Y ==  CurrentLODMaxDistance) LODBoundary.Y =  1;
	if (Coord.Y == -CurrentLODMaxDistance) LODBoundary.Y = -1;	

	return LODBoundary;
}


void AWorldGenerator::HandleChunkShifting()
{
	if (ShiftsToMake.X > 0)
	{
		ShiftsToMake.X--;
		ShiftLandChunks(FIntPoint(1,0));
		ShiftOceanChunks(FIntPoint(1, 0));
	}
	if (ShiftsToMake.X < 0)
	{
		ShiftsToMake.X++;
		ShiftLandChunks(FIntPoint(-1, 0));
		ShiftOceanChunks(FIntPoint(-1, 0));
	}
	if (ShiftsToMake.Y > 0)
	{
		ShiftsToMake.Y--;
		ShiftLandChunks(FIntPoint(0, 1));
		ShiftOceanChunks(FIntPoint(0, 1));
	}
	if (ShiftsToMake.Y < 0)
	{
		ShiftsToMake.Y++; ;
		ShiftLandChunks(FIntPoint(0, -1));
		ShiftOceanChunks(FIntPoint(0, -1));
	}
}

void AWorldGenerator::ShiftLandChunks(FIntPoint Direction)
{
	// total render distance of chunks of all LODs
	int TotalChunkViewDistance = 
		LOD1RenderDistance + 
			LOD2RenderDistance +
				LOD3RenderDistance + 
					LOD4RenderDistance;
	
	// Size of loaded chunks across one axis (in chunks)
	int ChunkAxisSize = (TotalChunkViewDistance * 2) + 1;
	
	// Distance chunk should move when being wrapped
	float ChunkMoveDistance = ChunkAxisSize * ChunkSize;
	
	// loop through rendered chunks
	for (auto& Pair : ChunkVisualisers)
	{
		AChunkVisualiser* Chunk = Pair.Value;
		
		bool bShouldRegenerate = false; // should chunk rebuild mesh
		bool bWrapChunk = false; // should chunk move to other edge
		
		// check if chunk should be moved
		if (Direction.X > 0)
		{
			bWrapChunk = Chunk->ChunkCoord.X == -TotalChunkViewDistance;
		}
		else if (Direction.X < 0)
		{
			bWrapChunk = Chunk->ChunkCoord.X == TotalChunkViewDistance;
		}
		else if (Direction.Y > 0)
		{
			bWrapChunk = Chunk->ChunkCoord.Y == -TotalChunkViewDistance;
		}
		else if (Direction.Y < 0)
		{
			bWrapChunk = Chunk->ChunkCoord.Y == TotalChunkViewDistance;
		}
		
		
		if (bWrapChunk) // if chunk is moved to other side of chunks
		{
			// get the chunk data
			FChunkData* Data = ChunkData.Find(Chunk->WorldCoord);
			if (Data)
			{
				// Remove any towns			
				if (Data->bHasTown && Data->bTownIsSpawned)
				{	
					ATownGenerator* TownGenerator = nullptr;
					// loop through all towns to find the one in data
					for (ATownGenerator* Town : TownGenerators)
					{
						if (Town->TownData.WorldPosition == Chunk->WorldCoord) // find town from data 
						{
							TownGenerator = Town; 
							break;
						}
					}
					TownGenerators.Remove(TownGenerator);
					DespawnTown(TownGenerator, *Data);	
					
				}
			}
		
			
			// move chunk to new edge
			FVector NewLocation =
				Chunk->GetActorLocation() +
				FVector(
					Direction.X * ChunkMoveDistance,
					Direction.Y * ChunkMoveDistance,
					0.f);

			Chunk->SetActorLocation(NewLocation);
			
			// Update Chunk Coordinates
			Chunk->ChunkCoord = FIntPoint(
				Chunk->ChunkCoord.X + (ChunkAxisSize - 1) * Direction.X,
				Chunk->ChunkCoord.Y + (ChunkAxisSize - 1) * Direction.Y);
			
			Chunk->WorldCoord = FIntPoint(
				Chunk->WorldCoord.X + ChunkAxisSize * Direction.X, 
				Chunk->WorldCoord.Y + ChunkAxisSize * Direction.Y);
				
			
			bShouldRegenerate = true; // wrapped chunks always need to rebuild mesh
			
			// clears the mesh of chunk 
			if (Chunk->Mesh)
			{
				Chunk->Mesh->ClearMeshSection(0);	
				Chunk->ClearFoliage();
			}		
		}
		else // chunk didn't move 
		{
			Chunk->ChunkCoord -= Direction; // update coord from player
		}
		
		// Update LODs and regenerate if changed
		int LastLOD = Chunk->LOD;
		int NewLOD = GetLOD(Chunk->ChunkCoord);
		if (NewLOD != LastLOD)
		{
			Chunk->LOD = NewLOD;
			bShouldRegenerate = true;
		}
		
		// get the direction of any LOD transitions
		FIntPoint LastLODBoundary = Chunk->LODBoundary;
		FIntPoint NewLODBoundary = GetLODTransitionBoundary(Chunk->LOD, Chunk->ChunkCoord);
		Chunk->LODBoundary = NewLODBoundary;
		if (LastLODBoundary != NewLODBoundary)
		{
			bShouldRegenerate = true;
		}
		
		if (bShouldRegenerate)
		{
			PendingChunkRebuilds.Add(Chunk);
		}
	}
	
	// sort chunks with LOD 1 generating first
	PendingChunkRebuilds.Sort(
		[](AChunkVisualiser& A, AChunkVisualiser& B)
		{
			return FMath::Max(
				FMath::Abs(A.ChunkCoord.X), 
				FMath::Abs(A.ChunkCoord.Y)) < FMath::Max(
					FMath::Abs(B.ChunkCoord.X), 
					FMath::Abs(B.ChunkCoord.Y));
		});	
}

void AWorldGenerator::ShiftOceanChunks(FIntPoint Direction)
{
	// total render distance of chunks of all LODs
	int TotalChunkViewDistance = 
	LOD1RenderDistance + 
		LOD2RenderDistance +
			LOD3RenderDistance + 
				LOD4RenderDistance;
	
	// Size of loaded chunks across one axis (in chunks)
	int ChunkAxisSize = (TotalChunkViewDistance * 2) + 1;
	
	// Distance chunk should move when being wrapped
	float ChunkMoveDistance = ChunkAxisSize * ChunkSize;
	
	// loop through rendered chunks
	for (auto& Pair : OceanChunks)
	{
		AOceanChunk* Chunk = Pair.Value;
		
		bool bShouldRegenerate = false; // should chunk rebuild mesh
		bool bWrapChunk = false; // should chunk move to other edge
		
		// check if chunk should be moved
		if (Direction.X > 0)
		{
			bWrapChunk = Chunk->ChunkCoord.X == -TotalChunkViewDistance;
		}
		else if (Direction.X < 0)
		{
			bWrapChunk = Chunk->ChunkCoord.X == TotalChunkViewDistance;
		}
		else if (Direction.Y > 0)
		{
			bWrapChunk = Chunk->ChunkCoord.Y == -TotalChunkViewDistance;
		}
		else if (Direction.Y < 0)
		{
			bWrapChunk = Chunk->ChunkCoord.Y == TotalChunkViewDistance;
		}
		
		if (bWrapChunk) // if chunk is moved to other side of chunks
		{
			// move chunk to new edge
			FVector NewLocation =
				Chunk->GetActorLocation() +
				FVector(
					Direction.X * ChunkMoveDistance,
					Direction.Y * ChunkMoveDistance,
					0.f);

			Chunk->SetActorLocation(NewLocation);
			
			// Update Chunk Coordinates
			Chunk->ChunkCoord = FIntPoint(
				Chunk->ChunkCoord.X + (ChunkAxisSize - 1) * Direction.X,
				Chunk->ChunkCoord.Y + (ChunkAxisSize - 1) * Direction.Y);
			
			Chunk->WorldCoord = FIntPoint(
				Chunk->WorldCoord.X + ChunkAxisSize * Direction.X, 
				Chunk->WorldCoord.Y + ChunkAxisSize * Direction.Y);
			
			
			
			bShouldRegenerate = true; // wrapped chunks always need to rebuild mesh
			
			// clears the mesh of chunk 
			if (Chunk->Mesh)
			{
				Chunk->Mesh->ClearMeshSection(0);				
			}
		}
		else // chunk didn't move 
		{
			Chunk->ChunkCoord -= Direction; // update coord from player
		}
		
		// Update LODs and regenerate if changed
		int LastLOD = Chunk->LOD;
		int NewLOD = GetLOD(Chunk->ChunkCoord);
		if (NewLOD != LastLOD)
		{
			Chunk->LOD = NewLOD;
			bShouldRegenerate = true;
		}
				
		if (bShouldRegenerate)
		{
			PendingOceanRebuilds.Add(Chunk);
		}
	}
	
	// sort chunks with LOD 1 generating first
	PendingOceanRebuilds.Sort(
		[](AOceanChunk& A, AOceanChunk& B)
		{
			return FMath::Max(
				FMath::Abs(A.ChunkCoord.X), 
				FMath::Abs(A.ChunkCoord.Y)) < FMath::Max(
					FMath::Abs(B.ChunkCoord.X), 
					FMath::Abs(B.ChunkCoord.Y));
		});
}

float AWorldGenerator::GetHeightAtLocation(float WorldX, float WorldY)
{
	ChunkSize = ChunkScale * ChunkResolution;

	// Convert world position to chunk coordinate
	int ChunkX = FMath::FloorToInt(WorldX / ChunkSize);
	int ChunkY = FMath::FloorToInt(WorldY / ChunkSize);

	FIntPoint ChunkCoord(ChunkX, ChunkY);	
	FChunkData* Chunk = ChunkData.Find(ChunkCoord);

	if (!Chunk)
	{
		return 0.f;
	}

	// Convert world position to local chunk space
	float LocalX = WorldX - (ChunkCoord.X * ChunkSize);
	float LocalY = WorldY - (ChunkCoord.Y * ChunkSize);

	// Convert to heightmap resolution
	int VertexCount = ChunkResolution + 1;

	float GridX = LocalX / ChunkScale;
	float GridY = LocalY / ChunkScale;

	int X0 = FMath::Clamp(FMath::FloorToInt(GridX), 0, ChunkResolution);
	int Y0 = FMath::Clamp(FMath::FloorToInt(GridY), 0, ChunkResolution);

	int Index = X0 + (Y0 * VertexCount);
	
	int X1 = FMath::Clamp(X0 + 1, 0, ChunkResolution);
	int Y1 = FMath::Clamp(Y0 + 1, 0, ChunkResolution);

	int BottomLeftIndex  = X0 + (Y0 * VertexCount);
	int BottomRightIndex = X1 + (Y0 * VertexCount);
	int TopLeftIndex     = X0 + (Y1 * VertexCount);
	int TopRightIndex    = X1 + (Y1 * VertexCount);

	float BottomLeft  = Chunk->Heights[BottomLeftIndex];
	float BottomRight = Chunk->Heights[BottomRightIndex];
	float TopLeft     = Chunk->Heights[TopLeftIndex];
	float TopRight    = Chunk->Heights[TopRightIndex];

	// Fraction inside the quad (0-1)
	float FracX = GridX - X0;
	float FracY = GridY - Y0;

	// Interpolate horizontally
	float Bottom = FMath::Lerp(BottomLeft, BottomRight, FracX);
	float Top = FMath::Lerp(TopLeft, TopRight, FracX);

	// Interpolate vertically
	return FMath::Lerp(Bottom, Top, FracY);
}

ELocationType AWorldGenerator::GetChunkBiomeAtLocation(float WorldX, float WorldY)
{
	// get the size of a chunk (length of 1 axis)
	ChunkSize = ChunkScale * ChunkResolution;

	// Convert world position to chunk coordinate
	int ChunkX = FMath::FloorToInt(WorldX / ChunkSize);
	int ChunkY = FMath::FloorToInt(WorldY / ChunkSize);

	// find the chunk data
	FIntPoint ChunkCoord(ChunkX, ChunkY);	
	FChunkData* Chunk = ChunkData.Find(ChunkCoord);

	// if data is out of world bounds, must be ocean
	if (!Chunk)
	{
		return ELocationType::Ocean;
	}
	return Chunk->Location;
}

void AWorldGenerator::SpawnTown(FChunkData& Data)
{	
	// if generator class isn't set return
	if (!TownGeneratorClass) return;
	
	ChunkSize = ChunkScale * ChunkResolution;
	
	// get the location of the chunk
	FVector WorldLocation = (
		FVector(Data.WorldCoord.X * ChunkSize,Data.WorldCoord.Y * ChunkSize,0 ) 
	);
	
	FTransform SpawnTransform = FTransform(
		FRotator::ZeroRotator, 
		WorldLocation, 
		FVector(0, 0, 0));
	
	// spawn the town, spawn is deferred so that town data is set
	ATownGenerator* TownGenerator = GetWorld()->SpawnActorDeferred<ATownGenerator>(
	   TownGeneratorClass,
	   SpawnTransform	   
   );
	
	// add data to the town 
	TownGenerator->TownData = Data.Town;
	TownGenerator->ChunkData = Data;
	
	// set the building actors in the town
	
	
	TownGenerator->BuildingClasses = BuildingClasses;
	TownGenerator->TownCentreClass = TownCentreClass;
	TownGenerator->JobBoardClass = JobBoardClass;
	TownGenerator->TownHeight = TownPlatformHeight;
	TownGenerator->ChunkSize = ChunkSize;
	
	UGameplayStatics::FinishSpawningActor(TownGenerator, SpawnTransform);
	
	// update bool in chunk data
	Data.bTownIsSpawned = true;
	
	
	// add town generator to array
	TownGenerators.Add(TownGenerator);
	
	
	TownGenerator->CreateTown();

}

void AWorldGenerator::DespawnTown(ATownGenerator* Town, FChunkData& Data)
{
	// save the town data to chunk data
	Data.Town = Town->TownData;
	
	// update bool in chunk data
	Data.bTownIsSpawned = false;
	
	// destroy town generator and remove from array
	Town->ClearTown();
	Town->Destroy();
	
}

void AWorldGenerator::SetTownData()
{	
	// get the total amount of building variations 
	int MaxBuildingTypes = BuildingClasses.Num();
	
	int Index = 0;
	for (FTown& Town : Towns)
	{
		Town.TownId = Index;
		Index++;
		
		int TownSeed = HashCombine(
			GetTypeHash(Seed),
			HashCombine(
				GetTypeHash(Town.WorldPosition.X),
				GetTypeHash(Town.WorldPosition.Y)
			)
		);

		FRandomStream RandomStream(TownSeed);		
		
		int BuildingSlots = RandomStream.RandRange(5, 8);
		
		Town.BuildingCount = BuildingSlots;
		
		// create an array of valid building types
		TArray<int> ValidBuildingTypes;
		for (int i = 0; i < MaxBuildingTypes; i++)
		{
			ValidBuildingTypes.Add(i);
		}
		
		// randomly pick different buildings
		for (int i =0 ; i < BuildingSlots; i++)
		{
			int BuildingIndex = RandomStream.RandRange(0, ValidBuildingTypes.Num() - 1);
			Town.Buildings.Add(i, ValidBuildingTypes[BuildingIndex]);
			ValidBuildingTypes.Remove(BuildingIndex);
		}		
		
		
		// place buildings in a circle
		float Radius = 2500.f;
		int BuildingCount = Town.BuildingCount;
		for (int i = 0; i < BuildingCount; i++)
		{
			float Angle = (2.f * PI * i) / BuildingCount;

			float X = FMath::Cos(Angle) * Radius;
			float Y = FMath::Sin(Angle) * Radius;

			FVector BuildingPosition(
				X,
				Y,
				i * 360 / BuildingCount // rotation of the building
			);
			
			
			Town.BuildingsLocations.Add(i, BuildingPosition);
		}
		
		FChunkData* Data = ChunkData.Find(Town.WorldPosition);

		if (Data)
		{
			Data->Town = Town;
			Data->bHasTown = true;
		}
	}
}




