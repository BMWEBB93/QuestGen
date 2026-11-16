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
	for (FNpcData NPC : Data.NPCs)
	{
		ANpc* SpawnedCharacter =
			GetWorld()->SpawnActorDeferred<ANpc>(
			NPCClasses[NPC.NpcTypeIndex],
			NPC.Transform
		);
		
		ANpc* SpawnedNPC = Cast<ANpc>(SpawnedCharacter);		
		if (SpawnedNPC)
		{
			SpawnedNPC->NPCData = NPC;
			SpawnedNPC->CurrentChunkCoord = Data.WorldCoord;
			SpawnedNPC->WorldGenerator = WorldGenerator;
		}
		
		SpawnedCharacter->SetFolderPath(TEXT("NPC"));

		
		NPC.bIsSpawned = true;
		
		SpawnedCharacter->FinishSpawning(NPC.Transform);
		SpawnedNPCs.Add(SpawnedCharacter);
	}
}

void UChunkSpawnManager::SpawnItems(FChunkData& Data)
{
	FVector ChunkPosition = FVector(Data.WorldCoord.X * ChunkSize, Data.WorldCoord.Y * ChunkSize, 0.f);		
		
	// loop through items in chunk data
	for (FItemData Item : Data.Items)
	{
		if (!Item.bIsSpawned)
		{
			// create transform using chunk position
			FTransform SpawnTransform = FTransform(
				Item.Transform.GetRotation(),
				Item.Transform.GetLocation() + ChunkPosition,
				Item.Transform.GetScale3D()
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
}

void UChunkSpawnManager::DeSpawnNPCs(FChunkData& Data)
{
	for (ANpc* NPC : SpawnedNPCs)
	{
		// skip npcs not on this chunk
		if (NPC->CurrentChunkCoord != Data.WorldCoord)
		{
			continue;
		}
		
		// match the npc in the chunk data
		for (FNpcData NPCData : Data.NPCs)
		{
			if (NPCData.NpcID == NPC->NPCData.NpcID)
			{
				NPCData.bIsSpawned = false;
				NPCData.Transform = NPC->GetActorTransform();
			}
		}
		NPC->Destroy();
		SpawnedNPCs.Remove(NPC);
	}
}

void UChunkSpawnManager::DeSpawnItems(FChunkData& Data)
{
	// loop through all spawned items
	for (int32 i = SpawnedItems.Num() - 1; i >= 0; --i)
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
				ItemData.Transform = Item->GetActorTransform();
			}
		}
		
		Item->Destroy();
		SpawnedItems.RemoveAt(i);
	}
}
