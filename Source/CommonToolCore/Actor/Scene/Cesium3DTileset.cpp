// Fill out your copyright notice in the Description page of Project Settings.


#include "ComCesium3DTileset.h"
#include "CesiumRuntime/Public/Cesium3DTileset.h"

#include "AttributeToolLibrary.h"
#include "SceneManagerSubsystem.h"

AComCesium3DTileset::AComCesium3DTileset(): Actor(nullptr)
{
	BIsLoad = false;
}

void AComCesium3DTileset::BeginPlay()
{
	Super::BeginPlay();
	GetOnActorInitializationCompletedDelegateChecked().AddDynamic(this, &ThisClass::OnActorInitializationCompleted);
	ActorInitializationCompleted(this);
}

void AComCesium3DTileset::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Actor)
	{
		Actor->Destroy();
	}
	Actor = nullptr;
	Super::EndPlay(EndPlayReason);
}

TSharedRef<FJsonObject> AComCesium3DTileset::GetDeviceJsonObjectData(const FString &GUID)
{
	TSharedRef<FJsonObject> ActorJson = Super::GetDeviceJsonObjectData(GUID);

	TArray<TSharedPtr<FJsonValue>> Attribute = ActorJson->GetArrayField(TEXT("Attribute"));
	TSharedPtr<FJsonObject> ActorJsonName = MakeShared<FJsonObject>();
	ActorJsonName->SetStringField(TEXT("AttributeName"), "Path");
	ActorJsonName->SetStringField(TEXT("AttributeValue"), Path);
	ActorJsonName->SetStringField(TEXT("AttributeType"), "EditType.Select");
	ActorJsonName->SetBoolField(TEXT("bDisplay"), true);
	ActorJsonName->SetBoolField(TEXT("bCanEditor"), true);
	Attribute.Add(MakeShared<FJsonValueObject>(ActorJsonName));
	ActorJson->SetArrayField(TEXT("Attribute"),Attribute);
	return ActorJson;
}

void AComCesium3DTileset::SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject)
{
	Super::SetDeviceJsonObjectData(JsonObject);
	UAttributeToolLibrary::GetStringAttribute(JsonObject,TEXT("Path"),Path);
	UAttributeToolLibrary::GetBoolAttribute(JsonObject,TEXT("bLoad"),BIsLoad);
	ActorInitializationCompleted(this);
}

AActor* AComCesium3DTileset::GetActor_Implementation()
{
	return Actor?Cast<AActor>(Actor):this;
}

void AComCesium3DTileset::OnActorInitializationCompleted(UObject* Object)
{
	if (USceneManagerSubsystem* SceneManagerSubsystem = UWorld::GetSubsystem<USceneManagerSubsystem>(
				GWorld))
	{
		ACesium3DTileset* InCesium3DTileset = SceneManagerSubsystem->ResetCesium3DTileSetActor(Path,BIsLoad,Actor,GetActorTransform());
		Actor = InCesium3DTileset;
	}
}

void AComCesium3DTileset::SetDeviceLoad_Implementation(const FString& bload)
{
	Super::SetDeviceLoad_Implementation(bload);
}

void AComCesium3DTileset::ReDraw()
{
	Super::ReDraw();

	if (Actor)
	{
		Actor->SetActorTransform(GetActorTransform());
	}
}
