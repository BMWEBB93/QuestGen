// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Town.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct QUESTGENERATOR_API FTown
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint WorldPosition;
	
};
