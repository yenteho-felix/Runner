// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerScoreManager.h"

#include "RunnerGameInstance.h"

void URunnerScoreManager::BeginPlay()
{
	Super::BeginPlay();

	CurrentScore = 0;

	if (GetWorld())
	{
		if (URunnerGameInstance* MyGameInstance = Cast<URunnerGameInstance>(GetWorld()->GetGameInstance()))
		{
			CurrentCoins = MyGameInstance->GetTotalCoinsFromSaveGame();
			HighScore = MyGameInstance->GetHighScoreFromSaveGame();
		}
	}
}

void URunnerScoreManager::AddScore(int32 Value)
{
	CurrentScore += Value;
}

void URunnerScoreManager::AddCoins(int32 Value)
{
	CurrentCoins += Value;
}

void URunnerScoreManager::SaveHighScore()
{
	if (URunnerGameInstance* MyGameInstance = Cast<URunnerGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (CurrentScore > MyGameInstance->CachedHighScore)
		{
			MyGameInstance->CachedHighScore = CurrentScore;
			MyGameInstance->SetHighScoreToSaveGame(CurrentScore);
		}
	}
}

void URunnerScoreManager::SaveTotalCoin()
{
	if (URunnerGameInstance* MyGameInstance = Cast<URunnerGameInstance>(GetWorld()->GetGameInstance()))
	{
		MyGameInstance->CachedTotalCoins = CurrentCoins;
		MyGameInstance->SetTotalCoinsToSaveGame(CurrentCoins);
	}
}
