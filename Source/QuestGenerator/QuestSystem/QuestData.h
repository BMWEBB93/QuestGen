// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestType.h"

#include "QuestData.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct QUESTGENERATOR_API FQuestData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int QuestId;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString QuestName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString QuestDescription;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EQuestType QuestType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int QuestGiverId;
};
