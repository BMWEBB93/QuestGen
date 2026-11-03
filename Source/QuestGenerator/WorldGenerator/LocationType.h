// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LocationType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ELocationType : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	Ocean UMETA(DisplayName = "Ocean"),
	Beach UMETA(DisplayName = "Beach"),
	Village	UMETA(DisplayName = "Town"),
	Forest UMETA(DisplayName = "Forest"),
	Swamp UMETA(DisplayName = "Swamp"),
	Grassland UMETA(DisplayName = "Grassland"),
	Mountain UMETA(DisplayName = "Mountain")
};
