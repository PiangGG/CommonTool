// Fill out your copyright notice in the Description page of Project Settings.

//@author piang
//优化项 用于异步交互

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "StructLibrary.h"
#include "GameplayTask_Interactive.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOLCORE_API UGameplayTask_Interactive : public UGameplayTask
{
	GENERATED_BODY()
	
public:
	UGameplayTask_Interactive(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	static UGameplayTask_Interactive* StartInteractiveTask(UObject* WorldContextObject,FInteractionSettings InteractionSettings,float InteractionScanRate = 0.01);
protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityEnded) override;
	
	void QueryInteractables();
private:
	FInteractionSettings InteractionSettings;
	float InteractionScanRate = 0.100;
	FTimerHandle QueryTimerHandle;
};
