// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldState.generated.h"

/**
 * 
 */
UCLASS()
class QUESTGENERATOR_API UWorldState : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
	
	virtual void Initialize(
	  FSubsystemCollectionBase& Collection
  ) override;

	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;
};

