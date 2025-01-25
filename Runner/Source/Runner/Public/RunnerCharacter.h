// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RunnerGenericStruct.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "RunnerCharacter.generated.h"

class URunnerGameInstance;
class ARunnerGameMode;
class USaveGame;
class ARunnerPlayerController;
class USpringArmComponent;
class UCameraComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/** Declare a multicast delegate for the MagnetPowerupStart event */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMagnetPowerupStart);

/** Declare a multicast delegate for the MagnetPowerupEnd event */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMagnetPowerupEnd);

/**
 *  Character class for Runner project
 */
UCLASS(config=Game)
class ARunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARunnerCharacter();

	virtual void Tick(float DeltaTime) override;
			
protected:
	virtual void BeginPlay() override;

/**
 *  -----------------------------------
 *  Basic Components
 *  -----------------------------------
 */ 
public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default|Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default|Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** Returns CameraBoom subobject */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	/** Returns FollowCamera subobject */
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
/**
 *  -----------------------------------
 *  Player Movement - Slide
 *  -----------------------------------
 */
public:
	/** Variable to indicate if player is currently sliding */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default|Movement|Slide")
	bool bIsSliding = false;

	/** Variable to define the length of sliding animation */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default|Movement|Slide")
	float SlidingDuration = 1;
	
	/** Start sliding */
	void StartSlide();

protected:
	/** Timer handle for the sliding animation */
	FTimerHandle SlidingTimerHandle;
	
	/** Timer call back function to handle settings when sliding finished */
	void EndSlide();

/**
 *  -----------------------------------
 *  Player Movement - Switch Lane
 *  -----------------------------------
 */
public:
	/** LaneYOffsets index */
	UPROPERTY(EditAnywhere, Category = "Default|Movement|SwitchLane")
	int32 LaneIndex = 1;
	
	/** Y-axis Lane offsets for spawn position */
	UPROPERTY(EditAnywhere, Category = "Default|Movement|SwitchLane")
	TArray<float> LaneYOffsets = {-325, 0, 325};

	/** Time it takes to switch lane in seconds */
	UPROPERTY(EditAnywhere, Category = "Default|Movement|SwitchLane")
	float LaneSwitchDuration = 0.2;

	/** Curve for switch lane timeline */
	UPROPERTY(EditAnywhere, Category = "Default|Movement|SwitchLane")
	UCurveFloat* LaneSwitchCurve;
	
	/** Variable to indicate if player is currently switching lane */
	UPROPERTY(BlueprintReadWrite)
	bool bIsSwitchingLane = false;

	/** Switch lane based on the index update */
	void SwitchLane(int32 LaneOffset);

protected:
	/** Struct to manage data required for lane switch */
	FLaneSwitchData LaneSwitchData;
	
	/** Timeline for lane switch */
	FTimeline LaneSwitchTimeline;

	/** Initialize timelinefor lane switch */
	void InitTimelineForLaneSwitch();
	
	/** Timeline progress for lane switch */
	UFUNCTION()
	void OnLaneSwitchProgress(float Value);

	/** Event to handle when the lane switch timeline finishes */
	UFUNCTION()
	void OnLaneSwitchFinished();
	
/**
 *  -----------------------------------
 *  Gameloop
 *  -----------------------------------
 */
public:
	/** Variable to indicate if player is dead */
	UPROPERTY(BlueprintReadWrite)
	bool bIsDead = false;

	/** Handle steps requires to resume game play */
	UFUNCTION(BlueprintCallable)
	void ResumeGameplay();

	/** Handle activities when player is dead */
	UFUNCTION(BlueprintCallable)
	void PlayerDeath();
	
protected:
	/** Pointer to game mode */
	TObjectPtr<ARunnerGameMode> MyGameMode;

	/** Pointer to game instance */
	TObjectPtr<URunnerGameInstance> MyGameInstance;

	/** Timer handle for the pause delay */
	FTimerHandle TimerHandle_PauseGame;

	/** Retrieve player character from game instance */
	void SpawnSelectedCharacter();
	
	/** Respawn player to the desire lane at the same x-axis */
	void RespawnPlayerAfterDeath(float LanePositionY);

	/** Toggle player input */
	void TogglePlayerInput(bool bEnabled);
	
	/** Timer callback function to pause the game */
	void PauseGameAfterDelay() const;

	/** To destroy any obstacles on the player respawn location*/
	void SpawnObstacleDestroyer(const FTransform& InTransform) const;

/**
 *  -----------------------------------
 *  Widget
 *  -----------------------------------
 */
public:
	/** Variable to store game over widget */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default|Widget")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	/** Variable to store game play widget */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default|Widget")
	TSubclassOf<UUserWidget> GamePlayWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default|Widget")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default|Widget")
	TSubclassOf<UUserWidget> ResumePopupWidgetClass;

	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable)
	void ShowResumePopup();

private:
	/** Variable to store pause menu widget instance */
	UPROPERTY()
	TObjectPtr<UUserWidget> PauseMenuWidgetInstance = nullptr;

	/** Add give widget class to the viewport */
	void AddWidgetToViewPort(const TSubclassOf<UUserWidget>& InWidgetClass, bool bShowMouseCursor) const;

/**
 *  -----------------------------------
 *  Game Features
 *  -----------------------------------
 */
public:
	/** Variable to indicate if player holds magnet */
	UPROPERTY(BlueprintReadWrite)
	bool bIsMagnetActive = false;

	/** Variable to indicate how much time left for magnet effect */
	UPROPERTY(BlueprintReadWrite)
	float MagnetTimer = 0;

	/** Variable to indicate the duration of magnet effect */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Features|Magnet")
	float MagnetDuration = 3;
	
	/** Expose the magnet powerup start delegate */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMagnetPowerupEnd OnMagnetPowerupStart;

	/** Expose the magnet powerup end delegate */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMagnetPowerupEnd OnMagnetPowerupEnd;

	/** Start magnet power up effect */
	UFUNCTION(BlueprintCallable)
	void MagnetPowerupStart();
	
protected:
	/** Timer handler for the magnet active duration */
	FTimerHandle TimerHandle_Magnet;

	/** Update magnet timer variable based on given delta time*/
	void UpdateMagnetTimer(float DeltaTime);

	/** Reset variables when magnet power up effect end */
	void MagnetPowerupEnd();
};

