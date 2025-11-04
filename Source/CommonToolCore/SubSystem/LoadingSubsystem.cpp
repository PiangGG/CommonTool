// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingSubsystem.h"

#include "CommonToolCore/Library/PrintToolLibrary.h"

int32 ULoadingSubsystem::num = 0;

ULoadingSubsystem* ULoadingSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		ULoadingSubsystem* Router = UGameInstance::GetSubsystem<ULoadingSubsystem>(GWorld->GetGameInstance());
		return Router;
	}
	return nullptr;
}

void ULoadingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("LoadingSubsystem::Initialize"));
	num = 0;
}

void ULoadingSubsystem::Deinitialize()
{
	num = 0;
	UPrintToolLibrary::Debug(FString("LoadingSubsystem::Deinitialize"));
	Super::Deinitialize();
}

void ULoadingSubsystem::ShowLoadScreen(bool bShow)
{
	if (GWorld)
	{
		if (bShow)
		{
			num++;
		}
		else
		{
			num--;
		}
		if (Get(GWorld))	
		{
			Get(GWorld)->SetLoading(num>0);
		}
	}
}

void ULoadingSubsystem::SetLoading(bool bLoad)
{
	if (Loading!=bLoad)
	{
		Loading = bLoad;
		OnLoadScreenShowChange.Broadcast(Loading);
	}
}