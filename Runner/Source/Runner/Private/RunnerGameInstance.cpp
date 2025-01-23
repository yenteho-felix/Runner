// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerGameInstance.h"

#include "LoadingScreenModule.h"
#include "RunnerSaveGame.h"
#include "Kismet/GameplayStatics.h"

void URunnerGameInstance::Init()
{
	Super::Init();

	// Save or retrieve data in save game
	if (!UGameplayStatics::DoesSaveGameExist(MySlotName, MyUserIndex))
	{
		CreateSaveGameObject(URunnerSaveGame::StaticClass());
	}
	else
	{
		LoadGameFromSlot(MySlotName, MyUserIndex);
	}

	// Retrieve data from SaveGame file
	CachedTotalCoins = GetTotalCoinsFromSaveGame();
	CachedHighScore = GetHighScoreFromSaveGame();

	// Bind the PreLoadMap delegate to function handler
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &URunnerGameInstance::BeginLoadingScreen);

	// Bind the PostLoadMapWithWorld delegate to function handler
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &URunnerGameInstance::EndLoadingScreen);
}

void URunnerGameInstance::SetTotalCoinsToSaveGame(const int32 Value) const
{
	MySaveGame->TotalCoins = Value;
	SaveGameToSlot(MySaveGame, MySlotName, MyUserIndex);
}

int32 URunnerGameInstance::GetTotalCoinsFromSaveGame() const
{
	return MySaveGame->TotalCoins;
}

void URunnerGameInstance::SetHighScoreToSaveGame(const int32 Value) const
{
	if (Value > MySaveGame->HighScore)
	{
		MySaveGame->HighScore = Value;
		SaveGameToSlot(MySaveGame, MySlotName, MyUserIndex);
	}
}

int32 URunnerGameInstance::GetHighScoreFromSaveGame() const
{
	return MySaveGame->HighScore;
}

void URunnerGameInstance::CreateSaveGameObject(const TSubclassOf<URunnerSaveGame>& SaveGameClass)
{
	if (!SaveGameClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid SaveGame class provided"));
		return;
	}
	
	MySaveGame = Cast<URunnerSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	if (!MySaveGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create SaveGame object"));
	}
}

void URunnerGameInstance::LoadGameFromSlot(const FString& SlotName, const int32 UserIndex)
{
	MySaveGame = Cast<URunnerSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (!MySaveGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load save game from slot %s"), *SlotName);
	}
}

void URunnerGameInstance::SaveGameToSlot(URunnerSaveGame* SaveGameObject, const FString& SlotName, const int32 UserIndex) const
{
	const bool bSuccess = UGameplayStatics::SaveGameToSlot(MySaveGame, SlotName, UserIndex);
	if (!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to write save game to slot"));
	}
}

void URunnerGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	UE_LOG(LogTemp, Display, TEXT("URunnerGameInstance::BeginLoadingScreen: %s"), *InMapName);

	// Try to get the loading screen module
	FLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<FLoadingScreenModule>("LoadingScreenModule");
	if (LoadingScreenModule)
	{
		LoadingScreenModule->StartLoadingScreen(InMapName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("URunnerGameInstance::BeginLoadingScreen: LoadingScreenModule not found"));
	}
}

void URunnerGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	UE_LOG(LogTemp, Display, TEXT("URunnerGameInstance::EndLoadingScreen: %s"), *InLoadedWorld->GetName());
}
