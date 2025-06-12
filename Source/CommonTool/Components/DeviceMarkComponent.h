// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DeviceMarkComponent.generated.h"


UCLASS(Blueprintable,BlueprintType,meta=(BlueprintSpawnableComponent))
class COMMONTOOL_API UDeviceMarkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDeviceMarkComponent();

	UFUNCTION(BlueprintCallable,Category="CommonLibrary|Device")
	FString GetDeviceID();

	UPROPERTY(EditAnywhere,Category="CommonLibrary|Device")
	FString DeviceID;
	//actor属于哪些种类
	UPROPERTY(EditAnywhere,Category="CommonLibrary|Device")
	FGameplayTagContainer ActorMarkTypes;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	

private:
};
