// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceInfo.h"

#include "CommonTool/Library/CommDeveloperSettings.h"
#include "CommonTool/SubSystem/UIManagerSubsystem.h"
#include "Components/WidgetComponent.h"

// Sets default values
ADeviceInfo::ADeviceInfo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//改为动态？
	// InfoWidget = CreateDefaultSubobject<UWidgetComponent>("InfoWidget");
	// InfoWidget->SetupAttachment(Root);
	// InfoWidget->SetVisibility(false);
	// InfoWidget->SetWidgetSpace(EWidgetSpace::Screen);
	// InfoWidget->SetDrawAtDesiredSize(true);
}

void ADeviceInfo::SetDeviceKey(const FString& id,AActor *Device)
{
	if (UIManagerSubsystem)
	{
		UIManagerSubsystem->ReSetDeviceInfo(id,this,!id.IsEmpty(),CommonToolTags::E3DUI_Type_DeviceInfo,UserWidget);
	}
	
	ID = id;
		
	if (ID.IsEmpty())
	{
		SetActorHiddenInGame(true);
		//InfoWidget->SetVisibility(false);
		SetActorLocation(FVector(0,0,-100000000));
	}
	else
	{
		SetActorHiddenInGame(false);
		//InfoWidget->SetVisibility(true);
		OnSetDeviceKey(ID);
		if (Device)
		{
			SetActorLocation(Device->GetActorLocation());
		}
	}
}

void ADeviceInfo::OnSetDeviceKey_Implementation(const FString& id)
{
	
}

// Called when the game starts or when spawned
void ADeviceInfo::BeginPlay()
{
	Super::BeginPlay();

	UIManagerSubsystem = UUIManagerSubsystem::Get(this);
	SetActorHiddenInGame(true);
	
	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
	{
		if (Settings->ActorInfoWidget != nullptr)
		{
			//InfoWidget->SetWidgetClass(Settings->ActorInfoWidget);
			
			UserWidget = CreateWidget<UUserWidget>(GetWorld(),Settings->ActorInfoWidget.Get());
		}
	}
}
