// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceManager.h"

#include "SceneManagerSubsystem.h"
#include "StateSubsystem.h"
#include "RegionManager.h"
#include "CommonTool/Actor/DeviceInfo.h"
#include "CommonTool/Library/CommDeveloperSettings.h"
#include "CommonTool/Library/JsonToolLibrary.h"
#include "CommonTool/Library/PrintToolLibrary.h"
#include "CommonTool/SubSystem/ConfigSubSystem.h"
#include "CommonTool/SubSystem/HttpSubSystem.h"
#include "CommonTool/SubSystem/LoadingSubsystem.h"

class UCommDeveloperSettings;

UDeviceManager::UDeviceManager()
{
}

void UDeviceManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	InitializeDeviceInfoPool();

	OnDeviceDataRefresh.AddDynamic(this,&ThisClass::OnDeviceDataRefreshFunc);

	StateSubsystem = UStateSubsystem::Get(this);
	if (StateSubsystem)
	{
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);
	}
	
	TrainStationManager = UTrainStationManager::Get(this);
	
	SceneManagerSubsystem = USceneManagerSubsystem::Get(this);
	
	if (TrainStationManager)
	{
		TrainStationManager->TrainStationSelected.AddDynamic(this,&ThisClass::OnTrainStationSelected);
	}
	States.AddUnique(TEXT("总数"));
	States.AddUnique(TEXT("在运"));
	States.AddUnique(TEXT("停运"));
	States.AddUnique(TEXT("告警"));
	StateCount.Add(TEXT("总数"),0);
	StateCount.Add(TEXT("在运"),0);
	StateCount.Add(TEXT("停运"),0);
	StateCount.Add(TEXT("告警"),0);
}

void UDeviceManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDeviceManager::Deinitialize()
{
	Super::Deinitialize();
}

void UDeviceManager::OnTrainStationSelected(const FString& TrainStation)
{
	DeviceDataMap.Empty();
	CurrentType = "";
	CurrentpsrType = "";
	typeIds.Empty();
	typeCount.Empty();
	psrTypeIds.Empty();
	DeviceList.Empty();
	DeviceListKey.Empty();
	
	//Todo TrainStation 站点将会获取实际的站点数据 现在所有的都获取模拟数据
	
	if (UHttpSubSystem* HttpSubSystem = UHttpSubSystem::Get(this))
	{
		if (UConfigSubSystem *ConfigSubSystem = UConfigSubSystem::Get(this))
		{
			FString DeviceIP;
			UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"DeviceIP",DeviceIP);
			FString Data;
			FHttpSingleCallBack SingleCallBack;
			SingleCallBack.BindDynamic(this,&ThisClass::GetDeviceListResult);
			HttpSubSystem->HttpGetCallBack(DeviceIP,Data,SingleCallBack);
		}
	}
}

void UDeviceManager::OnSystemStateOnChange(FGameplayTag newState)
{
	if (newState!=CommonToolTags::State_Inspection)
	{
		///OnDeviceInfoPoolRefresh.Broadcast();
	}
	else
	{
		//GetDeviceListByType(CurrentType,CurrentpsrType,DeviceList);
		
		DeviceList.Empty();
		GetSceneAllDevice(DeviceList);
		OnDeviceInfoPoolRefresh.Broadcast();
	}
}

void UDeviceManager::GetDeviceListResult(const FString& JsonStr)
{
	ReInitTrainStation(JsonStr);
}

bool UDeviceManager::GetDeviceData(const FString& ID, FString& JsonStringData)
{
	JsonStringData = DeviceDataMap.FindRef(ID);
	return !JsonStringData.IsEmpty();
}

bool UDeviceManager::GetDeviceAllKeys(const FString& ID, TArray<FString>& DataKeys)
{
	TSharedPtr<FJsonObject> JsonObject = UJsonToolLibrary::GetJsonObjectFromJsonString(DeviceDataMap.FindRef(ID));
	if (JsonObject)
	{
		JsonObject->Values.GetKeys(DataKeys);
		return true;
	}
	return false;
}

void UDeviceManager::GetDevices(const FString& Type,const FString& Type2, TArray<FString>& DeviceIDs)
{
	TArray<FString> DevicesKey;
	DeviceDataMap.GetKeys(DevicesKey);
	TArray<FString> DeviceID;
	for (auto Element : DevicesKey)
	{
		FString typeId;
		FString psrType;
		UJsonToolLibrary::GetStringFromJsonString(DeviceDataMap.FindRef(Element),"typeId",typeId);
		UJsonToolLibrary::GetStringFromJsonString(DeviceDataMap.FindRef(Element),"psrType",psrType);
		if (Type.IsEmpty()&&Type2.IsEmpty())
		{
			DeviceID.AddUnique(Element);
		}
		else if (!Type.IsEmpty()&&!Type2.IsEmpty())
		{
			if (typeId.Equals(Type)&&Type2.Equals(psrType))
			{
				DeviceID.AddUnique(Element);
			}
		}else if (Type.IsEmpty()&&!Type2.IsEmpty())
		{
			if (Type2.Equals(psrType))
			{
				DeviceID.AddUnique(Element);
			}
		}else if (!Type.IsEmpty()&&Type2.IsEmpty())
		{
			if (typeId.Equals(Type))
			{
				DeviceID.AddUnique(Element);
			}
		}
	}
	DeviceIDs = DeviceID;
}

void UDeviceManager::GetDeviceListByType(const FString& Type, const FString& Type2, TArray<FString>& deviceList)
{
	DeviceList.Empty();
	for (auto Element : DeviceDataMap)
	{
		FString typeId;
		FString psrType;
		UJsonToolLibrary::GetStringFromJsonString(Element.Value,"typeId",typeId);
		UJsonToolLibrary::GetStringFromJsonString(Element.Value,"psrType",psrType);
		if (typeId.Equals(Type)&&Type2.Equals(psrType))
		{
			deviceList.AddUnique(Element.Value);
			DeviceList.AddUnique(Element.Value);
		}
	}
	OnDeviceInfoPoolRefresh.Broadcast();
}

void UDeviceManager::GetDeviceTypes(TArray<FString>& Types)
{
	Types = typeIds;
}

int32 UDeviceManager::GetDeviceTypeCount(const FString& type)
{
	int32 Count = 0;

	if (typeCount.Find(type))
	{
		Count = *typeCount.Find(type);
	}
	
	return Count;
}

void UDeviceManager::GetDevicePsrTypes(TArray<FString>& psrTypes)
{
	psrTypes = psrTypeIds;
}

void UDeviceManager::GetDeviceStates(TArray<FString>& states)
{
	states = States;
}

int32 UDeviceManager::GetDeviceStateCount(const FString& state)
{
	int32 Count = 0;

	if (StateCount.Find(state))
	{
		Count = *StateCount.Find(state);
	}
	
	return Count;
}

FString UDeviceManager::GetCurrentFilterType()
{
	return CurrentType;
}

void UDeviceManager::SetCurrentFilterType(const FString& type)
{
	CurrentType = type;
	OnDeviceDataRefresh.Broadcast();
}

FString UDeviceManager::GetCurrentFilterpsrType()
{
	return CurrentpsrType;
}

void UDeviceManager::SetCurrentFilterpsrType(const FString& type)
{
	CurrentpsrType = type;
	OnDeviceDataRefresh.Broadcast();
}

FString UDeviceManager::GetCurrentFilterName()
{
	return FilterName;
}

void UDeviceManager::SetCurrentFilterName(const FString& name)
{
	FilterName = name;
	OnDeviceDataRefresh.Broadcast();
}

void UDeviceManager::OnDeviceDataRefreshFunc()
{
	psrTypeIds.Empty();
	
	TMap<FString, FString> LocalDeviceDataMap = DeviceDataMap;

	
	for (auto Element : DeviceDataMap)
	{
		if (CurrentType.IsEmpty())
		{
			FString typeId;
			UJsonToolLibrary::GetStringFromJsonString(Element.Value,"typeId",typeId);
			CurrentType = typeId;
		}
		
		if (!CurrentType.IsEmpty())
		{
			FString typeId;
			UJsonToolLibrary::GetStringFromJsonString(Element.Value,"typeId",typeId);
			if (!typeId.Equals(CurrentType))
			{
				LocalDeviceDataMap.Remove(Element.Key);
			}
			else
			{
				//
				FString psrType;
				UJsonToolLibrary::GetStringFromJsonString(Element.Value,"psrType",psrType);
				psrTypeIds.AddUnique(psrType);
			}
		}
		else
		{
			//
			FString psrType;
			UJsonToolLibrary::GetStringFromJsonString(Element.Value,"psrType",psrType);
			psrTypeIds.AddUnique(psrType);
		}

		if (!FilterName.IsEmpty())
		{
			FString deviceName;
			UJsonToolLibrary::GetStringFromJsonString(Element.Value,"deviceName",deviceName);
			
			if (!deviceName.Contains(FilterName))
			{
				LocalDeviceDataMap.Remove(Element.Key);
			}
		}
	}

	if (CurrentpsrType.IsEmpty()||!psrTypeIds.Contains(CurrentpsrType))
	{
		CurrentpsrType = psrTypeIds[0];
	}
	
	TMap<FString, FString> TempLocalDeviceDataMap = LocalDeviceDataMap;
	for (auto Element : TempLocalDeviceDataMap)
	{
		if (!CurrentpsrType.IsEmpty())
		{
			FString psrType;
			UJsonToolLibrary::GetStringFromJsonString(Element.Value,"psrType",psrType);
			if (!psrType.Equals(CurrentpsrType))
			{
				LocalDeviceDataMap.Remove(Element.Key);
			}
		}	
	}
	
	DeviceList.Empty();
	DeviceListKey.Empty();
	
	for (auto Element : LocalDeviceDataMap)
	{
		DeviceList.AddUnique(Element.Value);
		
		FString astId;
		UJsonToolLibrary::GetStringFromJsonString(Element.Value,"astId",astId);
		DeviceListKey.AddUnique(astId);
	}
	
	OnDeviceInfoPoolRefresh.Broadcast();

}

void UDeviceManager::GetDeviceList(TArray<FString>& ResultList)
{
	ResultList = DeviceList;
}

void UDeviceManager::GetDeviceListID(TArray<FString>& ResultList)
{
	ResultList = DeviceListKey;
}

void UDeviceManager::GetSceneAllDevice(TArray<FString>& ResultList)
{
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->GetSceneAllDevice(ResultList);
	}
}

void UDeviceManager::FindSceneDevice(const FString& DeviceID, AActor*& Actor)
{
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->FindSceneDevice(DeviceID,Actor);
	}
}

void UDeviceManager::ReInitTrainStation(const FString& Data)
{
	DeviceDataMap.Empty();
	CurrentType = "";
	CurrentpsrType = "";
	typeIds.Empty();
	typeCount.Empty();
	psrTypeIds.Empty();
	DeviceList.Empty();
	DeviceListKey.Empty();
	StateCount.Empty();

	StateCount.Add(TEXT("总数"),0);
	StateCount.Add(TEXT("在运"),0);
	StateCount.Add(TEXT("停运"),0);
	StateCount.Add(TEXT("告警"),0);
	
	//获取到设备列表之后进行处理
	TArray<FString> JsonStringArray;
	UJsonToolLibrary::GetJsonStringArrayFromJsonString(Data,"Data",JsonStringArray);

	StateCount.Add(TEXT("总数"),JsonStringArray.Num());
	for (auto Item : JsonStringArray)
	{
		FString ID;
		UJsonToolLibrary::GetStringFromJsonString(Item,"astId",ID);
		DeviceDataMap.Add(ID,Item);
		
		FString typeId;
		UJsonToolLibrary::GetStringFromJsonString(Item,"typeId",typeId);
		
		if (!typeId.IsEmpty())
		{
			if (typeIds.Contains(typeId))
			{
				int32 Count = *typeCount.Find(typeId);
				typeCount.Add(typeId,Count+1);
			}
			else
			{
				typeIds.AddUnique(typeId);
				typeCount.Add(typeId,1);
			}
		}

		FString State;
		UJsonToolLibrary::GetStringFromJsonString(Item,"state",State);
		if (States.Contains(State))
		{
			int32 Count = *StateCount.Find(State);
			StateCount.Add(State,Count+1);
		}
		else
		{
			States.AddUnique(State);
			StateCount.Add(State,1);
		}
	}
	OnDeviceDataRefresh.Broadcast();
}

void UDeviceManager::InitializeDeviceInfoPool()
{
	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
	{
		if (Settings->bInitializeDeviceInfoPool)
		{
			if (auto SpawnActor= Settings->DeviceInfoActor.LoadSynchronous())
			{
				UWorld* World = GetWorld();
				for (int32 i = 0; i <  Settings->InitializeDeviceInfoPoolSize; i++)
				{
					AActor* Actor = World->SpawnActor<AActor>(SpawnActor, FTransform::Identity);
					DeviceInfoActorPool.Add(Actor);
					Actor->SetActorHiddenInGame(true);
				}
			}
		}
	}
}

AActor* UDeviceManager::PopDeviceInfoPool(const FString& ID,AActor *Device)
{
	AActor* Actor = DeviceInfoActorPool.Pop();
	DeviceInfoActorPoolShowMap.Add(ID,Actor);
	if (ADeviceInfo* DeviceInfo = Cast<ADeviceInfo>(Actor))
	{
		DeviceInfo->SetDeviceKey(ID,Device);
	}
	return Actor;
}

void UDeviceManager::PushDeviceInfoPool(const FString& ID,AActor*Device)
{
	AActor* Actor = *DeviceInfoActorPoolShowMap.Find(ID);
	if (ADeviceInfo* DeviceInfo = Cast<ADeviceInfo>(Actor))
	{
		DeviceInfo->SetDeviceKey("",Device);
	}
	DeviceInfoActorPool.Push(Actor);
	DeviceInfoActorPoolShowMap.Remove(ID);
}
