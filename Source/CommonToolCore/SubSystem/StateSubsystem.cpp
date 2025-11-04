// Fill out your copyright notice in the Description page of Project Settings.


#include "StateSubsystem.h"

#include "CommonToolCoreTags.h"
#include "CommonToolCore/Library/PrintToolLibrary.h"

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
	UPrintToolLibrary::Debug(FString("StateSubsystem::Initialize"));
}

void UStateSubsystem::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("StateSubsystem::Deinitialize"));
	Super::Deinitialize();
}

void UStateSubsystem::ChangeSystemState(const FGameplayTag& NewState)
{
	if (NewState!= CommonToolTags::State_Null)
	{
		if (NewState != CurrentStateTag)
		{
			CurrentStateTag = NewState;
			SystemStateOnChange.Broadcast(CurrentStateTag);
			UPrintToolLibrary::Debug(FString("StateSubsystem::ChangeSystemState: ") + CurrentStateTag.ToString());
		}
		else
		{
			//CurrentStateTag = CommonToolTags::State_MainMenu;
			if (CurrentStateTag == CommonToolTags::State_MainMenu)
			{
				SystemStateOnChange.Broadcast(CurrentStateTag);
				UPrintToolLibrary::Debug(FString("StateSubsystem::ChangeSystemState: ") + CurrentStateTag.ToString());
			}
		}
	}
	else
	{
		CurrentStateTag = NewState;
		SystemStateOnChange.Broadcast(CurrentStateTag);
		UPrintToolLibrary::Debug(FString("StateSubsystem::ChangeSystemState: ") + CurrentStateTag.ToString());
	}
}

void UStateSubsystem::GetSystemState(FGameplayTag& CurState)
{
	CurState = CurrentStateTag;
}

void UStateSubsystem::ChangeChildSystemState(const FGameplayTag& NewState)
{
	if (NewState != CurrentChildStateTag)
	{
		CurrentChildStateTag = NewState;
		ChildSystemStateOnChange.Broadcast(CurrentChildStateTag);
		UPrintToolLibrary::Debug(FString("StateSubsystem::ChangeChildSystemState: ") + CurrentChildStateTag.ToString());
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
	}
	UserStateOnChange.Broadcast(CurrentUserStateTag, Transform, Zoom,bblend);
	UPrintToolLibrary::Debug(FString("StateSubsystem::ChangeUserState: ") + CurrentUserStateTag.ToString());
}

void UStateSubsystem::GetUserState(FGameplayTag& CurState)
{
	CurState = CurrentUserStateTag;
}

void UStateSubsystem::ChangeEditState(const FGameplayTag& NewState)
{
	if (NewState != CurrentEditTag)
	{
		CurrentEditTag = NewState;
		UPrintToolLibrary::Debug(FString("StateSubsystem::ChangeEditState: ") + NewState.ToString());
	}
	EditStateOnChange.Broadcast(CurrentEditTag);
}

void UStateSubsystem::GetEditState(FGameplayTag& CurState)
{
	CurState = CurrentEditTag;
}

void UStateSubsystem::ChangeDeviceState(const FGameplayTag& NewState,const FString& ID)
{
	if (NewState != CurrentDeviceStateTag)
	{
		CurrentDeviceStateTag = NewState;
	}
	// else
	// {
	// 	CurrentDeviceStateTag = CommonToolTags::DeviceState_Ledger;
	// }
	UPrintToolLibrary::Debug(FString("StateSubsystem::ChangeDeviceState: ") + CurrentDeviceStateTag.ToString());
	DeviceStateOnChange.Broadcast(CurrentDeviceStateTag,ID);
}

void UStateSubsystem::GetDeviceState(FGameplayTag& CurState)
{
	CurState = CurrentDeviceStateTag;
}

void UStateSubsystem::ChangeOperationState(const FGameplayTag& NewState)
{
	if (NewState != CurrentOperationStateTag)
	{
		CurrentOperationStateTag = NewState;
	}
	
	UPrintToolLibrary::Debug(FString("StateSubsystem::ChangeDeviceState: ") + CurrentOperationStateTag.ToString());
	OperationStateOnChange.Broadcast(CurrentOperationStateTag);
}

void UStateSubsystem::GetOperationState(FGameplayTag& CurState)
{
	CurState = CurrentOperationStateTag;
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
	UPrintToolLibrary::Debug(FString("UStateSubsystem::ChangeDialogState: ") + CurrentDialogStateTag.ToString());
	DialogStateChangeSignature.Broadcast(CurrentDialogStateTag);
}

void UStateSubsystem::GetDialogState(FGameplayTag& CurState)
{
	CurState = CurrentDialogStateTag;
}

void UStateSubsystem::ChangeMarkState(const FGameplayTag& NewState)
{
	if (NewState != CurrentMarkStateTag)
	{
		CurrentMarkStateTag = NewState;
	}
	else
	{
		CurrentMarkStateTag = CommonToolTags::Actor_Type_Null;
	}
	UPrintToolLibrary::Debug(FString("UStateSubsystem::ChangeMarkState: ") + CurrentMarkStateTag.ToString());
	MarkStateChangeSignature.Broadcast(CurrentMarkStateTag);
}

void UStateSubsystem::GetMarkState(FGameplayTag& CurState)
{
	CurState = CurrentMarkStateTag;
}

void UStateSubsystem::ChangeUIState(const FGameplayTag& NewState)
{
	if (NewState != CurrentUIStateTag)
	{
		CurrentUIStateTag = NewState;
	}
	else
	{
		CurrentUIStateTag = CommonToolTags::UIState_Show;
	}
	UPrintToolLibrary::Debug(FString("UStateSubsystem::ChangeUIState: ") + CurrentUIStateTag.ToString());
	UIChangeSignature.Broadcast(CurrentUIStateTag);
}

void UStateSubsystem::GetUIState(FGameplayTag& CurState)
{
	CurState = CurrentUIStateTag;
}
