// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTask_Interactive.h"
#include "CommonTool/Library/ToolFunctionLibrary.h"

UGameplayTask_Interactive::UGameplayTask_Interactive(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	
}

UGameplayTask_Interactive* UGameplayTask_Interactive::StartInteractiveTask(UObject* WorldContextObject,
	FInteractionSettings InteractionSettings,float InteractionScanRate)
{
	UGameplayTask_Interactive* InteractiveTask = NewTask<UGameplayTask_Interactive>(WorldContextObject);
	if (InteractiveTask)
	{
		InteractiveTask->InteractionSettings = InteractionSettings;
		InteractiveTask->InteractionScanRate = InteractionScanRate;
	}
	return InteractiveTask;
}

void UGameplayTask_Interactive::Activate()
{
	Super::Activate();
	UToolFunctionLibrary::Debug(FString::Printf(TEXT("GameplayTask_Interactive::Activate")));
	
	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
}

void UGameplayTask_Interactive::OnDestroy(bool AbilityEnded)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
		UToolFunctionLibrary::Debug(FString::Printf(TEXT("GameplayTask_Interactive::OnDestroy")));
	}
	Super::OnDestroy(AbilityEnded);
}

void UGameplayTask_Interactive::QueryInteractables()
{
	
}