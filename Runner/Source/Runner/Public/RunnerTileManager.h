// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RunnerTileManager.generated.h"

class ARunnerTileActor;
/**
 *  Spawning tiles based on given tile class and constraints
 */
UCLASS()
class RUNNER_API URunnerTileManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<AActor> TileClass;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 TilesAheadPlayer;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 TilesBehindPlayer;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector NextAttachPoint;

	UFUNCTION(BlueprintCallable)
	void ExtendTile();

	UFUNCTION(BlueprintCallable)
	void InitiateTile();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> TileActorArray;

	UFUNCTION(BlueprintCallable)
	void AddTile(const FTransform& AttachPoint);

	UFUNCTION(BlueprintCallable)
	void RemoveTile();
};
