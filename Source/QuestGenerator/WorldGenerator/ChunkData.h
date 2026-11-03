// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include  "LocationType.h"
#include "NeighbourDirections.h"
#include "Town.h"
#include "ChunkData.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct QUESTGENERATOR_API FChunkData
{
	GENERATED_BODY()
	
	UPROPERTY()
	ELocationType Location = ELocationType::NONE;
	
	
	
	
	UPROPERTY()
	TArray<float> Heights; // Heightmap 
	
	UPROPERTY()
	FIntPoint WorldCoord = FIntPoint(0, 0);
	
	// foliage
	UPROPERTY()
	TArray<FTransform> TreeTransforms;
	
	UPROPERTY()
	TArray<FTransform> GrassTransforms;
	
	UPROPERTY()
	TArray<FTransform> PlantTransforms;
	
	
	UPROPERTY()
	FTown Town;
	
	UPROPERTY()
	bool bHasTown = false;
	
	// Neighbouring locations
	
	UPROPERTY()
	TMap<ENeighbourDirections, ELocationType> NeighbourChunks;
	
	FChunkData()
	{
		NeighbourChunks.Add(ENeighbourDirections::N, ELocationType::NONE);
		NeighbourChunks.Add(ENeighbourDirections::NE, ELocationType::NONE);
		NeighbourChunks.Add(ENeighbourDirections::E, ELocationType::NONE);
		NeighbourChunks.Add(ENeighbourDirections::SE, ELocationType::NONE);
		NeighbourChunks.Add(ENeighbourDirections::S, ELocationType::NONE);
		NeighbourChunks.Add(ENeighbourDirections::SW, ELocationType::NONE);
		NeighbourChunks.Add(ENeighbourDirections::W, ELocationType::NONE);
		NeighbourChunks.Add(ENeighbourDirections::NW, ELocationType::NONE);
	}
	
	
	UPROPERTY()
	ELocationType NorthLocation = ELocationType::NONE;
	
	UPROPERTY()
	ELocationType EastLocation = ELocationType::NONE;
	
	UPROPERTY()
	ELocationType SouthLocation = ELocationType::NONE;
	
	UPROPERTY()
	ELocationType WestLocation = ELocationType::NONE;
	
	UPROPERTY()
	ELocationType NorthEastLocation = ELocationType::NONE;
	
	UPROPERTY()
	ELocationType SouthEastLocation = ELocationType::NONE;
	
	UPROPERTY()
	ELocationType NorthWestLocation = ELocationType::NONE;
	
	UPROPERTY()
	ELocationType SouthWestLocation = ELocationType::NONE;
	
};
