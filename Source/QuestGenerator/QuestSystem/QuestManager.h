// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"


#include "QuestManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class QUESTGENERATOR_API UQuestManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	
	virtual void Initialize(
	  FSubsystemCollectionBase& Collection
  ) override;

	virtual void Deinitialize() override;

	// Quests currently accepted by the player
	UPROPERTY()
	TArray<TObjectPtr<UQuest>> ActiveQuests;

	// Accept a quest
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void AcceptQuest(UQuest* Quest);

	

	// Check quest completion
	void CheckQuestCompletion(UQuest* Quest);
};
