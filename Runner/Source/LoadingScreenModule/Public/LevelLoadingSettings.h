// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LevelLoadingSettings.generated.h"

/**
 *  Loading Screen Settings
 */
UCLASS(Config = "Game", Defaultconfig, meta = (DisplayName = "Level Loading Settings"))
class LOADINGSCREENMODULE_API ULevelLoadingSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	/** Array of soft references to map assets that should display loading screens */
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (AllowedClasses = "World"))
	TArray<FSoftObjectPath> MapsWithLoadingScreens;

	/** Soft reference to the background image asset for the loading screen */
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (AllowedClasses = "World"))
	FSoftObjectPath BackgroundImage;

	/** Minimum duration that the loading screen will be displayed */
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (AllowedClasses = "World"))
	float MinLoadingScreenDisplayTime = 3.0f;
};
