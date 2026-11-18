// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create default scene root
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	RootComponent = SceneRoot;

	// Create item mesh
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	
	// Attach mesh to root
	ItemMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseItem::Interact()
{
	// get the game instance from the world
	UGameInstance* GameInstance = GetGameInstance();

	// get the quest manager from the game instance
	UQuestManager* QuestManager = GameInstance->GetSubsystem<UQuestManager>();
	
	// check item with quest objectives 
	bool bIsGatherQuest = QuestManager->CheckGatherQuestCompletion(ItemData.ItemTypeIndex);
	bool bIsItemQuest = QuestManager->CheckRetrieveQuestCompletion(ItemData.ItemID);
	
	if (bIsItemQuest || bIsGatherQuest)
	{
		ChunkSpawnManager->RemoveItem(ItemData.ItemID);
		WorldGenerator->RemoveItem(CurrentChunkCoord, ItemData.ItemID);
	
		Destroy();
	}

}

