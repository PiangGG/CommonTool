// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManagerSubsystem.h"

#include "CommonToolTags.h"
#include "CommonTool/Library/StructLibrary.h"
#include "CommonTool/Library/PrintToolLibrary.h"

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
	
}

void UUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UUIManagerSubsystem::ReSetDeviceInfo(const FString& ID,AActor* Actor, bool bAddOrRemove,const FGameplayTag&type,UUserWidget*UserWidget)
{
	OnDeviceInfoReset.Broadcast(ID,Actor,bAddOrRemove,type,UserWidget);
}

void UUIManagerSubsystem::DisassembleDevice(USkeletalMeshComponent* SkeletalMeshComponent)
{
	TArray<FName> BoneNames;
	SkeletalMeshComponent->GetBoneNames(BoneNames);

	TArray<FName> TempBoneNames = BoneNames;
	for (auto BoneName : TempBoneNames)
	{
		if (BoneName.IsEqual("")||BoneName.IsEqual("None"))
		{
			BoneNames.Remove(BoneName);
		}
	}
	
	OnDisassembleDevice.Broadcast(SkeletalMeshComponent,BoneNames);
}

void UUIManagerSubsystem::SelectedDeviceComponent(const FString& DeviceComponentName)
{
	OnSelectedDeviceComponent.Broadcast(DeviceComponentName);
}