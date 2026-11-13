// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralQuestGenerator.h"
#include "QuestGenerator/BaseCharacter.h"

#include "Kismet/GameplayStatics.h"

void UProceduralQuestGenerator::Initialise(UQuestManager* Manager)
{
	QuestManager = Manager;
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
				Objective->ObjectiveType = ObjectiveType::Location;				
				
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
				
				Quest->Objectives.Add(Objective); // add objective to the quest				
				
				break;
			}
		
		case EQuestType::Bounty:
			{				
				UObjective* Objective = NewObject<UObjective>(Quest);
					
				// get other town locations 
				for (FTown Town : WorldGenerator->Towns)
				{
						
				}
					
					
				//WorldGenerator->ChunkData
					
				break;
			}
	
		case EQuestType::Gather:
			{				
				UObjective* Objective = NewObject<UObjective>(Quest);
						
			
						
						
				//WorldGenerator->ChunkData
						
				break; 
			}
		
		case EQuestType::Retrieval:
			{				
				UObjective* Objective = NewObject<UObjective>(Quest);
							
				
							
							
				//WorldGenerator->ChunkData
							
				break;
			}
		
	
	}
	
	// set the town ID
	Quest->TownId = TownData.TownId;
	
	
	
	
	return Quest;
}
