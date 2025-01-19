// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RunnerGenericStruct.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "RunnerSpawnObjectsComponent.generated.h"

class UArrowComponent;
class UChildActorComponent;

/**
 * An actor component responsible for spawning objects attached to the specified child actor component.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUNNER_API URunnerSpawnObjectsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URunnerSpawnObjectsComponent();

public:
	/** Spawn options */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Objects")
	FSpawnSettings SpawnSettings;

	/** Color of visualize arrow */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Objects")
	FColor ArrowColor = FColor::Green;
	
	/** Spawns the objects in the editor */
	UFUNCTION(BlueprintCallable)
	void SpawnObjects(UChildActorComponent* AttachParent);

	/** Removes the objects */
	UFUNCTION(BlueprintCallable)
	void RemoveObjects();

protected:
	/** Array to store spawned objects */
	TArray<UChildActorComponent*> SpawnedObjects;

	/** Arrow components used for visualization */
	TArray<UArrowComponent*> SpawnedArrows;
	
	/** Creates and spawns a new object at the specified transform */
	void SpawnObjectClass(UClass* ActorClass, const FTransform& SpawnTransform, UChildActorComponent* AttachParent);

	/** Generates spawn transforms for the object */
	TArray<FTransform> GenerateSpawnTransform(const UChildActorComponent* AttachParent) const;

	/** Calculates the floor extent based on the given child actor component. */
	FVector CalculateFloorExtent(const UChildActorComponent* AttachParent) const;

	/** Iterate over the spawned objects and remove any that overlap with existing objects in the level. */
	void ResolveOverlaps();

	/** Adds an arrow for visualization */
	void AddArrowComponent(const FVector& Location, const FColor& Color, UChildActorComponent* AttachParent);
	
	/** Visualizes the spawn locations */
	void VisualizeSpawnLocations(const TArray<FTransform>& SpawnTransforms, UChildActorComponent* AttachParent);

	/** Remove all existing arrows */
	void RemoveArrowComponents();
};
