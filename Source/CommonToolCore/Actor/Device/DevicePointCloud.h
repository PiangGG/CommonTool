// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "DevicePointCloud.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class COMMONTOOLCORE_API ADevicePointCloud : public ABaseDevice
{
	GENERATED_BODY()
	
	ADevicePointCloud();
	
public:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UPROPERTY(EditAnywhere, Category=Device)
	FString Path;

	virtual TSharedRef<FJsonObject> GetDeviceJsonObjectData(const FString &GUID) override;
	
	virtual void SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject) override;

	UFUNCTION()
	void OnActorInitializationCompleted(UObject* Object);

	virtual void ReDraw() override;
private:
	UPROPERTY()
	class AComLidarPointCloudActor* Actor;
};
