// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "QuestGenerator/WorldGenerator/Town.h"
#include "Quest.h"
#include "JobBoard.generated.h"

UCLASS()
class QUESTGENERATOR_API AJobBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJobBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateJob(int JobIndex);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(BlueprintReadWrite)
	FTown TownData;
};
