// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "DevicePoint.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class COMMONTOOLCORE_API ADevicePoint : public ABaseDevice
{
	GENERATED_BODY()

public:
	ADevicePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnActorInitializationCompleted(UObject* Object);
	virtual void SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject) override;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonToolCore|Device", meta=(AllowPrivateAccess=true))
	UWidgetComponent* PointWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonToolCore|Device", meta=(AllowPrivateAccess=true))
	TSubclassOf<UUserWidget> PointWidgetClass;
};
