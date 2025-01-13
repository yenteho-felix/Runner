// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerTileManager.h"

void URunnerTileManager::ExtendTile()
{
	FTransform TransformPoint = FTransform();
	AddTile(TransformPoint);

	if (TileActorArray.Num() > TilesAheadPlayer + TilesBehindPlayer)
	{
		RemoveTile();
	}
}

void URunnerTileManager::InitiateTile()
{
	FTransform AttachPoint = FTransform();

	for(int32 i=0; i < TilesAheadPlayer; i++)
	{
		AddTile(AttachPoint);

		// Update AttachPoint for the next Floor
		if (TileClass)
		{
			FVector NewLocation = AttachPoint.GetLocation() + NextAttachPoint;
			AttachPoint.SetLocation(NewLocation);
			//AttachPoint = FloorActorClass->GetAttachPoint();
		}
	}
}

void URunnerTileManager::AddTile(const FTransform& AttachPoint)
{
	if (TileClass)
	{
		if (GetWorld())
		{
			if (AActor* NewFloorActor = GetWorld()->SpawnActor<AActor>(TileClass, AttachPoint))
			{
				TileActorArray.Add(NewFloorActor);
			}		
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FloorActorClass is NULL"));
	}
}

void URunnerTileManager::RemoveTile()
{
	if (TileActorArray.Num() > 0)
	{
		const int32 Index = 0;
		AActor* FloorActor = TileActorArray[Index];
		FloorActor->Destroy();
		TileActorArray.RemoveAt(Index);
	}
}
