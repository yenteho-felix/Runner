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
	
	LeftGround = CreateDefaultSubobject<UChildActorComponent>("LeftSkyline");
	LeftGround->SetupAttachment(Scene);
	
	RightGround = CreateDefaultSubobject<UChildActorComponent>("RightSkyline");
	RightGround->SetupAttachment(Scene);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(Scene);

	AttachpointArrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	AttachpointArrow->SetupAttachment(Scene);

	// Bind the OnComponentBeginOverlap event
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ARunnerSkylineActor::OnBoxCollisionBeginOverlap);

	// Create and assign default value for buildings on the left
	LeftSpawner1 = CreateDefaultSubobject<URunnerSpawnObjectsComponent>(TEXT("LeftSpawner1"));
	
	// Create and assign default value for trees on the left
	LeftSpawner2 = CreateDefaultSubobject<URunnerSpawnObjectsComponent>(TEXT("LeftSpawner2"));
	
	// Create and assign default value for buildings on the right
	RightSpawner1 = CreateDefaultSubobject<URunnerSpawnObjectsComponent>(TEXT("RightSpawner1"));
	
	// Create and assign default value for trees on the right
	RightSpawner2 = CreateDefaultSubobject<URunnerSpawnObjectsComponent>(TEXT("RightSpawner2"));
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
	if (LeftGround)
	{
		LeftSpawner1->SpawnObjects(LeftGround);
		LeftSpawner2->SpawnObjects(LeftGround);
	}
	if (RightGround)
	{
		RightSpawner1->SpawnObjects(RightGround);
		RightSpawner2->SpawnObjects(RightGround);
	}
}

