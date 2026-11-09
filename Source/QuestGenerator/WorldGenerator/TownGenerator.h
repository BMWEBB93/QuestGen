// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Town.h"
#include "QuestGenerator/QuestSystem/QuestManager.h"
#include "QuestGenerator/QuestSystem/Quest.h"
#include "ChunkData.h"
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
	
	
	UFUNCTION()
	void CreateTown();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generation")
	void ClearTown();
	
	UFUNCTION()
	void FillQuests();

	
	
	UPROPERTY()
	UQuestManager* QuestManager;
	
	UPROPERTY()
	FChunkData ChunkData;
	
	UPROPERTY()
	FTown TownData;
	
	UPROPERTY()
	float ChunkSize;
	
	UPROPERTY()
	float TownHeight;
	
	// array of buildings to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Town")
	TArray<TSubclassOf<AActor>> BuildingClasses;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Buildings;
	
	// Class of town centre to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	TSubclassOf<AActor> TownCentreClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	AActor* TownCentre;
	
	// Class of job billboard to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	TSubclassOf<AActor> JobBoardClass;
	
	// Class of job billboard to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	AActor* JobBoard;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	int MaxAvailableQuests = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	TArray<UQuest*> Quests;
	
	bool bTownSpawned = false;
};
