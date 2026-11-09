// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "WorldGenerator.h"

#include "GrassChunk.generated.h"

UCLASS()
class QUESTGENERATOR_API AGrassChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrassChunk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetHISMs(int32 cullDistance);

	UFUNCTION()
	void ClearGrass();

	UFUNCTION()
	void GenerateGrass();

	
	
	UPROPERTY()
	USceneComponent* Root;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	AWorldGenerator* WorldGenerator;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float CullDistance = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FIntPoint Coord = FIntPoint(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ChunkSize = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int Density = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grass Settings")
	float GrassMaxSlopeAngle = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grass Settings")
	float GrassMaxHeight = 3000.f;
	
	UPROPERTY()
	float TownPlatformHeight;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grass Settings")
	UStaticMesh* GrassMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grass Settings")
	UHierarchicalInstancedStaticMeshComponent* GrassHISM;
};
