// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "RunnerGenericStruct.generated.h"

/**
 *  Settings to configure how the objects should be placed
 */
USTRUCT(BlueprintType)
struct FSpawnSettings
{
	GENERATED_BODY()

	/** Enable or disable object spawning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	bool bEnabled = true;

	/** The actor classes to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TArray<TSubclassOf<AActor>> ActorClasses;

	/** The number of actors to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 ActorNum = 1;

	/** Spawn points per lane */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 PointsPerLane = 6;

	/** Y-axis Lane offsets for spawn position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TArray<float> LaneYOffsets = {-325, 0, 325};
	
	/** X-axis offset for spawn position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 XOffset = 0;

	/** Z-axis offset for spawn position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 ZOffset = 0;

	/** Rotation to apply to spawned actors */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	FRotator ActorRotator = FRotator::ZeroRotator;

	/** Define the number of tiles that must be passed before the item is spawned. */
	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	int SpawnIntervalBase = 1;
	
	/** Define the random spawn offset */
	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	int SpawnIntervalRandomOffset = 0;
};

/**
 * 
 */
class RUNNER_API RunnerGenericStruct
{
public:
	RunnerGenericStruct();
	~RunnerGenericStruct();
};

