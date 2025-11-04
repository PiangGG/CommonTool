// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceMarkComponent.h"

#include "DeviceManager.h"
#include "InspectionManager.h"
#include "InteractiveSubsystem.h"
#include "CommonToolCore/Library/PrintToolLibrary.h"

// Sets default values for this component's properties
UDeviceMarkComponent::UDeviceMarkComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...
}

FString UDeviceMarkComponent::GetDeviceID()
{
	return DeviceID;
}


// Called when the game starts
void UDeviceMarkComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (DeviceID.IsEmpty())
	{
		UPrintToolLibrary::Error(FString::Printf(TEXT("%s Device ID is empty"),*GetOwner()->GetName()));
	}
	else
	{
		DeviceManager = UDeviceManager::Get(this);
		if (DeviceManager&&Cast<AActor>(GetOwner()))
		{
			DeviceManager->RegisterDevice(DeviceID,Cast<AActor>(GetOwner()));
		}
		InspectionManager = UInspectionManager::Get(this);
		if (InspectionManager)
		{
			InspectionManager->InspectionCheckSignature.AddDynamic(this,&ThisClass::InspectionCheck);
		}
		InteractiveSubsystem = UInteractiveSubsystem::Get(this);
		if (InteractiveSubsystem)
		{
			//InteractiveSubsystem->InteractiveActorSignature.AddDynamic(this,&ThisClass::)
		}
	}
}

void UDeviceMarkComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!DeviceID.IsEmpty())
	{
		DeviceManager = UDeviceManager::Get(this);
		if (DeviceManager)
		{
			DeviceManager->PushDevicePOIPool(DeviceID);
			DeviceManager->UnRegisterDevice(DeviceID);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void UDeviceMarkComponent::InspectionCheck(bool bInspection, const FTransform& Transform)
{
	if (bInspection)
	{
		InspectionManager->InspectionTransformSignature.AddDynamic(this,&ThisClass::InspectionTransform);
	}
	else
	{
		DeviceManager->PushDevicePOIPool(DeviceID);
		InspectionManager->InspectionTransformSignature.RemoveDynamic(this,&ThisClass::InspectionTransform);
	}
}

void UDeviceMarkComponent::InspectionTransform(const FTransform& InTransform)
{
	if (GetOwner())
	{
		if (FVector::Distance(GetOwner()->GetActorLocation(),InTransform.GetLocation())<=5000.f)
		{
			DeviceManager->PopDevicePOIPool(DeviceID);
		}
		else
		{
			DeviceManager->PushDevicePOIPool(DeviceID);
		}
	}
}
