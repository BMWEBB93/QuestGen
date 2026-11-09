// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GrassChunk.h"
#include "WorldGenerator.h"


#include "GrassGenerator.generated.h"

UCLASS()
class QUESTGENERATOR_API AGrassGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrassGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION(BlueprintCallable)
	void CreateGrass();
	
	// shift grass when player moves 
	void ShiftGrass(FIntPoint Direction);

	

	// used to update chunk position
	UPROPERTY()
	FVector PlayerDelta = FVector(0, 0, 0);

	UPROPERTY()
	TArray<AGrassChunk*> GrassChunks;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ACharacter* Player;
	
	UPROPERTY()
	FVector LastPlayerLocation = FVector(0, 0, 0);

	bool bIsGrassGenerated = false;

	// reference to world generator, used to get height
	UPROPERTY()
	AWorldGenerator* WorldGenerator;
	
	UPROPERTY()
	float TownPlatformHeight;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 CullDistance = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int TileRange = 3; // in chunks away from the player

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float TileSize = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int Density = 8;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float GrassMaxHeight = 1500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float GrassMaxSlopeAngle = 10.f;


	/////////////////
	///// Grass
	/////////////////

	//meshes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grass Settings")
	UStaticMesh* GrassMesh;
};
