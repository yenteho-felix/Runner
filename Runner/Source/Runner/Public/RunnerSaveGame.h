// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RunnerSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class RUNNER_API URunnerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
	int32 HighScore;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
	int32 TotalCoins;
};
