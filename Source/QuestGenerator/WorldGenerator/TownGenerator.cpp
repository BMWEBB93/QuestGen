// Fill out your copyright notice in the Description page of Project Settings.


#include "TownGenerator.h"

#include "QuestGenerator/QuestSystem/JobBoard.h"


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
	
	// get the quest manager
	UGameInstance* GameInstance = GetGameInstance();

	if (!IsValid(GameInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is NULL in CreateTown"));
		return;
	}

	QuestManager = GameInstance->GetSubsystem<UQuestManager>();

	if (!IsValid(QuestManager))
	{
		UE_LOG(LogTemp, Error, TEXT("QuestManager is NULL in CreateTown"));
		return;
	}
	
	FillQuests();

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
	
		
		
		// spawn the different building types
		for (int i = 0; i < TownData.BuildingCount; i++)
		{
			int* BuildingIndex = TownData.Buildings.Find(i);
			FVector* BuildingLocation = TownData.BuildingsLocations.Find(i);
			
			FVector SpawnLocation(
				BuildingLocation->X + TownData.WorldPosition.X * ChunkSize + ChunkSize / 2,
				BuildingLocation->Y + TownData.WorldPosition.Y * ChunkSize + ChunkSize / 2,
				TownHeight - 10.f
			);
			
			FRotator SpawnRotation(0.f , BuildingLocation->Z, 0.f);

			UClass* BuildingClass = BuildingClasses[*BuildingIndex];

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AActor* SpawnedBuilding = GetWorld()->SpawnActor<AActor>(
				BuildingClass,
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);		
			Buildings.Add(SpawnedBuilding);
		}

		// spawn the town centre
		
		// get the spawn location
		FVector SpawnLocation(
				TownData.WorldPosition.X * ChunkSize + ChunkSize / 2,
				TownData.WorldPosition.Y * ChunkSize + ChunkSize / 2,
				TownHeight
			);
		
		// set spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// spawn town centre
		TownCentre = GetWorld()->SpawnActor<AActor>(
			TownCentreClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);		
		
		
		// spawn the Job board
		
		// offset spawn location
		SpawnLocation += FVector(
				-800.f,
				100.f,
				0.f
			);
		
		
		// spawn town centre
		JobBoard = GetWorld()->SpawnActor<AActor>(
			JobBoardClass,
			SpawnLocation,
			FRotator(0.f , 270.f , 0.f),
			SpawnParams
		);		
		
		AJobBoard* JobBoardActor = Cast<AJobBoard>(JobBoard);
		JobBoardActor->TownData = TownData;
		
		
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
	
	if (IsValid(JobBoard))JobBoard->Destroy();
	if (IsValid(TownCentre))TownCentre->Destroy();
	
}

void ATownGenerator::FillQuests()
{
	
	int MaxQuests = TownData.MaxAvailableQuests;
	Quests.SetNum(MaxQuests);
	TownData.Quests.SetNum(MaxQuests);
	
	for (int i = 0; i < MaxQuests; i++)
	{
		if (IsValid(TownData.Quests[i]))
		{
			Quests[i] = TownData.Quests[i];
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("QuestManager: %s"),
	IsValid(QuestManager) ? TEXT("Valid") : TEXT("NULL"));
			
			Quests[i] = QuestManager->RequestQuest(TownData);
			TownData.Quests[i] = Quests[i];
		}		
	}
}



