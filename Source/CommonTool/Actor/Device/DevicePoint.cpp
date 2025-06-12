// Fill out your copyright notice in the Description page of Project Settings.


#include "DevicePoint.h"
#include "Components/WidgetComponent.h"

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
	if (PointWidgetClass->IsValidLowLevel())
	{
		PointWidget->SetWidgetClass(PointWidgetClass);
	}
	ActorInitializationCompleted(this);
}
