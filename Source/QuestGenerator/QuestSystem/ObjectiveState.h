// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EObjectiveState : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	Available UMETA(DisplayName = "Available"),
	Locked UMETA(DisplayName = "Locked"),
	InProgress UMETA(DisplayName = "In Progress"),
	Completed	UMETA(DisplayName = "Completed"),
	Failed UMETA(DisplayName = "Failed")	

};
