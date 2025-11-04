// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "DeviceDataSmith.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class COMMONTOOLCORE_API ADeviceDataSmith : public ABaseDevice
{
	GENERATED_BODY()

	ADeviceDataSmith();
	
public:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditAnywhere, Category=Device)
	FString Path;
	
	virtual TSharedRef<FJsonObject> GetDeviceJsonObjectData(const FString &GUID) override;
	
	virtual void SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject) override;

	UFUNCTION()
	void OnActorInitializationCompleted(UObject* Object);

	virtual void SetDeviceLoad_Implementation(const FString& bload) override;

	virtual void ReDraw() override;
private:
	UPROPERTY()
	class AComDatasmithActor* Actor;
};

