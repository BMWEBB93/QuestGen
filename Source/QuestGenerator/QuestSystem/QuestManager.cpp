// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManager.h"

#include "Kismet/GameplayStatics.h"
#include "QuestGenerator/WorldGenerator/WorldGenerator.h"




void UQuestManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// create the quest generator
	QuestGenerator = NewObject<UProceduralQuestGenerator>(this);		
	QuestGenerator->Initialise(this);
}

void UQuestManager::Deinitialize()
{
	Super::Deinitialize();
}

UQuest* UQuestManager::RequestQuest(FTown TownData)
{
	// if world generator is empty, get it from the world
	if (WorldGenerator == nullptr)
	{
		WorldGenerator = Cast<AWorldGenerator>(
	UGameplayStatics::GetActorOfClass(
		GetWorld(),
		AWorldGenerator::StaticClass()));
	}
	
	// generate quest
	UQuest* quest = QuestGenerator->GenerateQuest(TownData);
	AvailableQuests.Add(quest);	
	
	return quest;
}

void UQuestManager::AcceptQuest(UQuest* NewQuest)
{
	
	if (!NewQuest)
	{
		return;
	}
	
	// loop through available quests
	for (UQuest* Quest : AvailableQuests)
	{
		if (!Quest)
		{
			continue;
		}
		
		// if quest id mathces, add to active quests
		if (Quest->QuestId == NewQuest->QuestId)
		{
			Quest->QuestState = EObjectiveState::InProgress;
			ActiveQuests.Add(Quest);
			
			return;
		}
	}

}

void UQuestManager::HandleObjectiveComplete(UQuest* Quest)
{
	// set objective state to complete 
	Quest->Objectives[Quest->ActiveObjective]->ObjectiveState = EObjectiveState::Completed;
	
	// increment active objective index
	Quest->ActiveObjective++; 
			
	// if active objective becomes out of bounds 
	if (Quest->ActiveObjective >= Quest->Objectives.Num())
	{
		// set to 0 and set objective state to complete
		Quest->ActiveObjective = 0;
		Quest->QuestState = EObjectiveState::Completed;
		
		// update arrays
		ActiveQuests.Remove(Quest);
		CompletedQuests.Add(Quest);
	}
}

bool UQuestManager::CheckLocationQuestCompletion(FIntPoint LocationToCheck)
{
	// loop active quests
	for (UQuest* Quest : ActiveQuests)
	{
		// check the active objective is a location
		if (Quest->Objectives[Quest->ActiveObjective]->ObjectiveType != EObjectiveType::Location)
		{
			continue;
		}
		// if the active objective location matches the location to check
		if (Quest->Objectives[Quest->ActiveObjective]->TargetLocation == LocationToCheck)
		{
			// complete objective
			HandleObjectiveComplete(Quest);
			return true;
		}
	}
	return false;
}

bool UQuestManager::CheckGatherQuestCompletion(int ItemType)
{
	// loop active quests
	for (UQuest* Quest : ActiveQuests)
	{
		// check the active objective is an item type
		if (Quest->Objectives[Quest->ActiveObjective]->ObjectiveType != EObjectiveType::Gather)
		{
			continue;
		}
		// if the active objective item type matches the item type to check
		if (Quest->Objectives[Quest->ActiveObjective]->TargetItemType == ItemType)
		{
			// increment the item count in the objective
			Quest->Objectives[Quest->ActiveObjective]->CurrentCount++;
			
			// if the item count reaches max count, complete the objective
			if (Quest->Objectives[Quest->ActiveObjective]->CurrentCount >= Quest->Objectives[Quest->ActiveObjective]->TargetCount)
			{
				// complete objective
				HandleObjectiveComplete(Quest);
			}
			return true;
		}
	}
	return false;
}

bool UQuestManager::CheckRetrieveQuestCompletion(int ItemID)
{
	// loop active quests
	for (UQuest* Quest : ActiveQuests)
	{
		// check the active objective is an item id
		if (Quest->Objectives[Quest->ActiveObjective]->ObjectiveType != EObjectiveType::GetItem)
		{
			continue;
		}
		// if the active objective id matches the id to check
		if (Quest->Objectives[Quest->ActiveObjective]->TargetItem == ItemID)
		{
			// complete objective
			HandleObjectiveComplete(Quest);
			return true;
		}
	}
	return false;
}

bool UQuestManager::CheckKillQuestCompletion(int NPCId)
{
	// loop active quests
	for (UQuest* Quest : ActiveQuests)
	{
		// check the active objective is a npc
		if (Quest->Objectives[Quest->ActiveObjective]->ObjectiveType != EObjectiveType::Kill)
		{
			continue;
		}
		// if the active objective npc matches the npc to check
		if (Quest->Objectives[Quest->ActiveObjective]->TargetItemType == NPCId)
		{
			// complete objective
			HandleObjectiveComplete(Quest);
			return true;
		}
	}
	return false;
}
