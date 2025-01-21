// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterSelection.generated.h"

class ARunnerCharacter;
class URunnerGameInstance;

UCLASS()
class RUNNER_API ACharacterSelection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterSelection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

/*
 * Default Scene Components
 */
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Scene;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

/*
 * Character Selection
 */
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default|Character Selection")
	TArray<UClass*> CharacterPawnArray;

	// Array of Skeletal Meshes
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default|Character Selection")
	TArray<USkeletalMesh*> CharacterMeshArray;

	UFUNCTION(BlueprintCallable)
	void PrevCharacter();

	UFUNCTION(BlueprintCallable)
	void NextCharacter();
	
	UFUNCTION(BlueprintCallable)
	void SaveSelectedCharacter();

protected:
	int32 CurrentIndex = 0;

	UPROPERTY()
	TObjectPtr<URunnerGameInstance> MyGameInstance;
};
