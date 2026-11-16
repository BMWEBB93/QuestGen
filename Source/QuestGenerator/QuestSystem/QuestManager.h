// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Quest.h"
#include "Objective.h"
#include "ObjectiveType.h"

#include "QuestGenerator/WorldGenerator/Town.h"
#include "ProceduralQuestGenerator.h"

#include "QuestManager.generated.h"

/**
 * 
 */

class AWorldGenerator;

UCLASS(Blueprintable)
class QUESTGENERATOR_API UQuestManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	// Quests available to the player in the level
	UPROPERTY()
	TArray<UQuest*> AvailableQuests;
	
	// Quests currently accepted by the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UQuest*> ActiveQuests;
	
	// Quests completed by the player
	UPROPERTY()
	TArray<UQuest*> CompletedQuests;
	
	UPROPERTY()
	TObjectPtr<UProceduralQuestGenerator> QuestGenerator;

	UFUNCTION()
	UQuest* RequestQuest(FTown TownData); 
	
	// Accept a quest
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void AcceptQuest(UQuest* Quest);
	void HandleObjectiveComplete(UQuest* Quest);

	UPROPERTY()
	AWorldGenerator* WorldGenerator = nullptr;
	
	
	
	// Check quest completion for go to location quest types
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void CheckLocationQuestCompletion(FIntPoint LocationToCheck);
	
	// Check quest completion for Gather quest types
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void CheckGatherQuestCompletion(int ItemType);
	
	// Check quest completion for Retieve quest types
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void CheckRetrieveQuestCompletion(int ItemID);
	
	
	// Check quest completion for bounty quest types
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void CheckKillQuestCompletion(int NPCId);
};
