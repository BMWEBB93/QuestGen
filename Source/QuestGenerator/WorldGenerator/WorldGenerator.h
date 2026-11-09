// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ChunkVisualiser.h"
#include "OceanChunk.h"
#include "ChunkData.h"
#include "River.h"
#include "Town.h"
#include "TownGenerator.h"

#include "WorldGenerator.generated.h"

UCLASS()
class QUESTGENERATOR_API AWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SetNeighbourLocation(
		FChunkData* ThisChunk,
		FChunkData* NeighbourChunk,
		ENeighbourDirections Direction);
		
	
	void UpdateChunkNeighbours();
	void DetermineMountains();
	void DetermineBiomes();
	void CreateRiverSources();
	void CreateRiverPaths();
	void CarvePointInRiver(FChunkData* Chunk, int x, int y);
	void CreateLake(int HalfChunkLength, FRiver& River);
	void CarveRivers();
	bool PositionOnRiver(FIntPoint Coord);
	
	

	
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generation")
	void GenerateWorld();	
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Generation")
	void ClearWorld();
	
	UFUNCTION(BlueprintImplementableEvent)
	void GenerationFinished();
	
	float AddHillsToMountainBorder(FChunkData& chunkData, int y, int x, float Noise2, float Noise1);

	void GenerateMapData(FChunkData& chunkData);
	
	void GenerateFoliageData(FChunkData& chunkData);

	
	int GetLOD(FIntPoint Coord);
	FIntPoint GetLODTransitionBoundary(int LOD, FIntPoint Coord);

	// Handle shifting chunks depending on player "shifts to make"
	void HandleChunkShifting();
	
	// shift land chuks when player moves to a new chunk
	void ShiftLandChunks(FIntPoint Direction);

	// shift ocean chuks when player moves to a new chunk
	void ShiftOceanChunks(FIntPoint Direction);
	
	
	UFUNCTION(BlueprintCallable)
	float GetHeightAtLocation(float WorldX, float WorldY);
	
	UFUNCTION(BlueprintCallable)
	ELocationType GetChunkBiomeAtLocation(float WorldX, float WorldY);

	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int Seed = 12345;
	
	
	
	UPROPERTY()
	TMap<FIntPoint, FChunkData> ChunkData;
	
	UPROPERTY()
	TMap<FIntPoint, AOceanChunk*> OceanChunks;
	
	UPROPERTY()
	TMap<FIntPoint, AChunkVisualiser*> ChunkVisualisers;
	
	// build queues for land chunks
	UPROPERTY()
	TArray<AChunkVisualiser*> PendingChunkRebuilds;
	
	// build queues for ocean chunks
	UPROPERTY()
	TArray<AOceanChunk*> PendingOceanRebuilds;
	
	
	// player reference used to move chunks
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACharacter* Player;

	UPROPERTY()
	FIntPoint LastPlayerCoord = FIntPoint(0, 0);

	UPROPERTY()
	FIntPoint PlayerChunkCoord = FIntPoint(0, 0);
	

	
	/////////////////////////
	/// World Settings
	////////////////////////
	
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	int WorldSize = 16; // value is in chunks

	// how many chunks can rebuild per tick
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	int BuildsPerTick = 2;
	
	// time allowed for chunk rebuilds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	float TimePerTick = 0.0333f;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	float CoastFrequency = 0.5f;
	
	UPROPERTY()
	float SeedOffsetX;
	
	UPROPERTY()
	float SeedOffsetY;
	
	// height before noise is applied
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	float ContinentBaseHeight = 1000.f;
	
	// Ocean Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Ocean")
	float OceanDepth = 1000.f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Ocean")
	float OceanFloorNoiseFrequency = 0.0002f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Ocean")
	float OceanFloorNoiseScale = 500.f;
	
	// Mountain Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Mountains")
	float MountainHeight = 1500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Mountains",
		  meta = (ClampMin = "0", ClampMax = "4"))
	float RegionRate = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Mountains")
	float MountainFrequency = 0.00006f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Mountains")
	float MountainPower = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Mountains")
	float RidgeScale = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Mountains")
	float RidgeNoiseFrequency = 0.001f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Mountains")
	float RidgePowerRate = 1.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings - Mountains")
	float MountainHillScale = 3.f;
	
	
	
	// Noise 1
	// frequency of sampled noise for heightmap
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	float Noise1Frequency = 0.00005f;

	// ratio of height this noise affects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	float Noise1Scale = 2000.f;

	// Noise 2
	// frequency of sampled noise for heightmap
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	float Noise2Frequency = 0.0002f;

	// ratio of height this noise affects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	float Noise2Scale = 400.f;

	// Noise 3
	// frequency of sampled noise for heightmap
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	float Noise3Frequency = 0.005f;

	// ratio of height this noise affects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Settings")
	float Noise3Scale = 50.f;
		
	
	/////////////////////////
	/// Chunk settings
	/////////////////////////
	
	UPROPERTY()
	float ChunkSize;
	
	UPROPERTY()
	FIntPoint ShiftsToMake = FIntPoint(0,0 );
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk Settings")
	UMaterialInterface* TerrainMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk Settings")
	UMaterialInterface* OceanMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk Settings")
	float ChunkScale = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk Settings")
	int ChunkResolution = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk Settings")
	float ChunkUVScale = 200;
	
	
	////////////////////////////
	/// Lod settings
	////////////////////////////
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int LOD1RenderDistance = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int LOD2RenderDistance = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int LOD3RenderDistance = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int LOD4RenderDistance = 10;
	
	
	////////////////////////////
	/// Foliage/Buildings settings
	////////////////////////////
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings")
	UStaticMesh* TreeMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings")
	UStaticMesh* GrassMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings")
	UStaticMesh* PlantMesh;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings")
	float HumidityFrequency = 0.02f;
	
	
	////////////////////////////
	/// Town settings
	////////////////////////////
	
	UFUNCTION()
	void SpawnTown(FChunkData& Data);
	
	UFUNCTION()
	void DespawnTown(ATownGenerator* Town, FChunkData& Data);
	
	void FindTownLocations();	
	
	UFUNCTION()
	void SetTownData();
	
	
	// array of town data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Towns")
	TArray<FTown> Towns;
	
	// array of town generator
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Towns")
	TArray<ATownGenerator*> TownGenerators;
	
	// town generator class to be spawned on town positions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	TSubclassOf<ATownGenerator> TownGeneratorClass;
	
	// array of buildings to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	TArray<TSubclassOf<AActor>> BuildingClasses;
	
	// Class of town centre to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	TSubclassOf<AActor> TownCentreClass;
	
	// Class of job billboard to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	TSubclassOf<AActor> JobBoardClass;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Towns")
	int TownsCount = 3;
	
	
	UPROPERTY(EditAnywhere, Category = "Towns")
	float TownPlatformHeight = 500.f;	
	
	////////////////////////////
	/// River settings
	////////////////////////////
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	TArray<FRiver> Rivers;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	float RiverDepth = -100.f;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	int RiverWidth = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	int FloodPlainWidth = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	float LakeRadius = 12.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	float LakeDepth = 60.f;
	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	float RiverFrequency = 0.005f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	float RiverNoiseScale = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	int RiverCount = 1;
	
};
