// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunnerCharacter.h"

#include "RunnerGameInstance.h"
#include "RunnerGameMode.h"
#include "RunnerPlayerController.h"
#include "RunnerScoreManager.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ARunnerCharacter

ARunnerCharacter::ARunnerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ARunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveForward();
}

void ARunnerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Initiate variables
	MyGameMode = Cast<ARunnerGameMode>(GetWorld()->GetAuthGameMode());
	MyGameInstance = Cast<URunnerGameInstance>(GetGameInstance());
	bIsDead = false;

	// Add game play widget
	if (GamePlayWidgetClass)
	{
		AddWidgetToViewPort(GamePlayWidgetClass, false);
	}
}

void ARunnerCharacter::StartSlide()
{
	bIsSliding = true;

	// Disable input
	if (APlayerController* MyPlayerController = Cast<APlayerController>(GetController()))
	{
		DisableInput(MyPlayerController);
	}
	
	// Disable collision
	UCapsuleComponent* MyCapsuleComponent = GetCapsuleComponent();
	if (!MyCapsuleComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCapsuleComponent() returned nullptr!"));
	}
	MyCapsuleComponent->SetCollisionResponseToChannel(ECC_Destructible, ECollisionResponse::ECR_Ignore);

	// Use Timer to end sliding
	GetWorld()->GetTimerManager().SetTimer(
		SlidingTimerHandle,
		this,
		&ARunnerCharacter::EndSlide,
		AnimationLength,
		false);
}

void ARunnerCharacter::EndSlide()
{
	bIsSliding = false;

	// Enable input
	if (APlayerController* MyPlayerController = Cast<APlayerController>(GetController()))
	{
		EnableInput(MyPlayerController);
	}

	// Restore collision response after the animation
	UCapsuleComponent* MyCapsuleComponent = GetCapsuleComponent();
	if (!MyCapsuleComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCapsuleComponent() returned nullptr!"));
	}
	MyCapsuleComponent->SetCollisionResponseToChannel(ECC_Destructible, ECollisionResponse::ECR_Block);
}

void ARunnerCharacter::MoveForward()
{
	AddMovementInput(GetActorForwardVector());
}

void ARunnerCharacter::ResumeGameplay()
{
	bIsDead = false;

	RespawnPlayerAfterDeath(0);

	TogglePlayerInput(true);

	AddWidgetToViewPort(GamePlayWidgetClass, false);

	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void ARunnerCharacter::PlayerDeath()
{
	bIsDead = true;

	TogglePlayerInput(false);

	// Show widget & enable mouse
	if (GameOverWidgetClass)
	{
		AddWidgetToViewPort(GameOverWidgetClass, true);
	}
	
	// Save current data
	SaveHighScore();
	SaveTotalCoins();
	
	// Start a timer to call PauseGameAfterDelay after 1 second
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_PauseGame, 
		this, 
		&ARunnerCharacter::PauseGameAfterDelay, 
		1.0f, 
		false
	);
}

void ARunnerCharacter::PauseGameAfterDelay() const
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void ARunnerCharacter::TogglePauseMenu()
{
	if (!PauseMenuWidgetClass)
	{
		return;
	}
	
	if (!PauseMenuWidgetInstance)
	{
		PauseMenuWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
		if (!PauseMenuWidgetInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create PauseMenuWidgetInstance"));
			return;
		}
	}

	// Show pause menu
	if (!PauseMenuWidgetInstance->IsInViewport())
	{
		PauseMenuWidgetInstance->AddToViewport();
		
		// Pause game
		if (APlayerController* MyPlayerController = Cast<APlayerController>(GetController()))
		{
			FInputModeUIOnly InputMode;
			if (PauseMenuWidgetInstance->IsFocusable())
			{
				InputMode.SetWidgetToFocus(PauseMenuWidgetInstance->TakeWidget());
			}
			MyPlayerController->SetInputMode(InputMode);
			MyPlayerController->SetShowMouseCursor(true);
			MyPlayerController->SetPause(true);
		}
	}
	// Hide pause menu
	else
	{
		PauseMenuWidgetInstance->RemoveFromParent();
		PauseMenuWidgetInstance = nullptr;
		
		// Unpause game
		if (APlayerController* MyPlayerController = Cast<APlayerController>(GetController()))
		{
			FInputModeGameOnly InputMode;
			MyPlayerController->SetInputMode(InputMode);
			MyPlayerController->SetShowMouseCursor(false);
			MyPlayerController->SetPause(false);
		}
	}
}

void ARunnerCharacter::ShowResumePopup()
{
	if (!ResumePopupWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResumePopupWidgetClass is NULL"));
		return;
	}

	AddWidgetToViewPort(ResumePopupWidgetClass, true);
}

void ARunnerCharacter::AddWidgetToViewPort(const TSubclassOf<UUserWidget>& InWidgetClass, bool bShowMouseCursor) const
{
	if (InWidgetClass)
	{
		if (UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), InWidgetClass))
		{
			WidgetInstance->AddToViewport();
		}
		
		if (APlayerController* MyPlayerController = Cast<APlayerController>(GetController()))
		{
			MyPlayerController->SetShowMouseCursor(bShowMouseCursor);
		}
	}
}

void ARunnerCharacter::SaveHighScore() const
{
	if (!MyGameMode)
	{
		return;
	}

	if (!MyGameInstance)
	{
		return;
	}

	const int32 CurrentScore = MyGameMode->RunnerScoreManager->CurrentScore;
	const int32 HighScore = MyGameInstance->GetHighScore();
	if (CurrentScore > HighScore)
	{
		MyGameInstance->SetHighScore(CurrentScore);
	}
}

void ARunnerCharacter::SaveTotalCoins() const
{
	if (!MyGameMode)
	{
		return;
	}

	if (!MyGameInstance)
	{
		return;
	}

	const int32 CurrentCoins = MyGameMode->RunnerScoreManager->CurrentCoins;
	MyGameInstance->SetTotalCoins(CurrentCoins);
}

void ARunnerCharacter::RespawnPlayerAfterDeath(float InLanePositionY)
{
	// Retrieve X-axis location of player
	float PositionX = GetCapsuleComponent()->GetRelativeLocation().X;

	// Teleport the actor to the new location
	FVector NewLocation = FVector(PositionX, InLanePositionY, 120);
	SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

	// Reset mesh relative location and rotation since character mesh may be offset from default value
	FVector MeshLocation = FVector (0, 0, -90);
	FRotator MeshRotation = FRotator(0.0f, -90.0f, 0.0f);
	GetMesh()->SetRelativeLocationAndRotation(MeshLocation, MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
}

void ARunnerCharacter::TogglePlayerInput(bool bEnabled)
{
	if (bEnabled)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		if (APlayerController* MyPlayerController = Cast<APlayerController>(GetController()))
		{
			EnableInput(MyPlayerController);
		}
		GetMesh()->SetSimulatePhysics(false);
	}
	else
	{
		GetCharacterMovement()->DisableMovement();
		if (APlayerController* MyPlayerController = Cast<APlayerController>(GetController()))
		{
			DisableInput(MyPlayerController);
		}
		GetMesh()->SetSimulatePhysics(true);
	}
}
