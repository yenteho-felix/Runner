// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RunnerCollisionInterface.h"
#include "GameFramework/Actor.h"
#include "RunnerFloorActor.generated.h"

class UBoxComponent;
class UArrowComponent;
class ARunnerGameMode;

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
	/** Lane offsets for object spawning */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects")
	TArray<float> LaneOffsets = {-325, 0, 325};

	/** Enables or disables coin spawning */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Coin")
	bool bSpawnCoin = true;
	
	/** Classes of coin actors to spawn */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Coin")
	TArray<UClass*> CoinActorClasses;

	/** Rotation applied to spawned coins */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Coin")
	FRotator CoinRotator;

	/** Number of coins to spawn per floor */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Coin")
	int32 CoinNum = 10;
	
	/** Spawn points per lane for coins */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Coin")
	int32 CoinSpawnPointsPerLane = 6;

	/** X-axis offset for coin spawning */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Coin")
	int32 CoinXOffset = 50;

	/** Z-axis offset for coin spawning */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Coin")
	int32 CoinZOffset = 0;

	/** Enables or disables power-up spawning */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Powerup")
	bool bSpawnPowerup = false;
	
	/** Classes of power-up actors to spawn */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Powerup")
	TArray<UClass*> PowerupActorClasses;

	/** Rotation applied to spawned power-ups */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Powerup")
	FRotator PowerupRotator;

	/** Number of power-ups to spawn per floor */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Powerup")
	int32 PowerupNum = 1;
	
	/** Spawn points per lane for power-ups */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Powerup")
	int32 PowerupSpawnPointsPerLane = 6;

	/** X-axis offset for power-up spawning */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Powerup")
	int32 PowerupXOffset = 50;

	/** Z-axis offset for power-up spawning */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Powerup")
	int32 PowerupZOffset = 0;

	/** Define the number of tiles that must be passed before the item is spawned. */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Powerup")
	int PowerupSpawnIntervalBase = 5;

	/** Define the random spawn offset */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Powerup")
	int PowerupSpawnIntervalRandomOffset = 1;

	/** Enables or disables obstacle spawning */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Obstacle")
	bool bSpawnObstacle = true;
	
	/** Classes of obstacle actors to spawn */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Obstacle")
	TArray<UClass*> ObstacleActorClasses;

	/** Rotation applied to spawned obstacles */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Obstacle")
	FRotator ObstacleRotator;

	/** Number of obstacles to spawn per floor */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Obstacle")
	int32 ObstacleNum = 1;
	
	/** Spawn points per lane for obstacles */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Obstacle")
	int32 ObstacleSpawnPointsPerLane = 2;

	/** X-axis offset for obstacle spawning */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Obstacle")
	int32 ObstacleXOffset = 50;

	/** Z-axis offset for obstacle spawning */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Obstacle")
	int32 ObstacleZOffset = 0;

	/** Determines if moving obstacles should be spawned. */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Moving Obstacles")
	bool bSpawnMovingObstacles = false;

	/** Array of moving obstacle classes to be spawned. */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Moving Obstacles")
	TArray<UClass*> MovingObstaclesActorClasses;

	/** Rotation applied to spawned moving obstacles. */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Moving Obstacles")
	FRotator MovingObstaclesRotator;

	/** Number of moving obstacles to spawn. */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Moving Obstacles")
	int32 MovingObstaclesNum = 1;

	/** Number of spawn points per lane for moving obstacles. */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Moving Obstacles")
	int32 MovingObstaclesSpawnPointsPerLane = 1;

	/** X-axis offset for moving obstacles' spawn points. */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Moving Obstacles")
	int32 MovingObstaclesXOffset = 50;

	/** Z-axis offset for moving obstacles' spawn points. */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Moving Obstacles")
	int32 MovingObstaclesZOffset = 0;

	/** Base interval (in tiles passed) between spawns of moving obstacles. */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Moving Obstacles")
	int MovingObstaclesSpawnIntervalBase = 10;

	/** Random offset added to the base spawn interval for variability. */
	UPROPERTY(EditAnywhere, Category = "Default|Spawn Objects|Moving Obstacles")
	int MovingObstaclesSpawnIntervalRandomOffset = 3;

	/** Spawns all defined objects, such as coins, powerups, and obstacles. */
	UFUNCTION(BlueprintCallable)
	void SpawnAllObjects();

	/** Removes all spawned objects, cleaning up the scene. */
	UFUNCTION(BlueprintCallable)
	void RemoveAllObjects();
	
protected:
	/** Generates spawn transforms based on the specified parameters, such as lane offsets and component positions. */
	UFUNCTION(BlueprintCallable)
	TArray<FTransform>& GenerateSpawnTransforms(const TArray<float>& LaneOffsetsY, int32 NumPointsPerLane, int32 XOffset, int32 ZOffset, const FColor& ArrowColor);

	/** Spawns child actor components based on the provided actor classes and spawn transforms. */
	UFUNCTION(BlueprintCallable)
	void SpawnChildActorComponents(const TArray<UClass*> ActorClasses, int32 ActorNum, FRotator ActorRotator, TArray<FTransform>& SpawnTransforms, USceneComponent* AttachParent);

private:
	/** Array storing created arrow components for later removal or cleanup. */
	TArray<UArrowComponent*> CreatedArrowComponents;

	/** Array storing created child actor components for later removal or cleanup. */
	TArray<UChildActorComponent*> CreatedChildActorComponents;

	/** Determines if objects should spawn based on the interval and random offset. */
	bool ShouldSpawnObjects(int32 SpawnIntervalBase, int32 SpawnIntervalRandomOffset) const;

	/** Adds an arrow component for visualizing or marking a location and attaches it to a parent component. */
	FTransform AddArrowComponent(FColor Color, const FVector& Location, USceneComponent* AttachParent);

	/** Removes all arrow components from the scene. */
	void RemoveArrowComponent();

	/** Calculates the floor extent based on the given child actor component. */
	FVector CalculateFloorExtent(const UChildActorComponent* ChildActorComponent) const;

/**
 * -----------------------------------------------
 *  Player Speed Adjustment
 * -----------------------------------------------
 */
public:
	/** The increment to player speed after passing a floor tile. */
	UPROPERTY(EditAnywhere, Category = "Default|Speed Adjustment")
	float SpeedIncrement = 10;

	/** The maximum speed the player can reach. */
	UPROPERTY(EditAnywhere, Category = "Default|Speed Adjustment")
	float MaxSpeed = 1500;

protected:
	/** Increases the player's speed by the specified increment, capping it at the maximum speed. */
	UFUNCTION(BlueprintCallable)
	void IncreaseSpeed(AActor* Actor, float Increment, float Max);
};
