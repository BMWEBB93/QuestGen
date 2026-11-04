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


}

void ATownGenerator::CreateTown()
{
	if (!bTownSpawned)
	{
		for (int i = 0; i < TownData.BuildingCount; i++)
		{
			int* BuildingIndex = TownData.Buildings.Find(i);
			FVector* BuildingLocation = TownData.BuildingsLocations.Find(i);
			
			FVector SpawnLocation(
				BuildingLocation->X + TownData.WorldPosition.X * ChunkSize + ChunkSize / 2,
				BuildingLocation->Y + TownData.WorldPosition.Y * ChunkSize + ChunkSize / 2,
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
			
			Buildings.Add(SpawnedBuilding);
		}

		bTownSpawned = true;
	}
}

void ATownGenerator::ClearTown()
{
	for (AActor* Building : Buildings)
	{
		if (Building)
		{
			Building->Destroy();
		}
		
	}
	Buildings.Empty();
}



