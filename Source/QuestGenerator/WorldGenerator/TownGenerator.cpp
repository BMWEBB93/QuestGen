// Fill out your copyright notice in the Description page of Project Settings.


#include "TownGenerator.h"

#include "Town.h"

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

