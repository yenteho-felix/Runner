// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerGameInstance.h"
#include "RunnerSaveGame.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void URunnerGameInstance::Init()
{
	Super::Init();
	
	if (!UGameplayStatics::DoesSaveGameExist(MySlotName, MyUserIndex))
	{
		CreateSaveGameObject(URunnerSaveGame::StaticClass());
	}
	else
	{
		LoadGameFromSlot(MySlotName, MyUserIndex);
	}
}

void URunnerGameInstance::SetTotalCoins(const int32 Value) const
{
	MySaveGame->TotalCoins = Value;
	SaveGameToSlot(MySaveGame, MySlotName, MyUserIndex);
}

int32 URunnerGameInstance::GetTotalCoins() const
{
	return MySaveGame->TotalCoins;
}

void URunnerGameInstance::SetHighScore(const int32 Value) const
{
	MySaveGame->HighScore = Value;
	SaveGameToSlot(MySaveGame, MySlotName, MyUserIndex);
}

int32 URunnerGameInstance::GetHighScore() const
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
