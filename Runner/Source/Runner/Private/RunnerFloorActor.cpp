// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerFloorActor.h"

#include "RunnerCharacter.h"
#include "RunnerGameMode.h"
#include "RunnerScoreManager.h"
#include "RunnerSpawnObjectsComponent.h"
#include "RunnerTileManager.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ARunnerFloorActor::ARunnerFloorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;
	
	FloorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("FloorComponent"));
	FloorComponent->SetupAttachment(Scene);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(Scene);

	AttachpointArrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	AttachpointArrow->SetupAttachment(Scene);
	
	// Bind the OnComponentBeginOverlap event
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ARunnerFloorActor::OnBoxCollisionBeginOverlap);
	
	// Create and assign default value for Coin
	CoinSpawner = CreateDefaultSubobject<URunnerSpawnObjectsComponent>(TEXT("CoinSpawner"));
	CoinSpawner->SpawnSettings.ActorNum = 10;
	CoinSpawner->SpawnSettings.PointsPerLane = 6;
	CoinSpawner->SpawnSettings.LaneYOffsets = {-325, 0, 325};
	CoinSpawner->SpawnSettings.XOffset = 50;
	CoinSpawner->SpawnSettings.ZOffset = 0;

	// Create and assign default value for Powerup
	PowerupSpawner = CreateDefaultSubobject<URunnerSpawnObjectsComponent>(TEXT("PowerupSpawner"));
	PowerupSpawner->SpawnSettings.ActorNum = 1;
	PowerupSpawner->SpawnSettings.PointsPerLane = 6;
	PowerupSpawner->SpawnSettings.LaneYOffsets = {-325, 0, 325};
	PowerupSpawner->SpawnSettings.XOffset = 50;
	PowerupSpawner->SpawnSettings.ZOffset = 0;
	PowerupSpawner->SpawnSettings.SpawnIntervalBase = 5;
	PowerupSpawner->SpawnSettings.SpawnIntervalRandomOffset = 1;

	// Create and assign default value for Obstacle
	ObstacleSpawner = CreateDefaultSubobject<URunnerSpawnObjectsComponent>(TEXT("ObstacleSpawner"));
	ObstacleSpawner->SpawnSettings.ActorNum = 1;
	ObstacleSpawner->SpawnSettings.PointsPerLane = 2;
	ObstacleSpawner->SpawnSettings.LaneYOffsets = {-325, 0, 325};
	ObstacleSpawner->SpawnSettings.XOffset = 50;
	ObstacleSpawner->SpawnSettings.ZOffset = 0;

	// Create and assign default value for Moving Obstacle
	MovingObstacleSpawner = CreateDefaultSubobject<URunnerSpawnObjectsComponent>(TEXT("MovingObstacleSpawner"));
	MovingObstacleSpawner->SpawnSettings.ActorNum = 1;
	MovingObstacleSpawner->SpawnSettings.PointsPerLane = 2;
	MovingObstacleSpawner->SpawnSettings.LaneYOffsets = {-325, 0, 325};
	MovingObstacleSpawner->SpawnSettings.XOffset = 50;
	MovingObstacleSpawner->SpawnSettings.ZOffset = 0;
	MovingObstacleSpawner->SpawnSettings.ActorRotator = FRotator(0, 180, 0);
	MovingObstacleSpawner->SpawnSettings.SpawnIntervalBase = 8;
	MovingObstacleSpawner->SpawnSettings.SpawnIntervalRandomOffset = 2;
}

void ARunnerFloorActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//UE_LOG(LogTemp, Display, TEXT("ARunnerFloorActor::OnConstruction"));

	SpawnAllObjects();
}

void ARunnerFloorActor::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	if (bFinished)
	{
		// SpawnAllObjects();	
	}
}

void ARunnerFloorActor::BeginPlay()
{
	Super::BeginPlay();
}

void ARunnerFloorActor::OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
												   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleBoxCollision_Implementation(OtherActor);
}

FVector ARunnerFloorActor::GetAttachLocation_Implementation() const
{
	if (AttachpointArrow)
	{
		return AttachpointArrow->GetComponentTransform().GetLocation();
	}
	return FVector::ZeroVector;
}

void ARunnerFloorActor::HandleBoxCollision_Implementation(AActor* OverlappingActor)
{
	// Check if the overlapping actor is the player
	ARunnerCharacter* MyCharacter = Cast<ARunnerCharacter>(OverlappingActor);
	if (!MyCharacter)
	{
		return;
	}
	
	// Extend Floor
	ARunnerGameMode* MyGameMode = Cast<ARunnerGameMode>(GetWorld()->GetAuthGameMode());
	if (MyGameMode && MyGameMode->RunnerFloorManager)
	{
		MyGameMode->RunnerFloorManager->ExtendTile();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode or RunnerFloorManager is NULL!"));
	}

	// Update Speed
	IncreaseSpeed(OverlappingActor, SpeedIncrement, MaxSpeed);
	
	// Update Score
	IncreaseScore(ScoreIncrement);
}

void ARunnerFloorActor::SpawnAllObjects()
{
	if (MovingObstacleSpawner)
	{
		// Moving Obstacles are not spawned on every floor
		int32 SpawnIntervalBase = MovingObstacleSpawner->SpawnSettings.SpawnIntervalBase;
		int32 SpawnIntervalRandomOffset = MovingObstacleSpawner->SpawnSettings.SpawnIntervalRandomOffset;
		if (ShouldSpawnObjects(SpawnIntervalBase, SpawnIntervalRandomOffset))
		{
			MovingObstacleSpawner->SpawnObjects(FloorComponent);
		}
	}
	if (ObstacleSpawner)
	{
		ObstacleSpawner->SpawnObjects(FloorComponent);
	}
	if (PowerupSpawner)
	{
		// Powerups are not spawned on every floor
		int32 SpawnIntervalBase = PowerupSpawner->SpawnSettings.SpawnIntervalBase;
		int32 SpawnIntervalRandomOffset = PowerupSpawner->SpawnSettings.SpawnIntervalRandomOffset;
		if (ShouldSpawnObjects(SpawnIntervalBase, SpawnIntervalRandomOffset))
		{
			PowerupSpawner->SpawnObjects(FloorComponent);
		}
	}
	if (CoinSpawner)
	{
		CoinSpawner->SpawnObjects(FloorComponent);
	}
}

bool ARunnerFloorActor::ShouldSpawnObjects(int32 SpawnIntervalBase, int32 SpawnIntervalRandomOffset) const
{
	ARunnerGameMode* MyGameMode = Cast<ARunnerGameMode>(GetWorld()->GetAuthGameMode());
	if (MyGameMode && MyGameMode->RunnerFloorManager)
	{
		int32 RandomizedInterval = SpawnIntervalBase + FMath::RandRange(-SpawnIntervalRandomOffset, SpawnIntervalRandomOffset);
		return ((MyGameMode->RunnerFloorManager->TileCount % RandomizedInterval) == 0);
	}

	return false;
}

void ARunnerFloorActor::IncreaseSpeed(AActor* Actor, float Increment, float Max)
{
	ARunnerCharacter* MyCharacter = Cast<ARunnerCharacter>(Actor);
	if (MyCharacter && MyCharacter->GetCharacterMovement())
	{
		float NewSpeed = MyCharacter->GetCharacterMovement()->MaxWalkSpeed + Increment;
		
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = FMath::Min(NewSpeed, Max);
	}
}

void ARunnerFloorActor::IncreaseScore(int32 Amount)
{
	ARunnerGameMode* MyGameMode = Cast<ARunnerGameMode>(GetWorld()->GetAuthGameMode());
	if (MyGameMode && MyGameMode->RunnerScoreManager)
	{
		MyGameMode->RunnerScoreManager->AddScore(Amount);
	}
}
