// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RunnerGameMode.generated.h"

class URunnerTileManager;
class URunnerScoreManager;

/**
 *  Game mode for Runner project
 */
UCLASS(minimalapi)
class ARunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARunnerGameMode();

private:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<URunnerTileManager> RunnerFloorManager;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<URunnerTileManager> RunnerSkylineManager;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<URunnerScoreManager> RunnerScoreManager;
};



