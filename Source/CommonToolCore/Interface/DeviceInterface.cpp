// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceInterface.h"

// Add default functionality here for any IDeviceInterface functions that are not pure virtual.
UDeviceInterface::UDeviceInterface(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

void IDeviceInterface::ActorInitializationCompleted(TScriptInterface<IDeviceInterface> This)
{
	This.GetInterface()->GetOnActorInitializationCompletedDelegateChecked().Broadcast(This.GetObject());
}

void IDeviceInterface::ActorReDraw(TScriptInterface<IDeviceInterface> This)
{
	This.GetInterface()->GetOnOnActorReDrawDelegateChecked().Broadcast();
}
