// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "ShowInfoDevice.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOLCORE_API AShowInfoDevice : public ABaseDevice
{
	GENERATED_BODY()

public:
	AShowInfoDevice();
	
	virtual void BeginPlay() override;
};
