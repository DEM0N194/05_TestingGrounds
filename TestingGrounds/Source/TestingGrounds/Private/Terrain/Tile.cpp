// Fill out your copyright notice in the Description page of Project Settings.

#include "Terrain/Tile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "ActorPool.h"
#include "NavigationSystem.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

void ATile::SetPool(UActorPool* ActorPool)
{
	Pool = ActorPool;

	PositionNavMeshBoundsVolume();
	UNavigationSystemV1::GetCurrent(GetWorld())->Build();
}

void ATile::PositionNavMeshBoundsVolume()
{
	NavMeshBoundsVolume = Pool->Checkout();
	if (NavMeshBoundsVolume == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Not enough actors in pool."), *GetName());
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[%s] Checked out: %s"), *GetName(), *NavMeshBoundsVolume->GetName());
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation()+FVector(2000.0f, 0.0f, 0.0f));
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, const FSpawnParams& SpawnParams)
{
	TArray<FSpawnPosition> SpawnPositions = RandomSpawnPostitions(SpawnParams);
	for (FSpawnPosition SpawnPosition : SpawnPositions)
	{
		PlaceActor(ToSpawn, SpawnPosition);
	}
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	Pool->Return(NavMeshBoundsVolume);
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FVector Min(0, -2000, 0);
	FVector Max(4000, 2000, 0);
	FBox Bounds(Min, Max);
	const int32 MAX_ATTEMPTS = 25;
	for (int i = 0; i < MAX_ATTEMPTS; i++)
	{
		FVector CandidateSpot = FMath::RandPointInBox(Bounds);
		if (CanSpawnAtLocation(CandidateSpot, Radius))
		{
			OutLocation = CandidateSpot;
			return true;
		}
	}
	return false;
}

TArray<FSpawnPosition> ATile::RandomSpawnPostitions(const FSpawnParams& SpawnParams)
{
	TArray<FSpawnPosition> SpawnPositions;
	int32 NumberToSpawn = FMath::RandRange(SpawnParams.MinSpawn, SpawnParams.MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(SpawnParams.MinScale, SpawnParams.MaxScale);
		if (FindEmptyLocation(SpawnPosition.Location, SpawnParams.Radius * SpawnPosition.Scale))
		{
			SpawnPosition.Rotation = FMath::RandRange(-180.0f, 180.0f);
			SpawnPositions.Add(SpawnPosition);
		}
	}
	return SpawnPositions;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, const FSpawnPosition& SpawnPosition)
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->SetActorRelativeLocation(SpawnPosition.Location);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorRotation(FRotator(0.0f, SpawnPosition.Rotation, 0.0f));
	Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius)
{
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);
	FColor ResultColor = HasHit ? FColor::Red : FColor::Green;
	// DrawDebugCapsule(GetWorld(), GlobalLocation, 0, Radius, FQuat::Identity, ResultColor, true, 100);
	return !HasHit;
}
