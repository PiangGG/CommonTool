// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "DeviceEditPoint.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class COMMONTOOLCORE_API ADeviceEditPoint : public ABaseDevice
{
	GENERATED_BODY()

	virtual void ReDraw() override;

public:
	UPROPERTY()
	AActor* ParentActor = nullptr;
};
