// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestType.generated.h"


/**
 * 
 */
UENUM(BlueprintType)
enum class EQuestType : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	Delivery UMETA(DisplayName = "Delivery"),
	Gather UMETA(DisplayName = "Gather"),
	Bounty	UMETA(DisplayName = "Bounty"),
	Retrieval UMETA(DisplayName = "Retrieval")	
};

