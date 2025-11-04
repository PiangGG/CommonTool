// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceInfo.h"

#include "DeviceManager.h"
#include "CommonSettings.h"
#include "Components/WidgetComponent.h"

// Sets default values
ADeviceInfo::ADeviceInfo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
}

void ADeviceInfo::Pop(const FString& id)
{
	SetActorHiddenInGame(false);
	// if (DeviceManager)
	// {
	// 	if (DeviceManager->GetDevice(id))
	// 	{
	// 		
	// 		DeviceManager->InitDevicePOI(id,true);
	// 	}
	// }
}

void ADeviceInfo::Push(const FString& id)
{
	SetActorHiddenInGame(true);
	// if (DeviceManager)
	// {
	// 	DeviceManager->InitDevicePOI(id,false);
	// }
}

void ADeviceInfo::OnSetDeviceID_Implementation(const FString& id)
{
	
}

UUserWidget* ADeviceInfo::GetDevicePOI()
{
	if (UserWidget)
	{
		return UserWidget;
	}
	else
	{
		if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
		{
			if (CommonSettings->DevicePOI != nullptr)
			{
				UserWidget = CreateWidget<UUserWidget>(GetWorld(),CommonSettings->DevicePOI.Get());
			}
		}
		return UserWidget;
	}
}

// Called when the game starts or when spawned
void ADeviceInfo::BeginPlay()
{
	Super::BeginPlay();

	DeviceManager = UDeviceManager::Get(this);
	SetActorHiddenInGame(true);
}
