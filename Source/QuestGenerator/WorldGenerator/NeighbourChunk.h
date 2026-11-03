// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NeighbourDirections.h"
#include "LocationType.h"
#include "NeighbourChunk.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct QUESTGENERATOR_API FNeighbourChunk
{
	GENERATED_BODY()
	
	UPROPERTY()
	ENeighbourDirections Direction;
	
	UPROPERTY()
	ELocationType Location;
	
};
