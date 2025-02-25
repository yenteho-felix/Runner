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
	
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Default|ScoreManager")
	int32 HighScore;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Default|ScoreManager")
	int32 CurrentScore;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Default|ScoreManager")
	int32 CurrentCoins;

	UFUNCTION(BlueprintCallable)
	void AddScore(int32 Value);

	UFUNCTION(BlueprintCallable)
	void AddCoins(int32 Value);

	UFUNCTION(BlueprintCallable)
	void SaveHighScore();

	UFUNCTION(BlueprintCallable)
	void SaveTotalCoin();
};
