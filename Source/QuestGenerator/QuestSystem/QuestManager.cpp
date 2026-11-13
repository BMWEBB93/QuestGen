// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManager.h"

#include "Kismet/GameplayStatics.h"
#include "QuestGenerator/WorldGenerator/WorldGenerator.h"
#include "QuestGenerator/WorldGenerator/ChunkData.h"




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
	for (UQuest* Quest : AvailableQuests)
	{
		if (Quest->QuestId == NewQuest->QuestId)
		{
			Quest->QuestState = EObjectiveState::InProgress;
			ActiveQuests.Add(Quest);
		}
	}
}

void UQuestManager::CheckQuestCompletion(UQuest* Quest)
{
}
