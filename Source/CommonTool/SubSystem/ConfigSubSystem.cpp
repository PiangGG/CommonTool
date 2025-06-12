// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigSubSystem.h"

#include "CommonTool/Library/JsonFunctionLibrary.h"
#include "CommonTool/Library/ToolFunctionLibrary.h"

UConfigSubSystem* UConfigSubSystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject && GWorld)
	{
		return UGameInstance::GetSubsystem<UConfigSubSystem>(GWorld->GetGameInstance());
	}
	return nullptr;
}

bool UConfigSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UConfigSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UToolFunctionLibrary::Debug(FString("ConfigSubSystem::Initialize"));
	InitConfig();
}

void UConfigSubSystem::Deinitialize()
{
	UToolFunctionLibrary::Debug(FString("ConfigSubSystem::Deinitialize"));
	Super::Deinitialize();
}

void UConfigSubSystem::InitConfig()
{
	UJsonFunctionLibrary::GetJsonString("cfg",JsonStringCfg);
}

FString UConfigSubSystem::GetCfgData()
{
	return JsonStringCfg;
}

void UConfigSubSystem::SetCfgData(const FString& Key, const FString& Value)
{
	FString JsonString = JsonStringCfg;
	UJsonFunctionLibrary::SetStringFromJsonString(JsonString,Key,Value,JsonStringCfg);
	OnConfigChanged.Broadcast(JsonStringCfg);
}
