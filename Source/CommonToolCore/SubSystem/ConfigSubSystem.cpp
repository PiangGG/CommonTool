// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigSubSystem.h"

#include "CommonToolCore/Library/JsonToolLibrary.h"
#include "CommonToolCore/Library/PrintToolLibrary.h"

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
	UPrintToolLibrary::Debug(FString("ConfigSubSystem::Initialize"));
	InitConfig();
}

void UConfigSubSystem::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("ConfigSubSystem::Deinitialize"));
	Super::Deinitialize();
}

void UConfigSubSystem::InitConfig()
{
	UJsonToolLibrary::GetJsonString("cfg",JsonStringCfg,false,"cfg");
}

FString UConfigSubSystem::GetCfgData()
{
	return JsonStringCfg;
}

void UConfigSubSystem::SetCfgData(const FString& Key, const FString& Value)
{
	FString JsonString = JsonStringCfg;
	UJsonToolLibrary::SetStringFromJsonString(JsonString,Key,Value,JsonStringCfg);
	OnConfigChanged.Broadcast(JsonStringCfg);
}
