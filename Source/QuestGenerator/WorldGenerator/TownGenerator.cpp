// Fill out your copyright notice in the Description page of Project Settings.


#include "TownGenerator.h"


// Sets default values
ATownGenerator::ATownGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATownGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATownGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bTownSpawned && bIsDataValid)
	{
		for (int i = 0; i < TownData.BuildingCount; i++)
		{
			int* BuildingIndex = TownData.Buildings.Find(i);
			FVector* BuildingLocation = TownData.BuildingsLocations.Find(i);

			// Check the map entries exist
			if (!BuildingIndex || !BuildingLocation)
			{
				UE_LOG(LogTemp, Warning, TEXT("Missing building data for slot %d"), i);
				continue;
			}

			// Check the building class exists
			if (!BuildingClasses.IsValidIndex(*BuildingIndex) ||
				!BuildingClasses[*BuildingIndex])
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid building class for slot %d"), i);
				continue;
			}

			FVector SpawnLocation(
				BuildingLocation->X,
				BuildingLocation->Y,
				TownHeight
			);

			UClass* BuildingClass = BuildingClasses[*BuildingIndex];

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AActor* SpawnedBuilding = GetWorld()->SpawnActor<AActor>(
				BuildingClass,
				SpawnLocation,
				FRotator::ZeroRotator,
				SpawnParams
			);
		}

		bTownSpawned = true;
	}
}

