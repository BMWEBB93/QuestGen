// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));

	RootComponent = ItemMesh;
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
	QuestManager->CheckGatherQuestCompletion(ItemData.ItemTypeIndex);
	QuestManager->CheckRetrieveQuestCompletion(ItemData.ItemID);

}

