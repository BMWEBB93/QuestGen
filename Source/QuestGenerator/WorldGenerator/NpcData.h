// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "NpcData.generated.h"

/**
 * Data for NPCs that are held in chunks
 */
USTRUCT(BlueprintType)
struct QUESTGENERATOR_API FNpcData
{
	GENERATED_BODY()

	// name of the npc
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Name;
	
	// index representing the actor class the character should spawn as 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int NpcTypeIndex;
	
	// index representing the specific npc
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int NpcID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform Transform;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsSpawned = false;
	
	
	
};
