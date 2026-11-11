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
	TArray<EQuestType> QuestTypes = 
	{
		EQuestType::Delivery,
		EQuestType::Gather,
		EQuestType::Bounty,
		EQuestType::Retrieval
	};	

	int32 RandomQuestType = FMath::RandRange(0, 3);	
	Quest->QuestType = QuestTypes[RandomQuestType];
	
	// set the town ID
	Quest->TownId = TownData.TownId;
	
	
	
	
	return Quest;
}
