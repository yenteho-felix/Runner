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
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
	TSubclassOf<AActor> TileClass;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
	int32 TilesAheadPlayer;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
	int32 TilesBehindPlayer;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	int32 TileCount = 1;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
	FVector FirstTileLocation;

	UFUNCTION(BlueprintCallable)
	void ExtendTile();

	UFUNCTION(BlueprintCallable)
	void InitiateTile();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	FVector TileAttachLocation = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	TArray<AActor*> TileActorArray;

	UFUNCTION(BlueprintCallable)
	void AddTile();

	UFUNCTION(BlueprintCallable)
	void RemoveTile();
};
