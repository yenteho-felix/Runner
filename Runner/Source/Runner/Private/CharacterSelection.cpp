// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelection.h"
#include "RunnerCharacter.h"
#include "RunnerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
ACharacterSelection::ACharacterSelection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	SkeletalMesh->SetupAttachment(Scene);
}

// Called when the game starts or when spawned
void ACharacterSelection::BeginPlay()
{
	Super::BeginPlay();
	
	// Set Default Character from array index 0
	if (CharacterMeshArray.Num() > 0)
	{
		SkeletalMesh->SetSkeletalMesh(CharacterMeshArray[CurrentIndex]);
		SaveSelectedCharacter();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterMeshArray is NULL"));
	}
}

void ACharacterSelection::PrevCharacter()
{
	CurrentIndex = (CurrentIndex - 1 + CharacterMeshArray.Num()) % CharacterMeshArray.Num();
	SkeletalMesh->SetSkeletalMesh(CharacterMeshArray[CurrentIndex]);
}

void ACharacterSelection::NextCharacter()
{
	CurrentIndex = (CurrentIndex + 1) % CharacterMeshArray.Num();
	SkeletalMesh->SetSkeletalMesh(CharacterMeshArray[CurrentIndex]);
}

void ACharacterSelection::SaveSelectedCharacter()
{
	if (!MyGameInstance)
	{
		// Get GameInstance
		MyGameInstance = Cast<URunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (!MyGameInstance)
		{
			UE_LOG(LogTemp, Error, TEXT("MyGameInstance is NULL"));
		}
	}
	MyGameInstance->PlayerCharacterClass = CharacterPawnArray[CurrentIndex];
}

