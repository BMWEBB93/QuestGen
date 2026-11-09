// Fill out your copyright notice in the Description page of Project Settings.


#include "GrassGenerator.h"

#include "WorldGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "GrassChunk.h"
#include "GameFramework/Character.h"


// Sets default values
AGrassGenerator::AGrassGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGrassGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrassGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!Player)
	{
		Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	}
    
	if (Player && bIsGrassGenerated)
	{
		// get the distance the player is from the start
		FVector PlayerLocation = Player->GetActorLocation();
		FVector Delta = PlayerLocation - LastPlayerLocation;

		PlayerDelta += Delta;
		LastPlayerLocation = PlayerLocation;



		// shift the chunks towards the direction the player moves
		if (PlayerDelta.X > TileSize)
		{
			ShiftGrass(FIntPoint(1, 0));
			PlayerDelta.X -= TileSize;
		}
		if (PlayerDelta.X < 0.f)
		{
			ShiftGrass(FIntPoint(-1, 0));
			PlayerDelta.X += TileSize;
		}
		if (PlayerDelta.Y > TileSize)
		{
			ShiftGrass(FIntPoint(0, 1));
			PlayerDelta.Y -= TileSize;
		}
		if (PlayerDelta.Y < 0.f)
		{
			ShiftGrass(FIntPoint(0, -1));
			PlayerDelta.Y += TileSize;
		}
	}
}

void AGrassGenerator::CreateGrass()
{
	WorldGenerator = Cast<AWorldGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(),AWorldGenerator::StaticClass()));
	TownPlatformHeight = WorldGenerator->TownPlatformHeight;
	
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	// player starts in the middle of the chunk
	PlayerDelta += FVector(TileSize / 2, TileSize / 2, 0);

	for (AGrassChunk* chunk : GrassChunks)
	{
		chunk->Destroy();
	}
	GrassChunks.Empty();


	int grassAxisLength = (TileRange * 2) + 1;

	// start spawning chunks from the bottom left
	FVector GenerationStartPoint = FVector(-grassAxisLength * TileSize / 2, -grassAxisLength * TileSize / 2, 0);
	
	 for (int y = 0; y < grassAxisLength; y++)
    {
        for (int x = 0; x < grassAxisLength; x++)
        {
            FIntPoint Coord = FIntPoint(x - TileRange, y - TileRange);


            FVector SpawnLocation = GenerationStartPoint + FVector(x * TileSize, y * TileSize, 0);
            AGrassChunk* Chunk = 
                GetWorld()->SpawnActor<AGrassChunk>(
                    AGrassChunk::StaticClass(),
                    SpawnLocation,
                    FRotator::ZeroRotator
                );

            // set grass meshes
            Chunk->GrassMesh = GrassMesh;
    
        	Chunk->Coord = Coord;
        	Chunk->ChunkSize = TileSize;
        	Chunk->Density = Density;
        	Chunk->WorldGenerator = WorldGenerator;
        	Chunk->CullDistance = CullDistance;
        	
        	Chunk->GrassMaxHeight = GrassMaxHeight;
        	Chunk->GrassMaxSlopeAngle = GrassMaxSlopeAngle;
        	
        	Chunk->TownPlatformHeight = TownPlatformHeight;
        	
            Chunk->SetHISMs(CullDistance);
            Chunk->GenerateGrass();

            GrassChunks.Add(Chunk);
            
            Chunk->SetFolderPath(TEXT("Grass"));

        }
    }
    bIsGrassGenerated = true;
}

void AGrassGenerator::ShiftGrass(FIntPoint Direction)
{
	int ChunkAxisLength = TileRange * 2 + 1;
	float MoveDistance = TileSize * ChunkAxisLength;
	//  chunks
	for (AGrassChunk* GrassChunk : GrassChunks)
	{
		bool bWrapChunk = false; // should chunk move to other edge

		if (Direction.X > 0)
		{
			bWrapChunk = GrassChunk->Coord.X == -TileRange;
		}
		else if (Direction.X < 0)
		{
			bWrapChunk = GrassChunk->Coord.X == TileRange;
		}
		else if (Direction.Y > 0)
		{
			bWrapChunk = GrassChunk->Coord.Y == -TileRange;
		}
		else if (Direction.Y < 0)
		{
			bWrapChunk = GrassChunk->Coord.Y == TileRange;
		}


		if (bWrapChunk) // if chunk is on the oppesite edge of movement
		{
			GrassChunk->ClearGrass();
			// move chunk to new edge
			FVector NewLocation =
				GrassChunk->GetActorLocation() +
				FVector(
					Direction.X * MoveDistance,
					Direction.Y * MoveDistance,
					0.f);

			GrassChunk->SetActorLocation(NewLocation);

			GrassChunk->Coord = FIntPoint(GrassChunk->Coord.X + (ChunkAxisLength - 1) * Direction.X, GrassChunk->Coord.Y + (ChunkAxisLength - 1) * Direction.Y);

			GrassChunk->GenerateGrass();
            
		}
		else
		{
			GrassChunk->Coord -= Direction;
		}
	}
}

