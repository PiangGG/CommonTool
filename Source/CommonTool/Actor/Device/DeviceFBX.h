// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "DeviceFBX.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOL_API ADeviceFBX : public ABaseDevice
{
	GENERATED_BODY()
	
	ADeviceFBX();
	
public:
	// virtual void BeginPlay() override;
	//
	// virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//
	// UPROPERTY(EditAnywhere, Category=Device)
	// FString Path;
	//
	// virtual void SetDeviceData_Implementation(const FString& DeviceData) override;
	//
	// virtual TSharedRef<FJsonObject> GetDeviceJsonObjectData() override;
	//
	// virtual void SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject) override;
	//
	// UFUNCTION()
	// void OnActorInitializationCompleted(UObject* Object);
	//
	// virtual void SetDeviceLoad_Implementation(const FString& bload) override;
	//
	// virtual void ReDraw() override;
private:
	// UPROPERTY()
	// class ACommonDataSmithRuntimeActor* Actor;
};
