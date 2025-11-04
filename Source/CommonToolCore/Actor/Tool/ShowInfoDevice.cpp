// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowInfoDevice.h"

AShowInfoDevice::AShowInfoDevice()
{
	
}

void AShowInfoDevice::BeginPlay()
{
	Super::BeginPlay();
	
	ActorInitializationCompleted(this);
}
