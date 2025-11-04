// Fill out your copyright notice in the Description page of Project Settings.


#include "DevicePoint.h"

#include "AttributeToolLibrary.h"
#include "UIInterface.h"

ADevicePoint::ADevicePoint()
{
	//改为动态？
	PointWidget=CreateDefaultSubobject<UWidgetComponent>("InfoWidget");
	PointWidget->SetupAttachment(Root);
	PointWidget->SetVisibility(true);
}

void ADevicePoint::BeginPlay()
{
	Super::BeginPlay();

	GetOnActorInitializationCompletedDelegateChecked().AddDynamic(this, &ThisClass::OnActorInitializationCompleted);

	ActorInitializationCompleted(this);
}

void ADevicePoint::OnActorInitializationCompleted(UObject* Object)
{
	if (PointWidgetClass->IsValidLowLevel())
	{
		PointWidget->SetWidgetClass(PointWidgetClass);

		if (PointWidget)
		{
			if (PointWidget->GetWidget()->GetClass()->ImplementsInterface(UUIInterface::StaticClass()))
			{
				IUIInterface::Execute_Init(PointWidget->GetWidget(),Data);
			}
		}
	}
}

void ADevicePoint::SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject)
{
	Super::SetDeviceJsonObjectData(JsonObject);
	ActorInitializationCompleted(this);
}
