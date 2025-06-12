// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingSubsystem.h"

#include "CommonTool/Library/ToolFunctionLibrary.h"

bool ULoadingSubsystem::Loading = false;
bool ULoadingSubsystem::LastLoading = false;
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
	UToolFunctionLibrary::Debug(FString("LoadingSubsystem::Initialize"));
}

void ULoadingSubsystem::Deinitialize()
{
	UToolFunctionLibrary::Debug(FString("LoadingSubsystem::Deinitialize"));
	Super::Deinitialize();
}

void ULoadingSubsystem::ShowLoadScreen(bool bShow)
{
	if (GWorld)
	{
		//Get(GWorld)->SetLoading(bShow);
		if (bShow)
		{
			num++;
		}
		else
		{
			num--;
		}
		LastLoading = Loading;
		Loading = num>0?true:false;
		if (Get(GWorld))
		{
			Get(GWorld)->SetLoading(Loading);
		}
	}
}

void ULoadingSubsystem::SetLoading(bool bLoad)
{
	// if (bLoad)
	// {
	// 	num++;
	// }
	// else
	// {
	// 	num--;
	// }
	// LastLoading = GetLoading();
	// Loading = num>0?true:false;
	// if (LastLoading != Loading)
	// {
	OnLoadScreenShowChange.Broadcast(bLoad);
	// }
}