// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ObjectiveType.h"
#include "ObjectiveState.h"
#include "CoreMinimal.h"


#include "Objective.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class QUESTGENERATOR_API UObjective : public UObject
{
    GENERATED_BODY()   
    
    public: 
    
    virtual bool IsComplete();
    
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int ObjectiveId;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString ObjectiveDescription;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    ObjectiveType ObjectiveType;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EObjectiveState ObjectiveState;
};
