// Fill out your copyright notice in the Description page of Project Settings.


#include "DocumentMarkComponent.h"

#include "CommonSettings.h"
#include "DocumentManager.h"
#include "SceneManagerSubsystem.h"
#include "StateSubsystem.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UDocumentMarkComponent::UDocumentMarkComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// ...
}


// Called when the game starts
void UDocumentMarkComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	StateSubsystem =  UStateSubsystem::Get(this);
	if (StateSubsystem)
	{
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateChange);
	}

	// SceneManagerSubsystem = USceneManagerSubsystem::Get(this);
	// if (SceneManagerSubsystem)
	// {
	// 	SceneManagerSubsystem->OnSceneChangeSignature.AddDynamic()
	// }
	DocumentManager = UDocumentManager::Get(this);
	if (bRegisterPOI)
	{
		if (DefaultPOI->IsValidLowLevel())
		{
			UserWidget = CreateWidget<UUserWidget>(GetWorld(),DefaultPOI.Get());
		}
		else
		{
			if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
			{
				if (CommonSettings->DefaultPOI != nullptr)
				{
					UserWidget = CreateWidget<UUserWidget>(GetWorld(),CommonSettings->RegionPOI.Get());
				}
			}
		}
	}
}

void UDocumentMarkComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UDocumentMarkComponent::OnSystemStateChange(FGameplayTag newState)
{
	if (DocumentManager)
	{
		bool bShow = newState == ShowStata;
		GetOwner()->SetActorHiddenInGame(bShow);
		if (bShow)
		{
			DocumentManager = UDocumentManager::Get(this);
			if (Cast<AActor>(GetOwner())&&!RegionID.IsEmpty() && bRegisterPOI)
			{
				DocumentManager->RegisterDocumentPOI(RegionID,Cast<AActor>(GetOwner()));
				DocumentManager->InitDocumentPOI(RegionID,bShow,UserWidget,LocationOffsize);
			}
		}
		else
		{
			if (Cast<AActor>(GetOwner()))
			{
				DocumentManager->InitDocumentPOI(RegionID,bShow,UserWidget,LocationOffsize);
				GetOwner()->SetActorHiddenInGame(true);
				DocumentManager->UnRegisterDocumentPOI(RegionID);
			}
		}
	}
}

void UDocumentMarkComponent::OnSceneChange(const FString& name)
{
	// if (DocumentManager)
	// {
	// 	GetOwner()->SetActorHiddenInGame(true);
	// 	if (UserWidget&&bRegisterPOI)
	// 	{
	// 		DocumentManager->InitDocumentPOI(RegionID,false,UserWidget,LocationOffsize);
	// 	}
	// }
}