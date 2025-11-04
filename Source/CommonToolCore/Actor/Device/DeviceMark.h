// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "DeviceMark.generated.h"

class UDeviceMarkComponent;

UCLASS(Blueprintable,BlueprintType)
class COMMONTOOLCORE_API ADeviceMark : public ABaseDevice
{
	GENERATED_BODY()
	
	ADeviceMark();

	virtual void BeginPlay() override;

	virtual TSharedRef<FJsonObject> GetDeviceJsonObjectData(const FString &GUID) override;

	virtual void SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject) override;
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UDeviceMarkComponent* DeviceMarkComponent;
};
