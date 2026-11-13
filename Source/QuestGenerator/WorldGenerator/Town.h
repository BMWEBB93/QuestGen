// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestGenerator/QuestSystem/Quest.h"
#include "Town.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct QUESTGENERATOR_API FTown
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint WorldPosition = FIntPoint(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TownId = 0;
	
	UPROPERTY()
	int BuildingCount = 5;
	
	UPROPERTY()
	TMap<int, int> Buildings; // building slot, building index
	
	UPROPERTY()
	TMap<int, FVector> BuildingsLocations; // building slot, building location
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxAvailableQuests = 5;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UQuest>> Quests;
};
