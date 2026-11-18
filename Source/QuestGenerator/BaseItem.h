// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestGenerator/WorldGenerator/ItemData.h"
#include "QuestGenerator/QuestSystem/QuestManager.h"
#include "QuestGenerator/WorldGenerator/WorldGenerator.h"

#include "QuestGenerator/WorldGenerator/ChunkSpawnManager.h"

#include "BaseItem.generated.h"


UCLASS()
class QUESTGENERATOR_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void Interact();
	
	// Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UStaticMeshComponent> ItemMesh;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntPoint CurrentChunkCoord;
	
	UPROPERTY()
	UChunkSpawnManager* ChunkSpawnManager;
	
	
	UPROPERTY()
	AWorldGenerator* WorldGenerator;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FItemData ItemData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString ItemName;
	
};
