// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerSkylineActor.h"

#include "RunnerCharacter.h"
#include "RunnerGameMode.h"
#include "RunnerSpawnObjectsComponent.h"
#include "RunnerTileManager.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ARunnerSkylineActor::ARunnerSkylineActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;
	
	LeftSkyline = CreateDefaultSubobject<UChildActorComponent>("LeftSkyline");
	LeftSkyline->SetupAttachment(Scene);
	
	RightSkyline = CreateDefaultSubobject<UChildActorComponent>("RightSkyline");
	RightSkyline->SetupAttachment(Scene);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(Scene);

	AttachpointArrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	AttachpointArrow->SetupAttachment(Scene);

	// Bind the OnComponentBeginOverlap event
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ARunnerSkylineActor::OnBoxCollisionBeginOverlap);

	// Create and assign default value for buildings on the left
	LeftSpawner = CreateDefaultSubobject<URunnerSpawnObjectsComponent>(TEXT("LeftSpawner"));
	LeftSpawner->SpawnSettings.ActorNum = 3;
	LeftSpawner->SpawnSettings.PointsPerLane = 3;
	LeftSpawner->SpawnSettings.LaneYOffsets = {0};
	LeftSpawner->SpawnSettings.XOffset = 50;
	LeftSpawner->SpawnSettings.ZOffset = 0;
	
	// Create and assign default value for buildings on the right
	RightSpawner = CreateDefaultSubobject<URunnerSpawnObjectsComponent>(TEXT("RightSpawner"));
	RightSpawner->SpawnSettings.ActorNum = 3;
	RightSpawner->SpawnSettings.PointsPerLane = 3;
	RightSpawner->SpawnSettings.LaneYOffsets = {0};
	RightSpawner->SpawnSettings.XOffset = 50;
	RightSpawner->SpawnSettings.ZOffset = 0;
	RightSpawner->SpawnSettings.ActorRotator = FRotator(0, 180, 0);
}

void ARunnerSkylineActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SpawnAllObjects();
}

void ARunnerSkylineActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARunnerSkylineActor::OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
												   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleBoxCollision_Implementation(OtherActor);
}

FVector ARunnerSkylineActor::GetAttachLocation_Implementation() const
{
	if (AttachpointArrow)
	{
		return AttachpointArrow->GetComponentTransform().GetLocation();
	}
	return FVector::ZeroVector;
}

void ARunnerSkylineActor::HandleBoxCollision_Implementation(AActor* OverlappingActor)
{
	// Check if the overlapping actor is the player
	ARunnerCharacter* MyCharacter = Cast<ARunnerCharacter>(OverlappingActor);
	if (!MyCharacter)
	{
		return;
	}
	
	// Extend Floor
	ARunnerGameMode* MyGameMode = Cast<ARunnerGameMode>(GetWorld()->GetAuthGameMode());
	if (MyGameMode && MyGameMode->RunnerSkylineManager)
	{
		MyGameMode->RunnerSkylineManager->ExtendTile();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode or RunnerSkylineManager is NULL!"));
	}
}

void ARunnerSkylineActor::SpawnAllObjects()
{
	if (LeftSkyline)
	{
		LeftSpawner->SpawnObjects(LeftSkyline);
	}
	if (RightSkyline)
	{
		RightSpawner->SpawnObjects(RightSkyline);
	}
}

