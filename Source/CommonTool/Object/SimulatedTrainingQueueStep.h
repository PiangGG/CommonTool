// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimulatedTrainingQueueStep.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSimulatedTrainingQueue, FString, SimulatedTrainingNodeName);

UCLASS(BlueprintType)
class COMMONTOOL_API USimulatedTrainingQueueStep : public UObject
{
	GENERATED_BODY()

public:
	USimulatedTrainingQueueStep();

	UFUNCTION()
	void SimulatedFlowQueueStep();

	UPROPERTY()
	FString SimulatedFlowName;

	UPROPERTY()
	FSimulatedTrainingQueue SimulatedTrainingQueue;
};
