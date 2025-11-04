// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageSubsystem.h"

#include "CommonToolCore/Library/PrintToolLibrary.h"

UMessageSubsystem* UMessageSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		UMessageSubsystem* Router = UGameInstance::GetSubsystem<UMessageSubsystem>(GWorld->GetGameInstance());
		return Router;
	}
	return nullptr;
}

void UMessageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("MessageSubsystem::Initialize"));
}

void UMessageSubsystem::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("MessageSubsystem::Deinitialize"));
	Super::Deinitialize();
}

void UMessageSubsystem::Message(FString msg, FGameplayTag Type)
{
	Get(GWorld)->SendMessage(msg,Type);
}

void UMessageSubsystem::SendMessage(const FString& msg, FGameplayTag Type) const
{
	MessageSignature.Broadcast(msg,Type);
}
