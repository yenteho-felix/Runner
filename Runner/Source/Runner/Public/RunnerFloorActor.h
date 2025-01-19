// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RunnerCollisionInterface.h"
#include "GameFramework/Actor.h"
#include "RunnerFloorActor.generated.h"

class UBoxComponent;
class UArrowComponent;
class ARunnerGameMode;
class URunnerSpawnObjectsComponent;

/**
 * Manages individual floor tiles in the runner game.
 * Handles spawning of obstacles, coins, power-ups, and managing player collision events.
 */
UCLASS()
class RUNNER_API ARunnerFloorActor : public AActor, public IRunnerCollisionInterface
{
	GENERATED_BODY()
	
public:	
	ARunnerFloorActor();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostEditMove(bool bFinished) override;

protected:
	virtual void BeginPlay() override;

/**
 *	--------------------------------------------------
 *  Basic scene components visible in the viewport
 *	--------------------------------------------------
 */
public:
	/** Root scene for organizing hierarchy */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Scene;
	
	/** Collision box for detecting player overlap */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxCollision;

	/** Arrow component marking the attach point for the next floor */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UArrowComponent> AttachpointArrow;

	/** FloorComponent representing the floor */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UChildActorComponent> FloorComponent;

protected:
	/**
	 * Called when an overlap event occurs on the BoxCollision component.
	 * Forwards the event to the interface method for further processing.
	 */
	UFUNCTION(BlueprintCallable)
	void OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
/**
 *  -----------------------------------------------
 *  IRunnerCollisionInterface Implementation
 *  -----------------------------------------------
 */
public:
	/** Returns the world transform of the AttachpointArrow. Used for determining the attach location for the next floor tile. */
	UFUNCTION(BlueprintCallable)
	virtual FVector GetAttachLocation_Implementation() const override;

	/** Handles actions triggered when the player collides with the BoxCollision component. */
	UFUNCTION(BlueprintCallable)
	virtual void HandleBoxCollision_Implementation(AActor* OverlappingActor) override;

/**
 *  -----------------------------------------------
 *   Spawn Objects
 *  -----------------------------------------------
 */
public:
	/** Component to spawn Coin */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Spawner Settings")
	TObjectPtr<URunnerSpawnObjectsComponent> CoinSpawner;

	/** Component to spawn Powerup */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Spawner Settings")
	TObjectPtr<URunnerSpawnObjectsComponent> PowerupSpawner;

	/** Component to spawn Obstacle */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Spawner Settings")
	TObjectPtr<URunnerSpawnObjectsComponent> ObstacleSpawner;

	/** Component to spawn Moving Obstacle */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Spawner Settings")
	TObjectPtr<URunnerSpawnObjectsComponent> MovingObstacleSpawner;

	/** Spawn all objects based on constraints */
	UFUNCTION(BlueprintCallable)
	void SpawnAllObjects();
	
protected:
	/** Return true if objects should spawn */
	UFUNCTION(BlueprintCallable)
	bool ShouldSpawnObjects(int32 SpawnIntervalBase, int32 SpawnIntervalRandomOffset) const;
	
/**
 * -----------------------------------------------
 *  Player Speed Adjustment
 * -----------------------------------------------
 */
public:
	/** The increment to player speed after passing a floor tile. */
	UPROPERTY(EditAnywhere, Category = "Default|Speed Settings")
	float SpeedIncrement = 10;

	/** The maximum speed the player can reach. */
	UPROPERTY(EditAnywhere, Category = "Default|Speed Settings")
	float MaxSpeed = 1500;

protected:
	/** Increases the player's speed by the specified increment, capping it at the maximum speed. */
	UFUNCTION(BlueprintCallable)
	void IncreaseSpeed(AActor* Actor, float Increment, float Max);

/**
 * -----------------------------------------------
 *  Player Score Update
 * -----------------------------------------------
 */
public:
	/** The score increment when passing a tile */
	UPROPERTY(EditAnywhere, Category = "Default|Score Settings")
	int32 ScoreIncrement = 1;

protected:
	UFUNCTION(BlueprintCallable)
	void IncreaseScore(int32 Amount);
};
