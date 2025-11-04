// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorManager.h"

#include "GameplayTagContainer.h"
#include "PrintToolLibrary.h"

UEditorManager::UEditorManager()
{
	
}

UEditorManager* UEditorManager::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return UWorld::GetSubsystem<UEditorManager>(GWorld);
	}
	return nullptr;
}

void UEditorManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("EditorManager::Initialize"));
}

void UEditorManager::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("EditorManager::Deinitialize"));
	Super::Deinitialize();
}

void UEditorManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	InitializeScene();
}

void UEditorManager::OnSystemStateOnChange(FGameplayTag State)
{
	
}

void UEditorManager::InitializeScene()
{
	
}

void UEditorManager::SetTransformMode(FGameplayTag InTransformMode, bool CombineTranslateRotate)
{
	
}

void UEditorManager::SetAlignSpace(FGameplayTag InAlignSpace)
{
	
}
