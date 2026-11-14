// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LocationType.h"

#include "NameGenerator.generated.h"

/**
 * 
 */
UCLASS()
class QUESTGENERATOR_API UNameGenerator : public UObject
{
	GENERATED_BODY()

public:

	UNameGenerator();
	
	FText GenerateName(ELocationType Biome);

private:

	TArray<FString> Prefixes;

    TMap<ELocationType, TArray<FString>> Suffixes;
};
