// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DeviceMarkComponent.generated.h"


class UInteractiveSubsystem;
class UInspectionManager;
class UDeviceManager;

UCLASS(Blueprintable,BlueprintType,meta=(BlueprintSpawnableComponent))
class COMMONTOOLCORE_API UDeviceMarkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDeviceMarkComponent();

	UFUNCTION(BlueprintCallable,Category="CommonToolCore|Device")
	FString GetDeviceID();

	UPROPERTY(EditAnywhere,Category="CommonToolCore|Device")
	FString DeviceID;
	//actor属于哪些种类
	UPROPERTY(EditAnywhere,Category="CommonToolCore|Device")
	FGameplayTagContainer ActorMarkTypes;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void InspectionCheck(bool bInspection,const FTransform& Transform);
	
	UFUNCTION()
	void InspectionTransform(const FTransform& InTransform);
private:
	UPROPERTY()
	UInteractiveSubsystem* InteractiveSubsystem;
	UPROPERTY()
	UDeviceManager *DeviceManager;
	UPROPERTY()
	UInspectionManager * InspectionManager;
};