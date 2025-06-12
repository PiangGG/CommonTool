// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceMarkComponent.h"

#include "CommonTool/Library/ToolFunctionLibrary.h"

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
		UToolFunctionLibrary::Error(FString::Printf(TEXT("%s Device ID is empty"),*GetOwner()->GetName()));
	}
	
}
