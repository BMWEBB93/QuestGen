// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkData.h"
#include "QuestGenerator/QuestSystem/Npc.h"

#include "NpcData.h"
#include "ItemData.h"

#include "ChunkSpawnManager.generated.h"



/**
 * 
 */
class ABaseItem;
class AWorldGenerator;

UCLASS()
class QUESTGENERATOR_API UChunkSpawnManager : public UObject
{
	
	GENERATED_BODY()
	
	
public:
	
	
	UFUNCTION()
	void ClearAll();
	
	UFUNCTION()
	void SpawnNPCs(FChunkData& Data);
	
	UFUNCTION()
	void SpawnItems(FChunkData& Data);
	
	UFUNCTION()
	void DeSpawnNPCs(FChunkData& Data);
	
	UFUNCTION()
	void DeSpawnItems(FChunkData& Data);
	
	// reference the world generator
	UPROPERTY()
	AWorldGenerator* WorldGenerator = nullptr;
	
	
	// items currently spawned 
	UPROPERTY()
	TArray<ABaseItem*> SpawnedItems;
	
	// items currently spawned
	UPROPERTY()
	TArray<ANpc*> SpawnedNPCs;
	
	// item classes that can be spawned by the world
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings")
	TArray<TSubclassOf<ABaseItem>> ItemClasses;
	
	// NPC classes that can be spawned by the world
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Settings")
	TArray<TSubclassOf<ANpc>> NPCClasses;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ChunkResolution;
	
	UPROPERTY()
	float ChunkScale = 0.f;
	
	UPROPERTY()
	float ChunkSize = 0.f;
	
};
