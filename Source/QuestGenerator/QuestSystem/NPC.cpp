// Fill out your copyright notice in the Description page of Project Settings.


#include "Npc.h"

#include "Kismet/GameplayStatics.h"
#include "QuestGenerator/WorldGenerator/WorldGenerator.h"


ANpc::ANpc()
{
}

void ANpc::BeginPlay()
{
	Super::BeginPlay();
	
	// if world isn't set, get it from the world
	if (!WorldGenerator)
	{
		WorldGenerator = Cast<AWorldGenerator>(
			UGameplayStatics::GetActorOfClass(
				GetWorld(),
				AWorldGenerator::StaticClass()
			)
		);

	}
}

void ANpc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// get the actor location 
	FVector Location = GetActorLocation();
	// use location to get the chunk the actor is on
	CurrentChunkCoord = WorldGenerator->GetChunkCoordAtLocation(Location.X, Location.Y);
}

void ANpc::Death()
{
	Super::Death();
	
	// get the game instance from the world
	UGameInstance* GameInstance = GetGameInstance();

	// get the quest manager from the game instance
	UQuestManager* QuestManager = GameInstance->GetSubsystem<UQuestManager>();
	
	// check item with quest objectives 
	QuestManager->CheckKillQuestCompletion(NPCData.NpcID);

}
