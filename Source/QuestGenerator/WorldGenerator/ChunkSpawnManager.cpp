// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkSpawnManager.h"
#include "Math/IntPoint.h"
#include "QuestGenerator/BaseItem.h"


void UChunkSpawnManager::ClearAll()
{
	// Destroy NPCs
	for (ANpc* NPC : SpawnedNPCs)
	{
		if (IsValid(NPC))
		{
			NPC->Destroy();
		}
	}
	
	// Destroy Items
	for (ABaseItem* Item : SpawnedItems)
	{
		if (IsValid(Item))
		{
			Item->Destroy();
		}
	}
	
	// Clear maps
	SpawnedNPCs.Empty();
	SpawnedItems.Empty();
}

void UChunkSpawnManager::SpawnNPCs(FChunkData& Data)
{	
	FVector ChunkPosition = FVector(Data.WorldCoord.X * ChunkSize, Data.WorldCoord.Y * ChunkSize, 0.f);		

	for (FNpcData& NPC : Data.NPCs)
	{
		if (NPC.bIsSpawned)
		{
			continue;
		}
			

	
		// create transform using chunk position
		FTransform SpawnTransform = FTransform(
			NPC.Transform.GetRotation(),
			NPC.Transform.GetLocation() + ChunkPosition,
			NPC.Transform.GetScale3D()
		);

		ANpc* SpawnedNPC = GetWorld()->SpawnActorDeferred<ANpc>(
			NPCClasses[NPC.NpcTypeIndex],
			SpawnTransform
		);
	
		SpawnedNPC->NPCData = NPC;
		SpawnedNPC->CurrentChunkCoord = Data.WorldCoord;
		SpawnedNPC->WorldGenerator = WorldGenerator;

		SpawnedNPC->SetFolderPath(TEXT("NPC"));

		SpawnedNPC->FinishSpawning(SpawnTransform);

		NPC.bIsSpawned = true;

		SpawnedNPCs.Add(SpawnedNPC);
	}
}

void UChunkSpawnManager::SpawnItems(FChunkData& Data)
{	
	FVector ChunkPosition = FVector(Data.WorldCoord.X * ChunkSize, Data.WorldCoord.Y * ChunkSize, 0.f);		
		
	// loop through items in chunk data
	for (FItemData& Item : Data.Items)
	{
		if (Item.bIsSpawned)
		{
			continue;
		}
			
	
		// create transform using chunk position
		FTransform SpawnTransform = FTransform(
			Item.Transform.GetRotation(),
			Item.Transform.GetLocation() + ChunkPosition,
			FVector(1.f)
		);
		
		// start item spawning
		ABaseItem* ItemToSpawn =
			GetWorld()->SpawnActorDeferred<ABaseItem>(
			ItemClasses[Item.ItemTypeIndex],
			SpawnTransform
		);
		
		ABaseItem* SpawnedItem = Cast<ABaseItem>(ItemToSpawn);		
		if (SpawnedItem)
		{
			SpawnedItem->ItemData = Item;
			SpawnedItem->CurrentChunkCoord = Data.WorldCoord;
		}
		
		Item.bIsSpawned = true;
		
		ItemToSpawn->SetFolderPath(TEXT("Items"));

			
		ItemToSpawn->FinishSpawning(SpawnTransform);
		SpawnedItems.Add(ItemToSpawn);
	}
}

void UChunkSpawnManager::DeSpawnNPCs(FChunkData& Data)
{	
	FVector ChunkPosition = FVector(Data.WorldCoord.X * ChunkSize, Data.WorldCoord.Y * ChunkSize, 0.f);		

	for (int i = SpawnedNPCs.Num() - 1; i >= 0; --i)
	{
		ANpc* NPC = SpawnedNPCs[i];

		// skip npcs not on this chunk
		if (NPC->CurrentChunkCoord != Data.WorldCoord)
		{
			continue;
		}
		
		// match the npc in the chunk data
		for (FNpcData& NPCData : Data.NPCs)
		{
			if (NPCData.NpcID == NPC->NPCData.NpcID)
			{
				NPCData.bIsSpawned = false;
				
				FTransform ActorTransform = FTransform(
					NPC->GetActorTransform().GetRotation(),
					NPC->GetActorLocation() - ChunkPosition,
					NPC->GetActorTransform().GetScale3D()
				);		
				
				NPCData.Transform = ActorTransform;
			}
		}
		NPC->Destroy();
		SpawnedNPCs.RemoveAt(i);
	}	
}

void UChunkSpawnManager::DeSpawnItems(FChunkData& Data)
{
	FVector ChunkPosition = FVector(Data.WorldCoord.X * ChunkSize, Data.WorldCoord.Y * ChunkSize, 0.f);		

	// loop through all spawned items
	for (int i = SpawnedItems.Num() - 1; i >= 0; --i)
	{
		ABaseItem* Item = SpawnedItems[i]; // get the item

		// skip items not relating to this chunk
		if (Item->CurrentChunkCoord != Data.WorldCoord)
		{
			continue;
		}
		
		// match the item in the chunk data
		for (FItemData& ItemData : Data.Items) // for each item in chunk data
		{
			if (ItemData.ItemID == Item->ItemData.ItemID) // if the id matches the spawned item
			{
				ItemData.bIsSpawned = false;
				
				FTransform ActorTransform = FTransform(
					Item->GetActorTransform().GetRotation(),
					Item->GetActorLocation() - ChunkPosition,
					FVector(1.f)
				);
				
				ItemData.Transform = ActorTransform;
			}
		}
		
		Item->Destroy();
		SpawnedItems.RemoveAt(i);
	}
}
