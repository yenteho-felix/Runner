// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunnerGameMode.h"
#include "RunnerTileManager.h"
#include "RunnerScoreManager.h"
#include "UObject/ConstructorHelpers.h"

ARunnerGameMode::ARunnerGameMode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	RunnerFloorManager = CreateDefaultSubobject<URunnerTileManager>("FloorManager");
	RunnerSkylineManager = CreateDefaultSubobject<URunnerTileManager>("SkylineManager");
	RunnerScoreManager = CreateDefaultSubobject<URunnerScoreManager>("ScoreManager");
}

void ARunnerGameMode::BeginPlay()
{
	Super::BeginPlay();

	RunnerFloorManager->InitiateTile();
	RunnerSkylineManager->InitiateTile();
}
