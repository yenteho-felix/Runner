// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerSpawnObjectsComponent.h"
#include "Algo/RandomShuffle.h"
#include "Components/ArrowComponent.h"
#include "CollisionQueryParams.h"
#include "PropertyAccess.h"

URunnerSpawnObjectsComponent::URunnerSpawnObjectsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URunnerSpawnObjectsComponent::SpawnObjects(UChildActorComponent* AttachParent)
{
    //UE_LOG(LogTemp, Display, TEXT("URunnerSpawnObjectsComponent::SpawnObjects"));
    
    if (!SpawnSettings.bEnabled)
    {
        return;
    }

    if (SpawnSettings.ActorClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No actor class specified"));
        return;
    }
    
    // Remove existing objects first
    RemoveObjects();

    // Generate spawn points
    TArray<FTransform> SpawnTransforms = GenerateSpawnTransform(AttachParent);

    // Visualize the spawn points
    VisualizeSpawnLocations(SpawnTransforms, AttachParent);

    // Randomize the order of spawn points
    Algo::RandomShuffle(SpawnTransforms);

    // Spawn the actual objects
    for (int32 i = 0; i < SpawnSettings.ActorNum; ++i)
    {
        // Pick a random class form the array
        TSubclassOf<AActor> ActorClass = SpawnSettings.ActorClasses[FMath::RandRange(0, SpawnSettings.ActorClasses.Num() - 1)];

        SpawnObjectClass(ActorClass, SpawnTransforms[i], AttachParent);
    }

    // Remove spawned that overlapped with existing objects
    //ResolveOverlaps();
}

void URunnerSpawnObjectsComponent::RemoveObjects()
{
    // Remove all spawned objects
    for (UChildActorComponent* Object : SpawnedObjects)
    {
        if (Object)
        {
            Object->DestroyComponent();
        }
    }
    SpawnedObjects.Empty();
}

void URunnerSpawnObjectsComponent::SpawnObjectClass(UClass* ActorClass, const FTransform& SpawnTransform, UChildActorComponent* AttachParent)
{
    if (ActorClass)
    {
        UChildActorComponent* NewChildActor = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass());
        NewChildActor->SetChildActorClass(ActorClass);
        NewChildActor->AttachToComponent(AttachParent, FAttachmentTransformRules::KeepRelativeTransform);
        NewChildActor->SetRelativeTransform(SpawnTransform);
        NewChildActor->RegisterComponent();
        SpawnedObjects.Add(NewChildActor);

        //UE_LOG(LogTemp, Display, TEXT("Spawned object : %s at location %s"), *NewChildActor->GetName(), *NewChildActor->GetRelativeLocation().ToString());
    }
}

TArray<FTransform> URunnerSpawnObjectsComponent::GenerateSpawnTransform(const UChildActorComponent* AttachParent) const
{
    // Get the scaled floor extent
    const FVector FloorExtent = CalculateFloorExtent(AttachParent);
    //UE_LOG(LogTemp, Display, TEXT("FloorExtent is %s"), *FloorExtent.ToString());

    // Calculate spacing dynamically based on AttachParent's size
    const float FloorWidth = FloorExtent.X * 2; 
    const float UsableWidth = FloorWidth - (2 * SpawnSettings.XOffset);
    const float SpacingX = UsableWidth / (SpawnSettings.PointsPerLane + 1);

    // Iterate over the lanes and points
    TArray<FTransform> SpawnTransforms;
    for (int32 Point = 0; Point < SpawnSettings.PointsPerLane; ++Point)
    {
        for (int32 i = 0; i < SpawnSettings.LaneYOffsets.Num(); i++)
        {
            // Calculate the spawn location for the current lane and point
            FVector Location(SpawnSettings.XOffset + (SpacingX * (Point + 1)- FloorExtent.X), SpawnSettings.LaneYOffsets[i], SpawnSettings.ZOffset);
            FRotator Rotation = SpawnSettings.ActorRotator;

            SpawnTransforms.Add(FTransform(Rotation, Location));
            //UE_LOG(LogTemp, Display, TEXT("Spawn Transform %s"), *Location.ToString());
        }
    }

    return SpawnTransforms;
}

FVector URunnerSpawnObjectsComponent::CalculateFloorExtent(const UChildActorComponent* AttachParent) const
{
    FVector FloorExtent = FVector::ZeroVector;

    if (IsValid(AttachParent) && IsValid(AttachParent->GetChildActor()))
    {
        // Get the child actor
        AActor* ChildActor = AttachParent->GetChildActor();
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

void URunnerSpawnObjectsComponent::ResolveOverlaps()
{
    UE_LOG(LogTemp, Display, TEXT("Spawned Object Count is %i"), SpawnedObjects.Num());
    for (TObjectPtr<UChildActorComponent> Object : SpawnedObjects)
    {
        UE_LOG(LogTemp, Display, TEXT("Object %s"), *Object->GetChildActor()->GetName());
    }
    
    for (int32 i = SpawnedObjects.Num() - 1; i >= 0; --i)
    {
        UChildActorComponent* Object = SpawnedObjects[i];
        if (AActor* SpawnedActor = Object->GetChildActor())
        {
            UE_LOG(LogTemp, Display, TEXT("Checking object %s at location %s"), *SpawnedActor->GetName(), *SpawnedActor->GetActorLocation().ToString());
            // Perform overlap check
            //TArray<FOverlapResult> Overlaps;
            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(SpawnedActor);

            bool bOverlapping = GetWorld()->OverlapBlockingTestByChannel(
                SpawnedActor->GetActorLocation(),
                SpawnedActor->GetActorRotation().Quaternion(), 
                ECC_WorldStatic, 
                FCollisionShape::MakeSphere(1.0f), 
                QueryParams
            );

            // bool bOverlapping = GetWorld()->OverlapMultiByChannel(
            //     Overlaps,
            //     SpawnedActor->GetActorLocation(),
            //     FQuat::Identity,
            //     ECC_WorldDynamic, // Adjust channel as needed
            //     FCollisionShape::MakeSphere(50.0f), // Adjust shape and size as needed
            //     QueryParams
            // );

            // Delete spawned object 
            if (bOverlapping)
            {
                UE_LOG(LogTemp, Display, TEXT("SpawnedActor %s overlapped with others, delete it"), *SpawnedActor->GetName());
                SpawnedObjects.RemoveAt(i);
                Object->DestroyComponent();
            }           
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Object has no ChildActor"))
        }

    }
}

void URunnerSpawnObjectsComponent::AddArrowComponent(const FVector& Location, const FColor& Color, UChildActorComponent* AttachParent)
{
    UArrowComponent* ArrowComponent = NewObject<UArrowComponent>(this);
    ArrowComponent->ArrowColor = Color;
    ArrowComponent->AttachToComponent(AttachParent, FAttachmentTransformRules::KeepRelativeTransform);
    ArrowComponent->SetRelativeLocation(Location);
    ArrowComponent->RegisterComponent();
    SpawnedArrows.Add(ArrowComponent);
}

void URunnerSpawnObjectsComponent::VisualizeSpawnLocations(const TArray<FTransform>& SpawnTransforms, UChildActorComponent* AttachParent)
{
    RemoveArrowComponents();
    
    // Visualize the spawn locations using arrows
    for (const FTransform& Transform : SpawnTransforms)
    {
        AddArrowComponent(Transform.GetLocation(), ArrowColor, AttachParent);
    }
}

void URunnerSpawnObjectsComponent::RemoveArrowComponents()
{
    // Remove all arrows used for visualization
    for (UArrowComponent* Arrow : SpawnedArrows)
    {
        if (Arrow)
        {
            Arrow->DestroyComponent();
        }
    }
    SpawnedArrows.Empty();
}

