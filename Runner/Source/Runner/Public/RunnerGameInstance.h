// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/UObjectGlobals.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RunnerGameInstance.generated.h"

class URunnerSaveGame;

/**
 * 
 */
UCLASS()
class URunnerGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	
/**
 *  Game Progress
 */
public:
	/** Total coins of the player */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int32 CachedTotalCoins;

	/** Highest score of all time */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int32 CachedHighScore;
	
	UFUNCTION(BlueprintCallable)
	void SetTotalCoinsToSaveGame(int32 Value) const;
	
	UFUNCTION(BlueprintCallable)
	int32 GetTotalCoinsFromSaveGame() const;

	UFUNCTION(BlueprintCallable)
	void SetHighScoreToSaveGame(int32 Value) const;
	
	UFUNCTION(BlueprintCallable)
	int32 GetHighScoreFromSaveGame() const;
	
protected:
	UPROPERTY()
	FString MySlotName = TEXT("RunnerGameProgress");
	
	UPROPERTY()
	int32 MyUserIndex = 0;
	
	UPROPERTY()
	TObjectPtr<URunnerSaveGame> MySaveGame;

	void CreateSaveGameObject(const TSubclassOf<URunnerSaveGame>& SaveGameClass);
	
	void LoadGameFromSlot(const FString& SlotName, const int32 UserIndex);
	
	void SaveGameToSlot(URunnerSaveGame* SaveGameObject, const FString& SlotName, const int32 UserIndex) const;

/**
 *  Character Selected by Player
 */
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	TSubclassOf<ACharacter> PlayerCharacterClass;

/**
 *  Loading Screen
 */
public:
	/** Function handler when loading screen starts */
	void BeginLoadingScreen(const FString& InMapName);

	/** Function handler when loading screen ends */
	void EndLoadingScreen(UWorld* InLoadedWorld);
};
