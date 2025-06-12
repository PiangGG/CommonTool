// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "DevicePoint.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class COMMONTOOL_API ADevicePoint : public ABaseDevice
{
	GENERATED_BODY()

public:
	ADevicePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Device", meta=(AllowPrivateAccess=true))
	UWidgetComponent* PointWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Device", meta=(AllowPrivateAccess=true))
	TSubclassOf<UUserWidget> PointWidgetClass;
};
