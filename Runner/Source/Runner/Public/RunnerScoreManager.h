// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RunnerScoreManager.generated.h"

/**
 * 
 */
UCLASS()
class RUNNER_API URunnerScoreManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default|ScoreManager")
	int32 CurrentScore = 0;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default|ScoreManager")
	int32 CurrentCoins = 0;

	UFUNCTION(BlueprintCallable)
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void AddCoins(int32 Amount);
};
