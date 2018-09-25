// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

USTRUCT()
struct FSpawnPosition
{
	GENERATED_BODY()

	FVector Location;
	float Rotation;
	float Scale;
};

USTRUCT(BlueprintType)
struct FSpawnParams
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 MinSpawn = 1;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxSpawn = 1;

	UPROPERTY(BlueprintReadWrite)
	float Radius = 500.0f;

	UPROPERTY(BlueprintReadWrite)
	float MinScale = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	float MaxScale = 1.0f;
};

class UActorPool;

UCLASS()
class TESTINGGROUNDS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	UFUNCTION(BlueprintCallable, Category = "ActorPool")
	void SetPool(UActorPool* ActorPool);

	UFUNCTION(BlueprintCallable)
	void PlaceActors(TSubclassOf<AActor> ToSpawn, const FSpawnParams& SpawnParams);

	UFUNCTION(BlueprintCallable, meta = (ShortTooltip = "Scaling Ignored", ToolTip = "MinScale and MaxScale are irrelevant."))
	void PlaceAIPawns(TSubclassOf<APawn> ToSpawn, const FSpawnParams& SpawnParams);

private:

	void PositionNavMeshBoundsVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	bool FindEmptyLocation(FVector& OutLocation, float Radius);

	template<class T>
	void RandomlyPlaceActors(TSubclassOf<T> ToSpawn, const FSpawnParams& SpawnParams);

	void PlaceActor(TSubclassOf<AActor> ToSpawn, const FSpawnPosition& SpawnPosition);

	void PlaceActor(TSubclassOf<APawn> ToSpawn, const FSpawnPosition& SpawnPosition);

	bool CanSpawnAtLocation(FVector Location, float Radius);
	
	UActorPool* Pool;

	AActor* NavMeshBoundsVolume;

};
