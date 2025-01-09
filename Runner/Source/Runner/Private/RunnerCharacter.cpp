// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunnerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ARunnerCharacter

ARunnerCharacter::ARunnerCharacter()
{
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
	GetCharacterMovement()->JumpZVelocity = 700.f;
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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ARunnerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ARunnerCharacter::StartSlide()
{
	bIsSliding = true;

	// Disable input
	APlayerController* MyPlayerController = Cast<APlayerController>(GetController());
	if (MyPlayerController)
	{
		MyPlayerController->DisableInput(MyPlayerController);
	}

	// Disable collision
	UCapsuleComponent* MyCapsuleComponent = GetCapsuleComponent();
	if (!MyCapsuleComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCapsuleComponent() returned nullptr!"));
	}
	MyCapsuleComponent->SetCollisionResponseToChannel(ECC_Destructible, ECollisionResponse::ECR_Ignore);

	// Use Timer to end sliding
	GetWorld()->GetTimerManager().SetTimer(SlidingTimerHandle, this, &ARunnerCharacter::EndSlide, AnimationLength, false);
}

void ARunnerCharacter::EndSlide()
{
	bIsSliding = false;

	// Enable input
	APlayerController* MyPlayerController = Cast<APlayerController>(GetController());
	if (MyPlayerController)
	{
		MyPlayerController->EnableInput(MyPlayerController);
	}

	// Restore collision response after the animation
	UCapsuleComponent* MyCapsuleComponent = GetCapsuleComponent();
	if (!MyCapsuleComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCapsuleComponent() returned nullptr!"));
	}
	MyCapsuleComponent->SetCollisionResponseToChannel(ECC_Destructible, ECollisionResponse::ECR_Block);
}

void ARunnerCharacter::PlaySlideMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetAnimInstance() returned nullptr!"));
		return;
	}

	UCapsuleComponent* MyCapsuleComponent = GetCapsuleComponent();
	if (!MyCapsuleComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCapsuleComponent() returned nullptr!"));
	}

	if (SlideMontage)
	{
		// Get velocity and check if speed > 60
		FVector Velocity = GetCharacterMovement()->Velocity;
		float Speed = Velocity.Size();
		if (Speed < 60.0f)
		{
			return;
		}

		bIsSliding = true;

		// Disable collision
		MyCapsuleComponent->SetCollisionResponseToChannel(ECC_Destructible, ECollisionResponse::ECR_Ignore);

		// Disable input
		APlayerController* MyPlayerController = Cast<APlayerController>(GetController());
		if (MyPlayerController)
		{
			MyPlayerController->DisableInput(MyPlayerController);
		}

		// Play animation
		AnimInstance->Montage_Play(SlideMontage, 1.5f);

		FOnMontageEnded MontageEndedDelegate;
		if (!MontageEndedDelegate.IsBound())
		{
			MontageEndedDelegate.BindUObject(this, &ARunnerCharacter::OnSlideMontageEnded);
		}
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, SlideMontage);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SlideMontage is not assigned!"));
		return;
	}
}

void ARunnerCharacter::OnSlideMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UCapsuleComponent* MyCapsuleComponent = GetCapsuleComponent();
	if (!MyCapsuleComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCapsuleComponent() returned nullptr!"));
	}

	// Enable input
	APlayerController* MyPlayerController = Cast<APlayerController>(GetController());
	if (MyPlayerController)
	{
		MyPlayerController->EnableInput(MyPlayerController);
	}

	// Restore collision response after the animation
	MyCapsuleComponent->SetCollisionResponseToChannel(ECC_Destructible, ECollisionResponse::ECR_Block);

	bIsSliding = false;
}