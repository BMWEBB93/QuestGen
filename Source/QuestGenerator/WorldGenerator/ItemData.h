// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ItemData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct QUESTGENERATOR_API FItemData
{
	
	GENERATED_BODY()

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ItemName;
	
	// index representing the actor class the item should spawn as 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ItemTypeIndex;
	
	// index representing the specific item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ItemID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform Transform;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsSpawned = false;

};
