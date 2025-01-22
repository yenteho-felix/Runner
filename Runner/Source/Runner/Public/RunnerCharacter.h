// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
/**
 *  -----------------------------------
 *  Player Movement
 *  -----------------------------------
 */
public:
	/** Variable to indicate if player is currently sliding */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Animation")
	bool bIsSliding = false;

	/** Variable to define the length of sliding animation */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Animation")
	float AnimationLength = 1;

	/** Variable to indicate if player is currently switching lane */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Animation")
	bool bIsSwitchingLane = false;
	
	/** Switch lane implemented in blueprint since we like to use time module to slowing shift character position */
	UFUNCTION(BlueprintImplementableEvent)
	void SwitchLane(int32 LandIndex);
	
	/** Start sliding */
	void StartSlide();
	
protected:
	/** Timer handle for the sliding animation */
	FTimerHandle SlidingTimerHandle;
	
	/** Timer call back function to handle settings when sliding finished */
	void EndSlide();

	/** Moves the player forward */
	void MoveForward();
	
/**
 *  -----------------------------------
 *  Gameloop
 *  -----------------------------------
 */
public:
	/** Variable to indicate if player is dead */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Default|GameLoop")
	bool bIsDead;

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
	UFUNCTION()
	void RetrievePlayerCharacter();
	
	/** Respawn player to the desire lane at the same x-axis */
	UFUNCTION(BlueprintCallable)
	void RespawnPlayerAfterDeath(float LanePositionY);

	/** Toggle player input */
	UFUNCTION(BlueprintCallable)
	void TogglePlayerInput(bool bEnabled);
	
	/** Timer callback function to pause the game */
	void PauseGameAfterDelay() const;

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Features|Magnet")
	bool bIsMagnetActive = false;

	/** Variable to indicate how much time left for magnet effect */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Features|Magnet")
	float MagnetTimer = 0;

	/** Variable to indicate the duration of magnet effect */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Features|Magnet")
	float MagnetDuration = 0;
	
/**
 *  -----------------------------------
 *  Game Record
 *  -----------------------------------
 */
public:
	/** Please add a function description */
	void SaveHighScore() const;
	
	/** Please add a function description */
	void SaveTotalCoins() const;
};

