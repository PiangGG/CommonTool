// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Device/BaseDevice.h"
#include "TrainStationMark.generated.h"

UCLASS()
class COMMONTOOL_API ATrainStationMark : public ABaseDevice
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrainStationMark();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	

};
