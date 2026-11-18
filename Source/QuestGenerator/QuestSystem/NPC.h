// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "QuestGenerator/BaseCharacter.h"
#include "QuestGenerator/WorldGenerator/NpcData.h"

#include "Npc.generated.h"

/**
 * 
 */
class AWorldGenerator;
class UChunkSpawnManager;

UCLASS(Blueprintable)
class QUESTGENERATOR_API ANpc : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	
	ANpc();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void Death() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	FNpcData NPCData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FString Name;
	
	UPROPERTY()
	AWorldGenerator* WorldGenerator;	
	
	UPROPERTY()
	UChunkSpawnManager* ChunkSpawnManager;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	FIntPoint CurrentChunkCoord;
	
	
};
