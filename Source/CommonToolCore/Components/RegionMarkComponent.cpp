// Fill out your copyright notice in the Description page of Project Settings.


#include "RegionMarkComponent.h"

#include "CommonSettings.h"
#include "SceneManagerSubsystem.h"
#include "StateSubsystem.h"
#include "PrintToolLibrary.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
URegionMarkComponent::URegionMarkComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// ...
}


// Called when the game starts
void URegionMarkComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (RegionID.IsEmpty())
	{
		UPrintToolLibrary::Error(FString::Printf(TEXT("%s Device ID is empty"),*GetOwner()->GetName()));
	}
	else
	{
		SceneManagerSubsystem = USceneManagerSubsystem::Get(this);
		if (SceneManagerSubsystem&&Cast<AActor>(GetOwner())&&!RegionID.IsEmpty() && bRegisterScene)
		{
			SceneManagerSubsystem->RegisterScene(RegionID,Cast<AActor>(GetOwner()));
		}
		StateSubsystem = UStateSubsystem::Get(this);
		if (StateSubsystem)
		{
			StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateChange);
		}
		
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
		OnSystemStateChange(ShowStata);
	}
}

void URegionMarkComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SceneManagerSubsystem&&Cast<AActor>(GetOwner()))
	{
		SceneManagerSubsystem->UnRegisterScene(RegionID);
	}
	Super::EndPlay(EndPlayReason);
}

void URegionMarkComponent::OnSystemStateChange(FGameplayTag newState)
{
	if (SceneManagerSubsystem)
	{
		FString TrainStationData;
		bool bShow = newState == ShowStata;
		GetOwner()->SetActorHiddenInGame(bShow);
		if (UserWidget&&bRegisterPOI)
		{
			SceneManagerSubsystem->InitScenePOI(RegionID,bShow,UserWidget,LocationOffsize);
		}
	}
}

FString URegionMarkComponent::GetRegionID()
{
	return RegionID;
}

int32 URegionMarkComponent::GetPOI0ffsize()
{
	return POI0ffsize;
}

#if WITH_EDITOR
void URegionMarkComponent::GenerateDevice()
{
	UPrintToolLibrary::Debug(TEXT("GenerateDevice"));
}
#endif