// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "NeighbourDirections.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class  ENeighbourDirections : uint8
{
	N UMETA(DisplayName = "N"),
	E UMETA(DisplayName = "NE"),
	S UMETA(DisplayName = "S"),
	W UMETA(DisplayName = "W"),
	NE UMETA(DisplayName = "NE"),
	NW UMETA(DisplayName = "NW"),
	SE UMETA(DisplayName = "SE"),
	SW UMETA(DisplayName = "SW"),

};
