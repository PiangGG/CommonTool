// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceDataSmith.h"

#include "CommonToolCore/Actor/Scene/ComDatasmithActor.h"
#include "CommonToolCore/SubSystem/SceneManagerSubsystem.h"

ADeviceDataSmith::ADeviceDataSmith(): Actor(nullptr)
{

}

void ADeviceDataSmith::BeginPlay()
{
	Super::BeginPlay();
	GetOnActorInitializationCompletedDelegateChecked().AddDynamic(this, &ThisClass::OnActorInitializationCompleted);
	ActorInitializationCompleted(this);
}

void ADeviceDataSmith::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Actor)
	{
		Actor->Destroy();
	}
	Actor = nullptr;
	Super::EndPlay(EndPlayReason);
}

TSharedRef<FJsonObject> ADeviceDataSmith::GetDeviceJsonObjectData(const FString &GUID)
{
	TSharedRef<FJsonObject> ActorJson = Super::GetDeviceJsonObjectData(GUID);
	ActorJson->SetStringField("SelectPath",Path);
	return ActorJson;
}

void ADeviceDataSmith::SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject)
{
	Super::SetDeviceJsonObjectData(JsonObject);
	
	JsonObject->TryGetStringField(TEXT("SelectPath"),Path);
}

void ADeviceDataSmith::OnActorInitializationCompleted(UObject* Object)
{
	if (USceneManagerSubsystem* SceneManagerSubsystem = UWorld::GetSubsystem<USceneManagerSubsystem>(
				GWorld))
	{
		//SceneManagerSubsystem->OnResetDataSmithRuntimeActor.Broadcast(Path,Actor);
		AComDatasmithActor* ComDatasmithActor = SceneManagerSubsystem->ResetDataSmithActor(Path,BIsLoad,Actor,GetActorTransform());
		Actor = ComDatasmithActor;
	}
}

void ADeviceDataSmith::SetDeviceLoad_Implementation(const FString& bload)
{
	Super::SetDeviceLoad_Implementation(bload);
	
}

void ADeviceDataSmith::ReDraw()
{
	Super::ReDraw();

	if (Actor)
	{
		Actor->SetActorTransform(GetActorTransform());
	}
}
