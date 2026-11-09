// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Quest.h"
#include "QuestGenerator/WorldGenerator/Town.h"
#include "ProceduralQuestGenerator.h"

#include "QuestManager.generated.h"

/**
 * 
 */
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
	UPROPERTY()
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

		

	// Check quest completion
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void CheckQuestCompletion(UQuest* Quest);
};
