// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunnerCharacter.h"

#include "RunnerGameInstance.h"
#include "RunnerGameMode.h"
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

	// Make player move forward infinitely
	AddMovementInput(GetActorForwardVector());

	// Update the lane switch timeline, which controls the player's movement between lanes over time
	if (LaneSwitchTimeline.IsPlaying())
	{
		LaneSwitchTimeline.TickTimeline(DeltaTime);
	}
}

void ARunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get game mode and game instance
	MyGameMode = Cast<ARunnerGameMode>(GetWorld()->GetAuthGameMode());
	MyGameInstance = Cast<URunnerGameInstance>(GetGameInstance());

	// Add game play widget
	if (GamePlayWidgetClass)
	{
		AddWidgetToViewPort(GamePlayWidgetClass, false);
	}

	// Spawn character
	SpawnSelectedCharacter();

	// Initialize the land switch timeline
	InitTimelineForLaneSwitch();
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
		SlidingDuration,
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

void ARunnerCharacter::SwitchLane(int32 LaneOffset)
{
	// Ignore lane switch when player is at the edge
	int32 NewIndex = FMath::Clamp(LaneIndex + LaneOffset, 0, LaneYOffsets.Num()-1);
	if (NewIndex == LaneIndex)
	{
		return;
	}
	
	bIsSwitchingLane = true;

	// Calculate location
	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = FVector(GetActorLocation().X , LaneYOffsets[NewIndex], GetActorLocation().Z);
	LaneSwitchData = FLaneSwitchData(CurrentLocation, TargetLocation, NewIndex);

	// Execute timeline for lane switch
	if (LaneSwitchCurve)
	{
		LaneSwitchTimeline.SetPlayRate(1.0f / LaneSwitchDuration);
		LaneSwitchTimeline.PlayFromStart();
	}
}

void ARunnerCharacter::InitTimelineForLaneSwitch()
{
	if (LaneSwitchCurve)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("OnLaneSwitchProgress"));
		LaneSwitchTimeline.AddInterpFloat(LaneSwitchCurve, TimelineProgress);

		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindUFunction(this, FName("OnLaneSwitchFinished"));
		LaneSwitchTimeline.SetTimelineFinishedFunc(TimelineFinished);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LaneSwitchCurve is NULL"))
	}
}

void ARunnerCharacter::OnLaneSwitchProgress(float Value)
{
	FVector NewLocation = FMath::Lerp(LaneSwitchData.StartLocation, LaneSwitchData.EndLocation, Value);
	SetActorLocation(NewLocation);
}

void ARunnerCharacter::OnLaneSwitchFinished()
{
	bIsSwitchingLane = false;
	LaneIndex = LaneSwitchData.NewLaneIndex;
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
	MyGameMode->RunnerScoreManager->SaveTotalCoin();
	MyGameMode->RunnerScoreManager->SaveHighScore();
	
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

void ARunnerCharacter::SpawnObstacleDestroyer(const FTransform& InTransform) const
{
	// Use StaticLoadClass to dynamically load the blueprint class
	UClass* ObstacleDestroyerBP = StaticLoadClass(
		AActor::StaticClass(),
		nullptr,
		TEXT("/Game/Runner/Blueprints/Actors/BP_ObstacleDestroyer.BP_ObstacleDestroyer_C")
	);

	if (ObstacleDestroyerBP != nullptr)
	{
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ObstacleDestroyerBP, InTransform);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot find SpawnObstacleDestroyerBP"));
	}
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
			MyPlayerController->SetInputMode(FInputModeGameOnly());
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

void ARunnerCharacter::MagnetPowerupStart()
{
	bIsMagnetActive = true;

	MagnetTimer = MagnetDuration;

	// Broadcast the delegate
	OnMagnetPowerupStart.Broadcast();
	
	// Start a timer to call update timer function
	float TimerInterval = 0.5f;
	GetWorld()->GetTimerManager().SetTimer(
	TimerHandle_Magnet,
	 FTimerDelegate::CreateUObject(this, &ARunnerCharacter::UpdateMagnetTimer, TimerInterval),
	 TimerInterval,
	 true 
	);
}

void ARunnerCharacter::UpdateMagnetTimer(float DeltaTime)
{
	if (MagnetTimer > 0)
	{
		MagnetTimer -= DeltaTime;

		if (MagnetTimer <= 0)
		{
			MagnetTimer = 0;
			MagnetPowerupEnd();
		}
	}
}

void ARunnerCharacter::MagnetPowerupEnd()
{
	bIsMagnetActive = false;

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Magnet);

	// Broadcast the delegate
	OnMagnetPowerupEnd.Broadcast();
}

void ARunnerCharacter::SpawnSelectedCharacter()
{
	if (!MyGameInstance || !MyGameInstance->PlayerCharacterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyGameInstance or MyGameInstance->PlayerCharacterClass is NULL"));
		return;
	}

	if (APlayerController* MyPlayerController = Cast<APlayerController>(GetController()))
	{
		// Get the current location and rotation of the character
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();

		// Destroy the current character
		this->Destroy();

		// Spawn the selected character class
		ARunnerCharacter* NewCharacter = GetWorld()->SpawnActor<ARunnerCharacter>(MyGameInstance->PlayerCharacterClass, SpawnLocation, SpawnRotation);
		if (NewCharacter)
		{
			MyPlayerController->Possess(NewCharacter);

			MyPlayerController->SetInputMode(FInputModeGameOnly());
		}
	}
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

	// remove obstacles at spawned location
	SpawnObstacleDestroyer(GetActorTransform());
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
