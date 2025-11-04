// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceEditPoint.h"

void ADeviceEditPoint::ReDraw()
{
	Super::ReDraw();

	if (ParentActor)
	{
		if (ParentActor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
		{
			Cast<IDeviceInterface>(ParentActor)->GetOnOnActorReDrawDelegateChecked().Broadcast();
		}
	}
}
