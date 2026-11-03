// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "River.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct QUESTGENERATOR_API FRiver
{
	GENERATED_BODY()

	UPROPERTY()
	int SpawnScore = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Source = FIntPoint(0, 0); 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntPoint> RiverPath;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntPoint> RiverDirection;
	
	UPROPERTY()
	bool bReachedOcean = false;
	
	UPROPERTY()
	bool bOnlyLake = false;
};
