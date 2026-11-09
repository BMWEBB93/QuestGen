// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include  "QuestType.h"
#include  "Quest.h"
#include "ProceduralQuestGenerator.generated.h"

/**
 * 
 */
class UQuestManager;

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
	UQuest* GenerateQuest();
	
	
	
	UPROPERTY()
	int NextQuestID = 0;
};
