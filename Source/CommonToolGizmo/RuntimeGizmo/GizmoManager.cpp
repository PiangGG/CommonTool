// Fill out your copyright notice in the Description page of Project Settings.


#include "GizmoManager.h"

#include "CommonGizmoTransform.h"
#include "GameplayTagContainer.h"
#include "PrintToolLibrary.h"
#include "StateSubsystem.h"
#include "Kismet/GameplayStatics.h"

UGizmoManager::UGizmoManager():
InteractiveSubsystem(nullptr),
StateSubsystem(nullptr),
CommonGizmoTransform(nullptr)
{
	
}

UGizmoManager* UGizmoManager::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return UWorld::GetSubsystem<UGizmoManager>(GWorld);
	}
	return nullptr;
}

void UGizmoManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("UGizmoManager::Initialize"));

}

void UGizmoManager::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("UGizmoManager::Deinitialize"));
	Super::Deinitialize();
}

void UGizmoManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	InteractiveSubsystem = UInteractiveSubsystem::Get(this);
	if (InteractiveSubsystem)
	{
		InteractiveSubsystem->InteractiveActorSignature.AddDynamic(this,&ThisClass::OnInteractiveActor);
		InteractiveSubsystem->OnPointerEnter.AddDynamic(this,&ThisClass::OnPointerEnter);
		InteractiveSubsystem->OnPointerMove.AddDynamic(this,&ThisClass::OnPointerMove);
		InteractiveSubsystem->OnPointerLeave.AddDynamic(this,&ThisClass::OnPointerLeave);
		InteractiveSubsystem->OnPointerUp.AddDynamic(this,&ThisClass::OnPointerUp);
		InteractiveSubsystem->OnPointerDrag.AddDynamic(this,&ThisClass::OnPointerDrag);
		InteractiveSubsystem->OnPointerDown.AddDynamic(this,&ThisClass::OnPointerDown);
	}
	StateSubsystem = UStateSubsystem::Get(this);

	InitializeScene();
}

void UGizmoManager::InitializeScene()
{
	CommonGizmoTransform = Cast<ACommonGizmoTransform>
	(UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(), ACommonGizmoTransform::StaticClass(),
														FTransform(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	UGameplayStatics::FinishSpawningActor(CommonGizmoTransform, FTransform());
}

void UGizmoManager::SetTransformMode(FGameplayTag InTransformMode, bool CombineTranslateRotate)
{
	OnChangeTransformMode.Broadcast(InTransformMode, CombineTranslateRotate);
}

void UGizmoManager::SetAlignSpace(FGameplayTag InAlignSpace)
{
	OnChangeAlignSpace.Broadcast(InAlignSpace);
}


void UGizmoManager::OnInteractiveActor(AActor* Actor)
{
	if (StateSubsystem)
	{
		FGameplayTag SystemState;
		StateSubsystem->GetSystemState(SystemState);

		if (SystemState == CommonToolTags::State_Edit)
		{
			if (Actor)
			{
				OnActivateGizmo.Broadcast(Actor);
			}
			else
			{
				OnActivateGizmo.Broadcast(nullptr);
			}
		}
		else
		{
			OnActivateGizmo.Broadcast(nullptr);
		}
	}
}

void UGizmoManager::OnPointerEnter(UPrimitiveComponent* Component)
{
	if (CommonGizmoTransform)
	{
		ICommonGizmoInterface::Execute_OnPointerEnter(CommonGizmoTransform,Component);
	}
}

void UGizmoManager::OnPointerMove(UPrimitiveComponent* Component)
{
	if (CommonGizmoTransform)
	{
		ICommonGizmoInterface::Execute_OnPointerMove(CommonGizmoTransform,Component);
	}
}

void UGizmoManager::OnPointerLeave(UPrimitiveComponent* Component)
{
	if (CommonGizmoTransform)
	{
		ICommonGizmoInterface::Execute_OnPointerLeave(CommonGizmoTransform,Component);
	}
}

void UGizmoManager::OnPointerUp(UPrimitiveComponent* Component)
{
	if (CommonGizmoTransform)
	{
		ICommonGizmoInterface::Execute_OnPointerUp(CommonGizmoTransform,Component);
	}
}

void UGizmoManager::OnPointerDrag(UPrimitiveComponent* Component, const FVector& RayStart, const FVector& RayEnd)
{
	if (CommonGizmoTransform)
	{
		ICommonGizmoInterface::Execute_OnPointerDrag(CommonGizmoTransform,Component,RayStart,RayEnd);
	}
}

void UGizmoManager::OnPointerDown(UPrimitiveComponent* Component, const FVector& RayStart, const FVector& RayEnd)
{
	if (CommonGizmoTransform)
	{
		ICommonGizmoInterface::Execute_OnPointerDown(CommonGizmoTransform,Component,RayStart,RayEnd);
	}
}