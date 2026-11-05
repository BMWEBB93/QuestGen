// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM( BlueprintType )
enum class ObjectiveType : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	Kill UMETA(DisplayName = "Kill"),
	Interact UMETA(DisplayName = "Interact"),
	Location	UMETA(DisplayName = "Location"),
};

