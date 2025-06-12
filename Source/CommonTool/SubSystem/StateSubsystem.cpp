// Fill out your copyright notice in the Description page of Project Settings.


#include "StateSubsystem.h"

#include "CommonToolTags.h"
#include "TrainStationManager.h"
#include "CommonTool/Library/ToolFunctionLibrary.h"

UStateSubsystem* UStateSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject && GWorld)
	{
		return UGameInstance::GetSubsystem<UStateSubsystem>(GWorld->GetGameInstance());
	}
	return nullptr;
}

bool UStateSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UToolFunctionLibrary::Debug(FString("StateSubsystem::Initialize"));

	CurrentUserStateTag = CommonToolTags::UserState_Null;
}

void UStateSubsystem::Deinitialize()
{
	UToolFunctionLibrary::Debug(FString("StateSubsystem::Deinitialize"));
	Super::Deinitialize();
}

void UStateSubsystem::ChangeSystemState(const FGameplayTag& NewState)
{
	if (NewState!= CommonToolTags::State_Null)
	{
		if (NewState != CurrentSystemStateTag)
		{
			CurrentSystemStateTag = NewState;
		}
		else
		{
			CurrentSystemStateTag = CommonToolTags::State_MainMenu;
		}
	}
	else
	{
		CurrentSystemStateTag = NewState;
		FTransform Transform;
		Transform.SetLocation(FVector(-177302.2, -29375.4, 456969.3));
		Transform.SetRotation(FRotator(-76.1, 736.6, 701.2).Quaternion());
		ChangeUserState(CommonToolTags::UserState_Panorama,Transform,0);
	}
	
	UToolFunctionLibrary::Debug(FString("StateSubsystem::ChangeSystemState: ") + CurrentSystemStateTag.ToString());
	SystemStateOnChange.Broadcast(CurrentSystemStateTag);
}

void UStateSubsystem::GetSystemState(FGameplayTag& CurState)
{
	CurState = CurrentSystemStateTag;
}

void UStateSubsystem::ChangeChildSystemState(const FGameplayTag& NewState)
{
	if (NewState != CurrentChildStateTag)
	{
		CurrentChildStateTag = NewState;
		ChildSystemStateOnChange.Broadcast(CurrentChildStateTag);
	}
}

void UStateSubsystem::GetChildSystemState(FGameplayTag& CurState)
{
	CurState = CurrentChildStateTag;
}

void UStateSubsystem::ChangeUserState(const FGameplayTag& NewState, FTransform Transform, float Zoom,bool bblend)
{
	if (NewState != CurrentUserStateTag)
	{
		CurrentUserStateTag = NewState;
		UToolFunctionLibrary::Debug(FString("StateSubsystem::ChangeUserState: ") + NewState.ToString());
	}
	UserStateOnChange.Broadcast(CurrentUserStateTag, Transform, Zoom,bblend);
}

void UStateSubsystem::GetUserState(FGameplayTag& CurState)
{
	CurState = CurrentUserStateTag;
}

void UStateSubsystem::ChangeEditState(const FGameplayTag& NewState)
{
	if (NewState != CurrentEditStateTag)
	{
		CurrentEditStateTag = NewState;
		UToolFunctionLibrary::Debug(FString("StateSubsystem::ChangeEditState: ") + NewState.ToString());
	}
	EditStateOnChange.Broadcast(CurrentEditStateTag);
}

void UStateSubsystem::GetEditState(FGameplayTag& CurState)
{
	CurState = CurrentEditStateTag;
}

void UStateSubsystem::ChangeDialogState(const FGameplayTag& NewState)
{
	if (NewState != CurrentDialogStateTag)
	{
		CurrentDialogStateTag = NewState;
	}
	else
	{
		CurrentDialogStateTag = CommonToolTags::DialogState_Null;
	}
	UToolFunctionLibrary::Debug(FString("UIManagerSubsystem::ChangeDialogState: ") + CurrentDialogStateTag.ToString());
	DialogStateOnChangeSignature.Broadcast(CurrentDialogStateTag);
}

void UStateSubsystem::GetDialogState(FGameplayTag& CurState)
{
	CurState = CurrentDialogStateTag;
}

void UStateSubsystem::ChangeDeviceState(const FGameplayTag& NewState)
{
	if (NewState != CurrentDeviceStateTag)
	{
		CurrentDeviceStateTag = NewState;
		UToolFunctionLibrary::Debug(FString("StateSubsystem::ChangeDeviceState: ") + NewState.ToString());
	}
	DeviceStateOnChange.Broadcast(CurrentDeviceStateTag);
}

void UStateSubsystem::GetDeviceState(FGameplayTag& CurState)
{
	CurState = CurrentDeviceStateTag;
}
