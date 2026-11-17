// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManager.h"

#include "Kismet/GameplayStatics.h"
#include "QuestGenerator/WorldGenerator/WorldGenerator.h"




void UQuestManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	QuestGenerator = NewObject<UProceduralQuestGenerator>(this);		
	QuestGenerator->Initialise(this);
}

void UQuestManager::Deinitialize()
{
	Super::Deinitialize();
}

UQuest* UQuestManager::RequestQuest(FTown TownData)
{
	if (WorldGenerator == nullptr)
	{
		WorldGenerator = Cast<AWorldGenerator>(
	UGameplayStatics::GetActorOfClass(
		GetWorld(),
		AWorldGenerator::StaticClass()));
	}
	
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
	
	for (UQuest* Quest : AvailableQuests)
	{
		if (!Quest)
		{
			continue;
		}
		
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
	Quest->Objectives[Quest->ActiveObjective]->ObjectiveState = EObjectiveState::Completed;
	Quest->ActiveObjective++; 
			
	if (Quest->ActiveObjective >= Quest->Objectives.Num())
	{
		Quest->ActiveObjective = 0;
		Quest->QuestState = EObjectiveState::Completed;
		
		ActiveQuests.Remove(Quest);
		CompletedQuests.Add(Quest);
	}
}

void UQuestManager::CheckLocationQuestCompletion(FIntPoint LocationToCheck)
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
		}
	}
}

void UQuestManager::CheckGatherQuestCompletion(int ItemType)
{
	// loop active quests
	for (UQuest* Quest : ActiveQuests)
	{
		// check the active objective is a location
		if (Quest->Objectives[Quest->ActiveObjective]->ObjectiveType != EObjectiveType::Gather)
		{
			continue;
		}
		// if the active objective location matches the location to check
		if (Quest->Objectives[Quest->ActiveObjective]->TargetItemType == ItemType)
		{
			// complete objective
			HandleObjectiveComplete(Quest);
		}
	}
}

void UQuestManager::CheckRetrieveQuestCompletion(int ItemID)
{
	// loop active quests
	for (UQuest* Quest : ActiveQuests)
	{
		// check the active objective is a location
		if (Quest->Objectives[Quest->ActiveObjective]->ObjectiveType != EObjectiveType::GetItem)
		{
			continue;
		}
		// if the active objective location matches the location to check
		if (Quest->Objectives[Quest->ActiveObjective]->TargetItem == ItemID)
		{
			// complete objective
			HandleObjectiveComplete(Quest);
		}
	}
}

void UQuestManager::CheckKillQuestCompletion(int NPCId)
{
	// loop active quests
	for (UQuest* Quest : ActiveQuests)
	{
		// check the active objective is a location
		if (Quest->Objectives[Quest->ActiveObjective]->ObjectiveType != EObjectiveType::Kill)
		{
			continue;
		}
		// if the active objective location matches the location to check
		if (Quest->Objectives[Quest->ActiveObjective]->TargetItemType == NPCId)
		{
			// complete objective
			HandleObjectiveComplete(Quest);
		}
	}
}
