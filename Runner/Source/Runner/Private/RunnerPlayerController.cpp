// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerPlayerController.h"
#include "RunnerCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"

void ARunnerPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ARunnerPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Ensure the Enhanced Input Component is used
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Binding actions
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARunnerPlayerController::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARunnerPlayerController::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ARunnerPlayerController::Jump);
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Triggered, this, &ARunnerPlayerController::Slide);
		//EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &ARunnerPlayerController::TogglePauseMenu);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ARunnerPlayerController::TogglePauseMenu);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find an Enhanced Input Component!"));
	}
}

void ARunnerPlayerController::Move(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		FVector2D MovementVector = Value.Get<FVector2D>();

		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARunnerPlayerController::Look(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		// add yaw and pitch input to controller
		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ARunnerPlayerController::Jump()
{
	if (ACharacter* MyCharacter = Cast<ACharacter>(GetPawn()))
	{
		MyCharacter->Jump();
	}
}

void ARunnerPlayerController::Slide()
{
	if (ARunnerCharacter* MyCharacter = Cast<ARunnerCharacter>(GetPawn()))
	{
		FVector Impulse(0.0f, 0.0f, -1000.0f);
		MyCharacter->LaunchCharacter(Impulse, true, false);
		MyCharacter->PlaySlideMontage();
	}
}

void ARunnerPlayerController::ShowPauseMenu()
{
	UE_LOG(LogTemp, Display, TEXT("Open Pause Menu"));
	//PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
	if (!PauseMenuWidgetClass)
	{
		return;
	}
	
	if (!PauseMenuWidget)
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
	}
	
	if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->AddToViewport();
	
		// Set input mode for UI
		FInputModeUIOnly InputMode;
		if (PauseMenuWidget->IsFocusable())
		{
			InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
		}
		SetInputMode(InputMode);
	
		// Show mouse cursor
		bShowMouseCursor = true;
	
		// Pause the game
		SetPause(true);
	}
}

void ARunnerPlayerController::HidePauseMenu()
{
	UE_LOG(LogTemp, Display, TEXT("Close Pause Menu"));
	//PauseMenuWidget = nullptr;
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	
		// Reset input mode
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	
		// Hide mouse cursor
		bShowMouseCursor = false;
	
		// Resume the game
		SetPause(false);
	}
}

void ARunnerPlayerController::TogglePauseMenu()
{
	if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
	{
		HidePauseMenu();
	}
	else
	{
		ShowPauseMenu();
	}
}