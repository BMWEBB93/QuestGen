// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

#include "ChunkData.h"


#include "ChunkVisualiser.generated.h"

UCLASS()
class QUESTGENERATOR_API AChunkVisualiser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkVisualiser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GenerateChunk(
	const FChunkData* Data,    
	UMaterialInterface* TerrainMaterial,
	FIntPoint LODBorderDir,
	float BeachValue
	);
	
	
	UFUNCTION()
	void SetHISMs();
	
	UFUNCTION()
	void CreateFoliage(const FChunkData& Data);

	
	UFUNCTION()
	void ClearFoliage();
	
	UPROPERTY()
	UProceduralMeshComponent* Mesh;
	
	// chunk settings		
	UPROPERTY()
	float Scale = 100;

	UPROPERTY()
	int Resolution = 100;

	UPROPERTY()
	int ChunkUVScale = 100;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Parameters")
	int LOD = 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Parameters")
	ELocationType Location = ELocationType::NONE;
	
	
	UPROPERTY()
	float SeedOffsetX;
	
	UPROPERTY()
	float SeedOffsetY;
	
	
	
	
	// trees
	UPROPERTY()
	UStaticMesh* TreeMesh;
	
	UPROPERTY()
	UStaticMesh* GrassMesh;
	
	UPROPERTY()
	UStaticMesh* PlantMesh;
	
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* TreeHISM;
	
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* GrassHISM;
	
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PlantHISM;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Parameters")
	FIntPoint WorldCoord = FIntPoint(0 , 0);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Parameters")
	FIntPoint ChunkCoord = FIntPoint(0, 0);
	
	UPROPERTY()
	TArray<float> Heights;
	
	
	UPROPERTY()
	FIntPoint LODBoundary = FIntPoint(0, 0);
	
	
	// ocean settings
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Edge Settings")
	bool bIsInGameWorld = true;
	
	UPROPERTY()
	float OceanDepth = 1000.f;
	
	UPROPERTY()
	float OceanFloorNoiseFrequency = 0.0002f;
	
	UPROPERTY()
	float OceanFloorNoiseScale = 500.f;

};
