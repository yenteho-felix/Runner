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

	/*
	 * Game Progress
	 */
public:
	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable)
	void SetTotalCoins(int32 Value) const;
	
	UFUNCTION(BlueprintCallable)
	int32 GetTotalCoins() const;

	UFUNCTION(BlueprintCallable)
	void SetHighScore(int32 Value) const;
	
	UFUNCTION(BlueprintCallable)
	int32 GetHighScore() const;
	
protected:
	UPROPERTY()
	FString MySlotName = TEXT("RunnerGameProgress");
	
	UPROPERTY()
	int32 MyUserIndex = 0;
	
	UPROPERTY()
	URunnerSaveGame* MySaveGame = nullptr;

	void CreateSaveGameObject(const TSubclassOf<URunnerSaveGame>& SaveGameClass);
	
	void LoadGameFromSlot(const FString& SlotName, const int32 UserIndex);
	
	void SaveGameToSlot(URunnerSaveGame* SaveGameObject, const FString& SlotName, const int32 UserIndex) const;

	/*
	 * Character Selected by Player
	 */
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	TSubclassOf<ACharacter> PlayerCharacterClass;
};
