// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerPlayerController.h"
#include "RunnerCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

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