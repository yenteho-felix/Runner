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

	FSpawnSettings()
		: bEnabled(true)
		, bRandomRotator(false)
		, bRandomZaxis(false)
		, ActorNum(1)
		, PointsPerLane(6)
		, LaneYOffsets({0})
		, XOffset(0)
		, ZOffset(0)
		, ActorRotator(FRotator::ZeroRotator)
		, SpawnIntervalBase(1)
		, SpawnIntervalRandomOffset(0) {}

	/** Enable or disable object spawning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	bool bEnabled;

	/** Enable random rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	bool bRandomRotator;

	/** Enable random Y-axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	bool bRandomZaxis;

	/** The actor classes to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TArray<TSubclassOf<AActor>> ActorClasses;

	/** The number of actors to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 ActorNum;

	/** Spawn points per lane */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 PointsPerLane;

	/** Y-axis Lane offsets for spawn position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TArray<float> LaneYOffsets;
	
	/** X-axis offset for spawn position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 XOffset;

	/** Z-axis offset for spawn position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 ZOffset;

	/** Rotation to apply to spawned actors */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	FRotator ActorRotator;

	/** Define the number of tiles that must be passed before the item is spawned. */
	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	int SpawnIntervalBase;
	
	/** Define the random spawn offset */
	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	int SpawnIntervalRandomOffset;
};

/**
 * Data for handling lane switch locations
 */
USTRUCT()
struct FLaneSwitchData
{
	GENERATED_BODY()

	/** Start location of the lane switch */
	FVector StartLocation;

	/** Target location of the lane switch */
	FVector EndLocation;

	/** Target lane index */
	int32 NewLaneIndex;

	/** Constructor for initialization */
	FLaneSwitchData(): NewLaneIndex(0) {}

	FLaneSwitchData(const FVector& Start, const FVector& End, const int32 LaneIndex)
		: StartLocation(Start), EndLocation(End), NewLaneIndex(LaneIndex) {}
};

/**
 *  Generic Struct
 */
class RUNNER_API RunnerGenericStruct
{
public:
	RunnerGenericStruct();
	~RunnerGenericStruct();
};

