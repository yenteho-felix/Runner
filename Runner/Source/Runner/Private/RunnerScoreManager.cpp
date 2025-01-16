// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerScoreManager.h"

void URunnerScoreManager::AddScore(int32 Amount)
{
	CurrentScore += Amount;
}

void URunnerScoreManager::AddCoins(int32 Amount)
{
	CurrentCoins += Amount;
}
