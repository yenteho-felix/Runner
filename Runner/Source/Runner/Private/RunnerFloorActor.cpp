// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerFloorActor.h"

#include "RunnerCharacter.h"
#include "RunnerGameMode.h"
#include "RunnerTileManager.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Algo/RandomShuffle.h"
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
}

void ARunnerFloorActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UE_LOG(LogTemp, Display, TEXT("ARunnerFloorActor::OnConstruction"));
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

// Called when the game starts or when spawned
void ARunnerFloorActor::BeginPlay()
{
	Super::BeginPlay();

	// UE_LOG(LogTemp, Warning, TEXT("ARunnerFloorActor::BeginPlay"));
	// SpawnChildActors();
	RemoveArrowComponent();
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

	// Increase Speed
	IncreaseSpeed(OverlappingActor, SpeedIncrement, MaxSpeed);
	
	// Update Score
}

void ARunnerFloorActor::SpawnAllObjects()
{
	// Clean up the components created previously
	RemoveArrowComponent();
	RemoveAllObjects();
	
	// Constructor call this function multiple times so we need to make sure it only go through when everything is valid
	if (IsValid(FloorComponent) && IsValid(FloorComponent->GetChildActor()))
	{
		UE_LOG(LogTemp, Display, TEXT("Spawn ChildActors"));
		
		// Conditionally Spawn Moving Obstacles
		bSpawnMovingObstacles = ShouldSpawnObjects(MovingObstaclesSpawnIntervalBase, MovingObstaclesSpawnIntervalRandomOffset);

		TArray<FTransform> MovingObstaclesSpwanTransforms = GenerateSpawnTransforms(LaneOffsets, MovingObstaclesSpawnPointsPerLane, MovingObstaclesXOffset, MovingObstaclesZOffset, FColor::Orange);
		if (bSpawnMovingObstacles && MovingObstaclesSpwanTransforms.Num() > 0)
		{
			SpawnChildActorComponents(MovingObstaclesActorClasses, MovingObstaclesNum, MovingObstaclesRotator, MovingObstaclesSpwanTransforms, Scene);
		}

		// Always Spawn Obstacles 
		TArray<FTransform> ObstacleSpwanTransforms = GenerateSpawnTransforms(LaneOffsets, ObstacleSpawnPointsPerLane, ObstacleXOffset, ObstacleZOffset, FColor::Yellow);
		if (bSpawnObstacle && ObstacleSpwanTransforms.Num() > 0)
		{
			SpawnChildActorComponents(ObstacleActorClasses, ObstacleNum, ObstacleRotator, ObstacleSpwanTransforms, Scene);
		}

		// Conditionally Spawn Power 
		bSpawnPowerup = ShouldSpawnObjects(PowerupSpawnIntervalBase, PowerupSpawnIntervalRandomOffset);
		
		TArray<FTransform> PowerupSpawnTransforms = GenerateSpawnTransforms(LaneOffsets, PowerupSpawnPointsPerLane, PowerupXOffset, PowerupZOffset, FColor::Green);
		if (bSpawnPowerup && PowerupSpawnTransforms.Num() > 0)
		{
			SpawnChildActorComponents(PowerupActorClasses, PowerupNum, PowerupRotator,PowerupSpawnTransforms, Scene);
		}

		// Always Spawn Coin
		TArray<FTransform> CoinSpawnTransforms = GenerateSpawnTransforms(LaneOffsets, CoinSpawnPointsPerLane, CoinXOffset, CoinZOffset, FColor::White);
		if (bSpawnCoin && CoinSpawnTransforms.Num() > 0)
		{
			SpawnChildActorComponents(CoinActorClasses, CoinNum, CoinRotator, CoinSpawnTransforms, Scene);
		}
	}
}

TArray<FTransform>& ARunnerFloorActor::GenerateSpawnTransforms(const TArray<float>& LaneOffsetsY, int32 NumPointsPerLane, int32 XOffset, int32 ZOffset, const FColor& ArrowColor)
{
	// Static array to store generated transforms
	static TArray<FTransform> SpawnTransforms;
	SpawnTransforms.Empty();

	// Constructor call this function multiple times so we need to make sure it only go through when everything is valid
	if (!IsValid(FloorComponent) || !IsValid(FloorComponent->GetChildActor()))
	{
		return SpawnTransforms;
	}
	
	// Get the scaled floor extent
	const FVector FloorExtent = CalculateFloorExtent(FloorComponent);

	// Calculate spacing dynamically based on FloorComponent size
	const float FloorWidth = FloorExtent.X * 2; 
	const float UsableWidth = FloorWidth - (2 * XOffset);
	const float SpacingX = UsableWidth / (NumPointsPerLane + 1);

	// Iterate over the lanes and points
	for (int32 PointIndex = 0; PointIndex < NumPointsPerLane; ++PointIndex)
	{
		for (float LaneOffsetY : LaneOffsetsY)
		{
			// Calculate the spawn location for the current lane and point
			FVector SpawnLocation(XOffset + (SpacingX * (PointIndex + 1)), LaneOffsetY, ZOffset);

			// Add an arrow for visualization and store its transform
			FTransform ArrowTransform = AddArrowComponent(ArrowColor, SpawnLocation, Scene);

			//UE_LOG(LogTemp, Log, TEXT("Spawning Arrow Location %s"), *ArrowTransform.GetLocation().ToString());
			SpawnTransforms.Add(ArrowTransform);
		}
	}

	return SpawnTransforms;
}

void ARunnerFloorActor::SpawnChildActorComponents(const TArray<UClass*> ActorClasses, int32 ActorNum, FRotator ActorRotator, TArray<FTransform>& SpawnTransforms, USceneComponent* AttachParent)
{
	// Check if array is valid
	if (ActorClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No child actor class specified"));
		return;
	}

	// Check if Transform points is greater than Object count
	if (ActorNum > SpawnTransforms.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Available spawn location is smaller than objects we like to spawn, reduce the object count "));
		ActorNum = SpawnTransforms.Num();
	}

	// Randomize the order of elements
	Algo::RandomShuffle(SpawnTransforms);

	// Loop to add new Object
	for (int32 i = 0; i < ActorNum; ++i)
	{
		// Pick a random class form the array
		TSubclassOf<AActor> ActorToSpwan = ActorClasses[FMath::RandRange(0, ActorClasses.Num() - 1)];

		// Create a new ChildActorComponent
		TObjectPtr<UChildActorComponent> NewChildActorComponent = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass());
		if (NewChildActorComponent)
		{
			NewChildActorComponent->SetChildActorClass(ActorToSpwan);
			NewChildActorComponent->SetRelativeTransform(SpawnTransforms[i]);
			NewChildActorComponent->SetRelativeRotation(ActorRotator);
			NewChildActorComponent->SetupAttachment(AttachParent);
			NewChildActorComponent->RegisterComponent();
			//UE_LOG(LogTemp, Display, TEXT("Spawned object %d : %s at location %s"), i, *NewChildActorComponent->GetName(), *NewChildActorComponent->GetRelativeLocation().ToString());

			CreatedChildActorComponents.Add(NewChildActorComponent);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Fail to create Child Actor Component."))
		}
	}
}

void ARunnerFloorActor::RemoveAllObjects()
{
	for (UChildActorComponent* ChildActorComponent : CreatedChildActorComponents)
	{
		if (IsValid(ChildActorComponent))
		{
			ChildActorComponent->DestroyComponent();
		}
	}
	CreatedChildActorComponents.Empty();
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

FTransform ARunnerFloorActor::AddArrowComponent(const FColor Color, const FVector& Location,
                                                USceneComponent* AttachParent)
{
	// Create a new arrow component
	UArrowComponent* NewArrow = NewObject<UArrowComponent>(this, UArrowComponent::StaticClass());
	if (NewArrow)
	{
		// Attach to the FloorComponent
		NewArrow->SetupAttachment(AttachParent);
		NewArrow->ArrowColor = Color;
		NewArrow->SetRelativeLocation(Location);
		NewArrow->RegisterComponent();
		
		CreatedArrowComponents.Add(NewArrow);

		return NewArrow->GetRelativeTransform();
	}

	return FTransform::Identity;
}

void ARunnerFloorActor::RemoveArrowComponent()
{
	for (UArrowComponent* ArrowComponent : CreatedArrowComponents)
	{
		if (IsValid(ArrowComponent))
		{
			ArrowComponent->DestroyComponent();
		}
	}
	CreatedArrowComponents.Empty();
}

FVector ARunnerFloorActor::CalculateFloorExtent(const UChildActorComponent* ChildActorComponent) const
{
	FVector FloorExtent = FVector::ZeroVector;

	if (IsValid(ChildActorComponent) && IsValid(ChildActorComponent->GetChildActor()))
	{
		// Get the child actor
		AActor* ChildActor = ChildActorComponent->GetChildActor();
		if (IsValid(ChildActor))
		{
			// Now access the StaticMeshComponent of the child actor
			UStaticMeshComponent* StaticMeshComp = ChildActor->FindComponentByClass<UStaticMeshComponent>();
			if (IsValid(StaticMeshComp))
			{
				const FVector MeshExtent = StaticMeshComp->GetStaticMesh()->GetBoundingBox().GetExtent();
				const FVector ComponentScale = StaticMeshComp->GetComponentScale(); 
				FloorExtent = MeshExtent * ComponentScale;
			}
		}
	}

	//UE_LOG(LogTemp, Display, TEXT("FloorExtend: %s"), *FloorExtent.ToString());
	return FloorExtent;
}

void ARunnerFloorActor::IncreaseSpeed(AActor* Actor, float Increment, float Max)
{
	ARunnerCharacter* MyCharacter = Cast<ARunnerCharacter>(Actor);
	if (!MyCharacter)
	{
		return;
	}
	
	if (MyCharacter->GetCharacterMovement())
	{
		float NewSpeed = MyCharacter->GetCharacterMovement()->MaxWalkSpeed + Increment;
		
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = FMath::Min(NewSpeed, Max);
	}
}
