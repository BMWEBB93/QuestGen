// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManager.h"

void UQuestManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(LogTemp, Warning, TEXT("QuestManager Initialising"));

	
	QuestGenerator = NewObject<UProceduralQuestGenerator>(this);
	
	UE_LOG(LogTemp, Warning, TEXT("QuestGenerator Created: %s"),
		IsValid(QuestGenerator) ? TEXT("Valid") : TEXT("NULL"));
	
	QuestGenerator->Initialise(this);
}

void UQuestManager::Deinitialize()
{
	Super::Deinitialize();
}

UQuest* UQuestManager::RequestQuest(FTown TownData)
{
	UQuest* quest = QuestGenerator->GenerateQuest();
	AvailableQuests.Add(quest);
	
	return quest;
}

void UQuestManager::AcceptQuest(UQuest* Quest)
{
}

void UQuestManager::CheckQuestCompletion(UQuest* Quest)
{
}
