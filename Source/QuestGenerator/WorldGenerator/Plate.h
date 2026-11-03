// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Plate.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct QUESTGENERATOR_API FPlate
{
	GENERATED_BODY()

	UPROPERTY()
	FIntPoint PlateLocation;
	
	UPROPERTY()
	FIntPoint MovementDirection;
};
