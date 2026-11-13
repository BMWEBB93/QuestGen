// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Objective.h"
#include  "QuestType.h"

#include "ObjectiveState.h"
#include "Quest.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class QUESTGENERATOR_API UQuest : public UObject
{
	
	GENERATED_BODY()   

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Quest")
	int QuestId = 0;
	
	UPROPERTY()
	int TownId = 0;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString QuestName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString QuestDescription;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EQuestType QuestType = EQuestType::NONE;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EObjectiveState QuestState = EObjectiveState::Available;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int QuestGiverId = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UObjective*> Objectives;

};
