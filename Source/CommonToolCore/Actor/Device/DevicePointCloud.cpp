// Fill out your copyright notice in the Description page of Project Settings.


#include "DevicePointCloud.h"
#include "SceneManagerSubsystem.h"
#include "CommonToolCore/Actor/Scene/ComLidarPointCloudActor.h"

ADevicePointCloud::ADevicePointCloud(): Actor(nullptr)
{
	BIsLoad = false;
}

void ADevicePointCloud::BeginPlay()
{
	Super::BeginPlay();

	GetOnActorInitializationCompletedDelegateChecked().AddDynamic(this, &ThisClass::OnActorInitializationCompleted);
	ActorInitializationCompleted(this);
}

void ADevicePointCloud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Actor)
	{
		Actor->Destroy();
	}
	
	Super::EndPlay(EndPlayReason);
}

TSharedRef<FJsonObject> ADevicePointCloud::GetDeviceJsonObjectData(const FString &GUID)
{
	TSharedRef<FJsonObject> ActorJson = Super::GetDeviceJsonObjectData(GUID);
	ActorJson->SetStringField("SelectPath",Path);
	return ActorJson;
}

void ADevicePointCloud::SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject)
{
	Super::SetDeviceJsonObjectData(JsonObject);
	
	JsonObject->TryGetStringField(TEXT("SelectPath"),Path);
}

void ADevicePointCloud::OnActorInitializationCompleted(UObject* Object)
{
	if (USceneManagerSubsystem* SceneManagerSubsystem = UWorld::GetSubsystem<USceneManagerSubsystem>(
				GWorld))
	{
		AComLidarPointCloudActor* ComLidarPointCloudActor = SceneManagerSubsystem->ResetPointCloudActor(Path,BIsLoad,Actor,GetActorTransform());
		Actor = ComLidarPointCloudActor;
	}
}

void ADevicePointCloud::ReDraw()
{
	Super::ReDraw();
	
	if (Actor)
	{
		Actor->SetActorTransform(GetActorTransform());
	}
}
