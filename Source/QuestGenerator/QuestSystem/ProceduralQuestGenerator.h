// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "QuestGenerator/WorldGenerator/WorldGenerator.h"
#include  "QuestType.h"
#include  "Quest.h"
#include "Objective.h"
#include "ProceduralQuestGenerator.generated.h"

/**
 * 
 */
class UQuestManager;
class AWorldGenerator;

UCLASS()
class QUESTGENERATOR_API UProceduralQuestGenerator : public UObject
{
	GENERATED_BODY()
	
	public:
	
	UFUNCTION()
	void Initialise(UQuestManager* Manager);

	UPROPERTY()
	TObjectPtr<UQuestManager> QuestManager;
	
	UFUNCTION()
	UQuest* GenerateQuest(FTown TownData);
	
	UPROPERTY()
	AWorldGenerator* WorldGenerator = nullptr;
	
	UPROPERTY()
	int NextQuestID = 0;
};
