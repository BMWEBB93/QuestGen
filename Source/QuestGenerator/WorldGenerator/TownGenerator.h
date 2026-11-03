// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Town.h"
#include "TownGenerator.generated.h"

UCLASS()
class QUESTGENERATOR_API ATownGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATownGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTown TownData;
};
