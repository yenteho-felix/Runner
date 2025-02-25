// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerTileManager.h"
#include "UObject/Interface.h"
#include "RunnerCollisionInterface.h"

void URunnerTileManager::ExtendTile()
{
	AddTile();
	
	if (TileActorArray.Num() > TilesAheadPlayer + TilesBehindPlayer)
	{
		RemoveTile();
	}
}

void URunnerTileManager::InitiateTile()
{
	TileAttachLocation = FirstTileLocation;
	
	for(int32 i=0; i < TilesAheadPlayer; i++)
	{
		AddTile();
	}
}

void URunnerTileManager::AddTile()
{
	if (TileClass)
	{
		if (TileClass->ImplementsInterface(URunnerCollisionInterface::StaticClass()))
		{
			const FTransform TileAttachTransform(FRotator::ZeroRotator, TileAttachLocation);
		
			if (AActor* NewFloorActor = GetWorld()->SpawnActor<AActor>(TileClass, TileAttachTransform))
			{
				//UE_LOG(LogTemp, Display, TEXT("Adding tile to world at position: %s"), *TileAttachLocation.ToString());
				TileActorArray.Add(NewFloorActor);
				TileCount++;
				
				IRunnerCollisionInterface* I = Cast<IRunnerCollisionInterface>(NewFloorActor);
				if (I)
				{
					TileAttachLocation = I->Execute_GetAttachLocation(NewFloorActor);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s does not implement URunnerCollisionInterface"), *TileClass->GetName());
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
