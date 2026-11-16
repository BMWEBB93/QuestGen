// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ObjectiveType.h"
#include "ObjectiveState.h"
#include "CoreMinimal.h"
#include "QuestGenerator/BaseCharacter.h"
#include "QuestGenerator/BaseItem.h"
#include "Objective.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class QUESTGENERATOR_API UObjective : public UObject
{
    GENERATED_BODY()   
    
    public: 
    
    bool IsComplete();
    
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int ObjectiveId;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText ObjectiveDescription;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EObjectiveType ObjectiveType;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EObjectiveState ObjectiveState;
    
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int TargetCharacter;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int TargetItem;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int TargetItemType;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int TargetCount = 1;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int CurrentCount = 0;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FIntPoint TargetLocation;
};
