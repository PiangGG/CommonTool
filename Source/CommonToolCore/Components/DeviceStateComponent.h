// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DeviceStateComponent.generated.h"


class UStateSubsystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMMONTOOLCORE_API UDeviceStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDeviceStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnSystemStateChange(FGameplayTag newState);
	UFUNCTION()
	void OnOperationStateChange(FGameplayTag newState);
public:
	UPROPERTY(EditAnywhere,Category="CommonToolCore|Device")
	bool bChangeVisibleState = false;
	UPROPERTY(EditAnywhere,Category="CommonToolCore|Device")
	TSoftObjectPtr<UMaterialInterface> TargetMaterial;
private:
	UPROPERTY()
	UStateSubsystem* StateSubsystem;

	//UPROPERTY()
	TMap<TObjectPtr<UStaticMeshComponent>,TArray<TObjectPtr<UMaterialInterface>>> MeshComponentMaterials;
	
	UPROPERTY()
	FGameplayTag LastState;
	UPROPERTY()
	UMaterialInterface* CurrentMaterial;
};

