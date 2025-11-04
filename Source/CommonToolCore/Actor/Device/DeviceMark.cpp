// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceMark.h"

#include "AttributeToolLibrary.h"
#include "DeviceManager.h"
#include "CommonToolCore/Components/DeviceMarkComponent.h"

ADeviceMark::ADeviceMark()
{
	DeviceMarkComponent = CreateDefaultSubobject<UDeviceMarkComponent>(TEXT("DeviceMarkComponent"));
}

void ADeviceMark::BeginPlay()
{
	Super::BeginPlay();
	ActorInitializationCompleted(this);
}

TSharedRef<FJsonObject> ADeviceMark::GetDeviceJsonObjectData(const FString& GUID)
{
	TSharedRef<FJsonObject> JsonObject = Super::GetDeviceJsonObjectData(GUID);

	TArray<TSharedPtr<FJsonValue>> Attribute = JsonObject->GetArrayField(TEXT("Attribute"));
	TSharedPtr<FJsonObject> ActorJsonName = MakeShared<FJsonObject>();
	ActorJsonName->SetStringField(TEXT("AttributeName"), "astId");
	ActorJsonName->SetStringField(TEXT("AttributeValue"), DeviceMarkComponent->DeviceID);
	ActorJsonName->SetStringField(TEXT("AttributeType"), "EditType.Normal");
	ActorJsonName->SetBoolField(TEXT("bDisplay"), true);
	ActorJsonName->SetBoolField(TEXT("bCanEditor"), true);
	Attribute.Add(MakeShared<FJsonValueObject>(ActorJsonName));
	JsonObject->SetArrayField(TEXT("Attribute"),Attribute);
	return JsonObject;
}

void ADeviceMark::SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject)
{
	Super::SetDeviceJsonObjectData(JsonObject);
	UAttributeToolLibrary::GetStringAttribute(JsonObject,TEXT("astId"),DeviceMarkComponent->DeviceID);
	UDeviceManager::Get(this)->RegisterDevice(DeviceMarkComponent->DeviceID,this);
}

