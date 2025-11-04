// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManagerSubsystem.h"

#include "CommonToolCoreTags.h"
#include "CommonToolCore/Library/StructLibrary.h"
#include "CommonToolCore/Library/PrintToolLibrary.h"

UUIManagerSubsystem* UUIManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject && GWorld)
	{
		return UGameInstance::GetSubsystem<UUIManagerSubsystem>(GWorld->GetGameInstance());
	}
	return nullptr;
}

bool UUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("UUIManagerSubsystem::Initialize"));
	
}

void UUIManagerSubsystem::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("UUIManagerSubsystem::Deinitialize"));
	Super::Deinitialize();
}
