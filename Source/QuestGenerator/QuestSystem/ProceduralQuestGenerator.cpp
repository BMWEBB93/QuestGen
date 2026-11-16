// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralQuestGenerator.h"
#include "QuestGenerator/BaseCharacter.h"
#include "QuestGenerator/BaseItem.h"

#include "Kismet/GameplayStatics.h"

void UProceduralQuestGenerator::Initialise(UQuestManager* Manager)
{
	QuestManager = Manager;
}

FIntPoint UProceduralQuestGenerator::FindValidNeighbourCoord(FIntPoint TownPosition, TArray <ELocationType> ValidLocations)
{
	// initialise array of possible locations
	TArray<FIntPoint> PossibleLocations; 
	// loop through surrounding coordinates
	for (int x = -2; x <= 2; x+= 2)
	{
		for (int y = -2; y <= 2; y+= 2)
		{
			// skip the towns location
			if (x == 0 && y == 0) continue;
						
			// get coord of locaiton to check
			FIntPoint LocationToCheck = TownPosition + FIntPoint(x, y);
			// get the data
			FChunkData* ChunkData = WorldGenerator->ChunkData.Find(LocationToCheck);
						
			if (!ChunkData) // skip chunks out of bounds
			{
				continue;
			}						

			// check location is valid and add if so
			if (ValidLocations.Contains(ChunkData->Location)) 
			{
				PossibleLocations.Add(FIntPoint(x, y));
			}							
		}
	}
	// if locations are found randomly pick one
	if (PossibleLocations.Num() > 0)
	{
		int LocationIndex = FMath::RandRange(0, PossibleLocations.Num() - 1);
		return PossibleLocations[LocationIndex];
	}
	
	// if no locations are found target is in the town
	return FIntPoint(0, 0);
}

FTransform UProceduralQuestGenerator::FindValidTransformInChunk(FChunkData* TargetData)
{
	// initialise item position
	FVector ItemPosition = FVector(0.f, 0.f, 0.f);
				
	// keep checking positions for valid locations
	bool bIsLocationValid = false;
	while (!bIsLocationValid)
	{
		// Random position inside chunk
		float X = FMath::FRandRange(0.f, WorldGenerator->ChunkSize);
		float Y = FMath::FRandRange(0.f, WorldGenerator->ChunkSize);

		// Get height from terrain data
		FVector ChunkLocation = FVector (
			TargetData->WorldCoord.X * WorldGenerator->ChunkSize + X, 
			TargetData->WorldCoord.Y * WorldGenerator->ChunkSize + Y,
			0
		);		
				
		// get the height at this position
		float Height = WorldGenerator->GetHeightAtLocation(ChunkLocation.X, ChunkLocation.Y);
					 
		// if the location is valid continue with tis location
		if (Height > 0.f)
		{
			ItemPosition = FVector(X, Y, Height);
			bIsLocationValid = true;
		}
	}

	// create random rotation
	FRotator ItemRotation = FRotator(
		0.f,
		FMath::FRandRange(0.f, 360.f),
		0.f
	);
	
	// Create transform
	FTransform ItemTransform(
		ItemRotation,
		ItemPosition,
		FVector(1.f)
	);
	
	return ItemTransform;
}

UQuest* UProceduralQuestGenerator::GenerateQuest(FTown TownData)
{
	// get the world 
	if (WorldGenerator == nullptr)
	{
		WorldGenerator = Cast<AWorldGenerator>(
	UGameplayStatics::GetActorOfClass(
		GetWorld(),
		AWorldGenerator::StaticClass()));
	}
	
	// create the quest
	UQuest* Quest = NewObject<UQuest>(this);	
	Quest->QuestId = NextQuestID;
	NextQuestID++;
	
	// Generate random quest type
	// create array of all quest types
	TArray<EQuestType> QuestTypes = 
	{
		EQuestType::Delivery,
		EQuestType::Gather,
		EQuestType::Bounty,
		EQuestType::Retrieval
	};	
	// get a random index
	int32 RandomQuestType = FMath::RandRange(0, 3);	
	Quest->QuestType = QuestTypes[RandomQuestType]; // use random index to set a random quest type
	
	switch (QuestTypes[RandomQuestType])
	{
		case EQuestType::Delivery:
			{							
				// create a 'go to location' objective
				UObjective* Objective = NewObject<UObjective>(Quest);		
				Objective->ObjectiveType = EObjectiveType::Location;				
				
				// get other town locations 
				TArray<FIntPoint> Coordinates;
				for (FTown Town : WorldGenerator->Towns)
				{
					if (Town.TownId == TownData.TownId) // skip current town
					{
						continue;
					}
					// add to array
					Coordinates.Add(Town.WorldPosition);
				}
				// set the objective location to the random town
				int RandomIndex = FMath::RandRange(0, Coordinates.Num() - 1);
				Objective->TargetLocation = Coordinates[RandomIndex];	
				
				// get the town
				FTown TargetTown = WorldGenerator->ChunkData.Find(Coordinates[RandomIndex])->Town;
				
				// set the description and name of quest
				Quest->QuestName = FText::Format(
					FText::FromString("Deliver supplies to {0}"),
					TargetTown.TownName
				);
				
				Quest->QuestDescription = FText::Format(
					FText::FromString("Deliver supplies to {0}"),
					TargetTown.TownName
				);
				
				Objective->ObjectiveDescription = FText::Format(				
					FText::FromString("Head to {0}"),
					TargetTown.TownName
				);
				
				
				// set progress state
				Objective->ObjectiveState = EObjectiveState::InProgress;
				Quest->Objectives.Add(Objective); // add objective to the quest				
				
				break;
			}
		
		case EQuestType::Bounty:
			{				
			UObjective* Objective = NewObject<UObjective>(Quest);
							
				// create a 'go to location' objective
				Objective->ObjectiveType = EObjectiveType::Kill;	
				
				// 0 = Warrior, 1 = Peasant man, 2 = Peasant Girl
				// Randomly pick an npc 
				int RandomNPC = FMath::RandRange(0, WorldGenerator->NPCClasses.Num() - 1);				
				
				// check surrounding chunks 2 away for valid positions
				TArray<ELocationType> ValidLocations = {
					ELocationType::Grassland,
					ELocationType::Forest,
					ELocationType::Swamp					
				};			
				
				// get chunk location for the item 
				FIntPoint NPCLocation = FindValidNeighbourCoord(TownData.WorldPosition, ValidLocations);
				
				
				// get the direction the item is located
				FString Direction;
				
				if (NPCLocation.X > 0) { Direction = TEXT("North"); }
				else if (NPCLocation.X < 0) { Direction = TEXT("South"); }
				
				if (NPCLocation.Y > 0) { Direction += TEXT("East"); }
				else if (NPCLocation.Y < 0) { Direction += TEXT("West"); }				
				
				// Get target chunk data
				FChunkData* TargetData = WorldGenerator->ChunkData.Find(TownData.WorldPosition + NPCLocation);

				// get the location type to display as a string
				FString TargetBiome = UEnum::GetDisplayValueAsText(TargetData->Location).ToString();
				
				
				// find transform for item in the chunk
				FTransform NPCTransform = FindValidTransformInChunk(TargetData);
								
				
				int QuestNPC = WorldGenerator->CreateNPC(
					NPCLocation + TownData.WorldPosition, 
					RandomNPC,
					NPCTransform
				);
				
				// get the name of the NPC 
				FString Name = WorldGenerator->NPCClasses[RandomNPC]
					->GetDefaultObject<ANpc>()->Name;				
				
				
				
				// set the name of quest
				Quest->QuestName = FText::Format(
					FText::FromString("Find and kill {0}"),
					FText::FromString(Name)
				);
				
				// set the description of quest
				Quest->QuestDescription = FText::Format(
				FText::FromString("Find {0}, they were last seen in the {1} to the {2}"),
					FText::FromString(Name),
					FText::FromString(TargetBiome),
					FText::FromString(Direction)
				);
				
				// set the description of objective
				Objective->ObjectiveDescription = FText::Format(				
				FText::FromString("Find {0}, they were last seen in the {1} to the {2}"),
					FText::FromString(Name),
					FText::FromString(TargetBiome),
					FText::FromString(Direction)
				);
				
				
				// set progress state
				Objective->TargetCharacter = QuestNPC;
				Objective->ObjectiveState = EObjectiveState::InProgress;
				Quest->Objectives.Add(Objective); // add objective to the quest						
					
				break;
			}
	
		case EQuestType::Gather:
			{				
				UObjective* Objective = NewObject<UObjective>(Quest);
					
				// set gather amount for the objective
				int GatherAmount = 5;
				Objective->TargetCount = GatherAmount;

				// create a 'gather' objective
				Objective->ObjectiveType = EObjectiveType::Gather;				
				
				// get the chunk data the town belongs to
				FChunkData* ChunkData = WorldGenerator->ChunkData.Find(TownData.WorldPosition);
	
				
				// get nearby woods and swamp				
				int NearbyWoods = 0;
				int NearbySwamp = 0;
				
				// loop neighbour directions in the chunk and increment based on location type
				for (const TPair<ENeighbourDirections, ELocationType>& Neighbour : ChunkData->NeighbourChunks)
				{
					if (Neighbour.Value == ELocationType::Forest)
					{
						NearbyWoods++;
					}
					else if (Neighbour.Value == ELocationType::Swamp)
					{
						NearbySwamp++;
					}
				}
				// 0 = mushroom, 1 = wood
				int ItemType; // class of actor to spawn
				
				// if swamp is nearby player collects mushrooms, else collect wood
			
				if (NearbySwamp > NearbyWoods)
				{
					ItemType = 0;
				}
				else
				{
					ItemType = 1;
				}
				
				// set the type of item to collect in the objective
				Objective->TargetItemType = ItemType;
				
				// get the name of the object to gather
				FString ItemName = WorldGenerator->ItemClasses[ItemType]
					->GetDefaultObject<ABaseItem>()->ItemName;				
				
				
				
				// set the name of quest
				Quest->QuestName = FText::Format(
					FText::FromString("Gather {0} {1}"),
					FText::AsNumber(GatherAmount),
					FText::FromString(ItemName)
				);
				
				// set the description of quest
				Quest->QuestDescription = FText::Format(
				FText::FromString("Gather {0} {1}"),
					FText::AsNumber(GatherAmount),
					FText::FromString(ItemName)
				);
				
				// set the description of objective
				Objective->ObjectiveDescription = FText::Format(				
				FText::FromString("Gather {0} {1}"),
				FText::AsNumber(GatherAmount),
				FText::FromString(ItemName)
				);
				
				
				// set progress state
				Objective->ObjectiveState = EObjectiveState::InProgress;
				Quest->Objectives.Add(Objective); // add objective to the quest								
						
				break; 
			}
		
		case EQuestType::Retrieval:
			{
				UObjective* Objective = NewObject<UObjective>(Quest);
							
				// create a 'go to location' objective
				Objective->ObjectiveType = EObjectiveType::GetItem;	
				
				// 2 = Shovel, 3 = Sword, 4 = Longsword, 5 = Dagger
				// Randomly pick an item to retrieve
				int RandomItem = FMath::RandRange(2, WorldGenerator->ItemClasses.Num() - 1);				
				
				// check surrounding chunks 2 away for valid positions
				TArray<ELocationType> ValidLocations = {
					ELocationType::Grassland,
					ELocationType::Forest,
					ELocationType::Swamp					
				};			
				
				// get chunk location for the item 
				FIntPoint ItemLocation = FindValidNeighbourCoord(TownData.WorldPosition, ValidLocations);
				
				
				// get the direction the item is located
				FString Direction;
				
				if (ItemLocation.X > 0) { Direction = TEXT("North"); }
				else if (ItemLocation.X < 0) { Direction = TEXT("South"); }
				
				if (ItemLocation.Y > 0) { Direction += TEXT("East"); }
				else if (ItemLocation.Y < 0) { Direction += TEXT("West"); }				
				
				// Get target chunk data
				FChunkData* TargetData = WorldGenerator->ChunkData.Find(TownData.WorldPosition + ItemLocation);

				// get the location type to display as a string
				FString TargetBiome = UEnum::GetDisplayValueAsText(TargetData->Location).ToString();
				
				
				// find transform for item in the chunk
				FTransform ItemTransform = FindValidTransformInChunk(TargetData);
								
				
				int QuestItem = WorldGenerator->CreateQuestItem(ItemLocation + TownData.WorldPosition, RandomItem, ItemTransform);
				
				// get the name of the object to gather
				FString ItemName = WorldGenerator->ItemClasses[RandomItem]
					->GetDefaultObject<ABaseItem>()->ItemName;				
				
				
				
				// set the name of quest
				Quest->QuestName = FText::Format(
					FText::FromString("Find the {0}"),
					FText::FromString(ItemName)
				);
				
				// set the description of quest
				Quest->QuestDescription = FText::Format(
				FText::FromString("Find the {0}, it was last seen in the {1} to the {2}"),
					FText::FromString(ItemName),
					FText::FromString(TargetBiome),
					FText::FromString(Direction)
				);
				
				// set the description of objective
				Objective->ObjectiveDescription = FText::Format(				
				FText::FromString("Find the {0}, it was last seen in the {1} to the {2}"),
					FText::FromString(ItemName),
					FText::FromString(TargetBiome),
					FText::FromString(Direction)
				);
				
				
				// set progress state
				Objective->TargetItem = QuestItem;
				Objective->ObjectiveState = EObjectiveState::InProgress;
				Quest->Objectives.Add(Objective); // add objective to the quest						
				
				break;
			}	
	}
	
	// set the town ID
	Quest->TownId = TownData.TownId;
	
	
	
	
	return Quest;
}
