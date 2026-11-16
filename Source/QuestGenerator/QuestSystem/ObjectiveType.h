// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM( BlueprintType )
enum class EObjectiveType : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	Kill UMETA(DisplayName = "Kill"),
	Interact UMETA(DisplayName = "Interact"),
	Gather UMETA(DisplayName = "Gather"),
	GetItem UMETA(DisplayName = "GetItem"),
	Location	UMETA(DisplayName = "Location"),
};

