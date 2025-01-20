// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RunnerCollisionInterface.h"
#include "GameFramework/Actor.h"
#include "RunnerSkylineActor.generated.h"

class UBoxComponent;
class UArrowComponent;
class URunnerSpawnObjectsComponent;

UCLASS()
class RUNNER_API ARunnerSkylineActor : public AActor, public IRunnerCollisionInterface
{
	GENERATED_BODY()
	
public:	
	ARunnerSkylineActor();

	virtual void OnConstruction(const FTransform& Transform) override;

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

	/** FloorComponent representing the skyline on the left */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<UChildActorComponent> LeftSkyline;
	
	/** FloorComponent representing the skyline on the right */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<UChildActorComponent> RightSkyline;

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
	/** Component to spawn buildings on the left side */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Spawner Settings")
	TObjectPtr<URunnerSpawnObjectsComponent> LeftSpawner;

	/** Component to spawn buildings on the right side */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Spawner Settings")
	TObjectPtr<URunnerSpawnObjectsComponent> RightSpawner;

	/** Spawn all objects based on constraints */
	UFUNCTION(BlueprintCallable)
	void SpawnAllObjects();
};
