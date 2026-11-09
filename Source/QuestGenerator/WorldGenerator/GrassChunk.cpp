// Fill out your copyright notice in the Description page of Project Settings.


#include "GrassChunk.h"
#include "LocationType.h"
#include "WorldGenerator.h"


// Sets default values
AGrassChunk::AGrassChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GrassHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("GrassHISM"));
	GrassHISM->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AGrassChunk::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrassChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrassChunk::SetHISMs(int32 cullDistance)
{
	if (GrassMesh)
	{
		GrassHISM->SetStaticMesh(GrassMesh);
		GrassHISM->SetCastShadow(false);
		GrassHISM->SetCullDistances(0, CullDistance);
	}
}

void AGrassChunk::ClearGrass()
{
	GrassHISM->ClearInstances();
}

void AGrassChunk::GenerateGrass()
{
	FVector ChunkLocation = GetActorLocation();
	float CellSize = ChunkSize / Density;
	
	int32 ChunkSeed = HashCombine(
	GetTypeHash(Coord.X),
	GetTypeHash(Coord.Y));
	FRandomStream Stream(ChunkSeed);
	
	// Get Biome at location
	ELocationType Biome = WorldGenerator->GetChunkBiomeAtLocation(ChunkLocation.X, ChunkLocation.Y);
	
	for (int y = 0; y <= Density; y++)
	{
		for (int x = 0; x <= Density; x++)
		{
			FTransform Transform;

			// randomly move cell 
			float RandomDeviationX = Stream.FRandRange(-CellSize/3, CellSize/3);
			float RandomDeviationY = Stream.FRandRange(-CellSize/3, CellSize/3);

			// get location
			FVector WorldLocation = FVector(ChunkLocation.X + (x * CellSize) + RandomDeviationX, ChunkLocation.Y + (y * CellSize) + RandomDeviationY, ChunkLocation.Z);
			float Height = WorldGenerator->GetHeightAtLocation(WorldLocation.X, WorldLocation.Y);			
			
			FVector Location = FVector(x * CellSize + RandomDeviationX, y * CellSize + RandomDeviationY, Height);
			Transform.SetLocation(Location);			

			// set a random rotation
			Transform.SetRotation(FRotator(
				Stream.FRandRange(-5.f, 5.f),
				Stream.FRandRange(0.f, 360.f), 
				Stream.FRandRange(-5.f, 5.f)).Quaternion());
							
		
			
			
			// get slope angle by comparing neighbouring heights
			float Offset = 200.f;
			float HL = WorldGenerator->GetHeightAtLocation(WorldLocation.X - Offset, WorldLocation.Y);
			float HR = WorldGenerator->GetHeightAtLocation(WorldLocation.X + Offset, WorldLocation.Y);

			float HD = WorldGenerator->GetHeightAtLocation(WorldLocation.X, WorldLocation.Y - Offset);
			float HU = WorldGenerator->GetHeightAtLocation(WorldLocation.X, WorldLocation.Y + Offset);

			FVector TangentX(2.f * Offset, 0.f, HR - HL);
			FVector TangentY(0.f, 2.f * Offset, HU - HD);
			FVector Normal = FVector::CrossProduct(TangentX, TangentY).GetSafeNormal();
			float Dot = FVector::DotProduct(Normal, FVector::UpVector);
			float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

			// add grass to landscape
			if (Height > 100.f && Height < GrassMaxHeight && Angle <= GrassMaxSlopeAngle && Height != TownPlatformHeight) // add grass
			{
				// set a random scale
				float xyScale = Stream.FRandRange(1.5f, 1.8f);
				float zScale = Stream.FRandRange(.9f, 1.2f);
				Transform.SetScale3D(FVector(xyScale, xyScale, zScale));

				GrassHISM->AddInstance(Transform);			
			}
			// add large grass to swamp
			else if (Biome == ELocationType::Swamp && Height > -15.f && Angle <= GrassMaxSlopeAngle)
			{
				// set a random scale
				float xyScale = Stream.FRandRange(1.5f, 1.8f);
				float zScale = Stream.FRandRange(1.5f, 2.2f);				
				Transform.SetScale3D(FVector(xyScale, xyScale, zScale));
				
				GrassHISM->AddInstance(Transform);
			}
		}
	}
}

