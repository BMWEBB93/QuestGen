// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldState.h"

void UWorldState::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	
}

void UWorldState::Deinitialize()
{
	Super::Deinitialize();
}

void UWorldState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TStatId UWorldState::GetStatId() const
{
	return Super::GetStatId();
}
