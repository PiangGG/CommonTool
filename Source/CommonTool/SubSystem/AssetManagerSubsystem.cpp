// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetManagerSubsystem.h"
#include "CommonTool/Library/ToolFunctionLibrary.h"

UAssetManagerSubsystem* UAssetManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject && GWorld)
	{
		return UGameInstance::GetSubsystem<UAssetManagerSubsystem>(GWorld->GetGameInstance());
	}
	return nullptr;
}

bool UAssetManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UAssetManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UToolFunctionLibrary::Debug(FString("UAssetManagerSubsystem::Initialize"));
}

void UAssetManagerSubsystem::Deinitialize()
{
	UToolFunctionLibrary::Debug(FString("UAssetManagerSubsystem::Deinitialize"));
	Super::Deinitialize();
}

