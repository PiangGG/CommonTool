// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceManager.h"
#include "Device.h"
#include "StateSubsystem.h"
#include "CommonSettings.h"
#include "JsonToolLibrary.h"
#include "ConfigSubSystem.h"
#include "EngineUtils.h"
#include "HttpSubSystem.h"
#include "InteractiveSubsystem.h"
#include "SceneManagerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "CommonToolCore/Actor/Tool/DeviceInfo.h"
#include "CommonToolCore/Components/DeviceMarkComponent.h"
#include "Engine/LevelStreamingDynamic.h"

UDeviceManager::UDeviceManager()
{
}

UDeviceManager* UDeviceManager::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return UWorld::GetSubsystem<UDeviceManager>(GWorld);
	}
	return nullptr;
}

void UDeviceManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	InitializeDevicePOIPool();
	OnDeviceDataRefresh.AddDynamic(this,&ThisClass::OnDeviceDataRefreshFunc);
	OnDevicePOIPoolRefresh.AddDynamic(this,&ThisClass::OnDeviceInfoPoolRefreshFunc);
	OnFocusDeviceChange.AddDynamic(this,&ThisClass::OnFocusDeviceChangeFunc);
	StateSubsystem = UStateSubsystem::Get(this);
	if (StateSubsystem)
	{
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);
		StateSubsystem->DeviceStateOnChange.AddDynamic(this, &ThisClass::OnDeviceStateOnChange);
	}
	
	InteractiveSubsystem = UWorld::GetSubsystem<UInteractiveSubsystem>(GWorld);
	
	SceneManagerSubsystem = USceneManagerSubsystem::Get(this);
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->OnSceneChangeCompleteSignature.AddDynamic(this,&ThisClass::OnSceneChangeComplete);
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

void UDeviceManager::OnSystemStateOnChange(FGameplayTag newState)
{
	if (newState==CommonToolTags::State_DeviceManager)
	{
		GetDeviceListByType(CurrentType,CurrentpsrType,DeviceList);
	}
	else
	{
		if (StateSubsystem)
		{
			SetFocusDevice("");
			//StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Null,FString());
		}
	}
}

void UDeviceManager::OnDeviceStateOnChange(FGameplayTag newState,const FString& ID)
{
	if (newState == CommonToolTags::DeviceState_Ledger)
	{
		if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
		{
			if (LastFocusDevice)
			{
				TArray<UActorComponent*> ActorComponents = LastFocusDevice->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
				for (auto Component : ActorComponents)
				{
					Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(nullptr);
				}
			}

			if (FocusDevice)
			{
				TArray<UActorComponent*> ActorComponents = FocusDevice->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
				for (auto Component : ActorComponents)
				{
					Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(CommonSettings->OverlayMaterial.LoadSynchronous());
				}
			}
			
		}
	}
	else if (newState == CommonToolTags::DeviceState_Null)
	{
		ReSetDeviceInfo(TArray<FString>());
		if (LastFocusDevice)
		{
			TArray<UActorComponent*> ActorComponents = LastFocusDevice->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
			for (auto Component : ActorComponents)
			{
				Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(nullptr);
			}
		}

		if (FocusDevice)
		{
			TArray<UActorComponent*> ActorComponents = FocusDevice->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
			for (auto Component : ActorComponents)
			{
				Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(nullptr);
			}
		}
	}
}

void UDeviceManager::OnSceneChangeComplete()
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
			
			FHttpSingleCallBack SingleCallBack;
			SingleCallBack.BindDynamic(this,&ThisClass::GetDeviceListResult);
			//TODO 数据方式更新为获取该场景数据
			//HttpSubSystem->HttpGetCB(DeviceIP,Data,SingleCallBack);

			if (SceneManagerSubsystem)
			{
				FString JsonSceneData;
				SceneManagerSubsystem->GetCurrentJsonSceneData(JsonSceneData);
				FString stationId;
				UJsonToolLibrary::GetStringFromJsonString(JsonSceneData,TEXT("Id"),stationId);
				FString Data;
				//Data = "stationId="+stationId;
				Data = FString::Printf(TEXT("stationId=%s"), *stationId);
				// TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
				// JsonObject->SetStringField("stationId", stationId);
				// TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Data);
				// FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
				//
				HttpSubSystem->HttpGetCB(DeviceIP,Data,SingleCallBack);
			}
		}
	}
}

void UDeviceManager::RegisterDevice(const FString& GUID,AActor* actor)
{
	if (!GUID.IsEmpty())
	{
		ComponentDeviceActorMap.Add(GUID,actor);
	}
}

void UDeviceManager::UnRegisterDevice(const FString& GUID)
{
	if (!GUID.IsEmpty())
	{
		ComponentDeviceActorMap.Remove(GUID);
	}
}

AActor* UDeviceManager::GetCurrentDisassembleDevice() const
{
	return CurrentDisassembleDevice;
}

void UDeviceManager::GetCurrentDisassembleDeviceType(FString& DeviceType)
{
	DeviceType = CurrentDisassembleDeviceType;
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
	OnDevicePOIPoolRefresh.Broadcast();
}

void UDeviceManager::GetDeviceType(const FString& GUID, FString& Type, FString& Type2)
{
	if (!GUID.IsEmpty() && !DeviceDataMap.FindRef(GUID).IsEmpty())
	{
		FString Element = DeviceDataMap.FindRef(GUID);
		UJsonToolLibrary::GetStringFromJsonString(Element,"typeId",Type);
		UJsonToolLibrary::GetStringFromJsonString(Element,"psrType",Type2);
	}
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
	if (CurrentpsrType != type)
	{
		SetFocusDevice("");
		CurrentpsrType = type;
		OnDeviceDataRefresh.Broadcast();
	}
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
		if (!psrTypeIds.IsEmpty())
		{
			CurrentpsrType = psrTypeIds[0];
		}
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
	OnDevicePOIPoolRefresh.Broadcast();
}

void UDeviceManager::OnDeviceInfoPoolRefreshFunc()
{
	ReSetDeviceInfo(DeviceListKey);
}

void UDeviceManager::OnFocusDeviceChangeFunc(const FString& GUID)
{
	FString Type,Type2;
	GetDeviceType(GUID,Type,Type2);
	CurrentType = Type;
	CurrentpsrType = Type2;
	OnDevicePOIPoolRefresh.Broadcast();
	OnDeviceDataRefresh.Broadcast();
}

void UDeviceManager::GetDeviceList(TArray<FString>& ResultList)
{
	ResultList = DeviceList;
}

void UDeviceManager::GetDeviceListID(TArray<FString>& ResultList)
{
	ResultList = DeviceListKey;
}

void UDeviceManager::InitDevicePOI(const FString& ID,const bool bShow)
{
	if (ComponentDeviceActorMap.Contains(ID))
	{
		FVector Origin;
		FVector BoxExtent;
		if (bShow)
		{
			ComponentDeviceActorMap.FindRef(ID)->GetActorBounds(false, Origin, BoxExtent);
		}
		else
		{
			Origin = FVector(0,0,-100000);
		}
		OnDevicePOIInit.Broadcast(ID,Origin,bShow);
	}
}

void UDeviceManager::DisassembleDevice(ADevice* Device,const FString& Type)
{
	CurrentDisassembleDevice = Device;
	CurrentDisassembleDeviceType = Type;
	if (Type.IsEmpty() && CurrentDisassembleDevice)
	{
		DisassembleTypes.Empty();
		for (auto Element : CurrentDisassembleDevice->DeviceComponentMap)
		{
			if (Element.Value.Name.Equals(Element.Value.Type))
			{
				DisassembleTypes.AddUnique(Element.Value.Type);
			}
		}
	}
	
	OnDeviceDisassemble.Broadcast(Device,Type);
	
	// for (auto Element : Device->DeviceComponentMap)
	// {
	// 	
	// }
	
	// TArray<FName> TempBoneNames = BoneNames;
	// for (auto BoneName : TempBoneNames)
	// {
	// 	if (BoneName.IsEqual("")||BoneName.IsEqual("None")||BoneName.IsEqual("RootNode"))
	// 	{
	// 		BoneNames.Remove(BoneName);
	// 	}
	// }
	// OnDeviceDisassemble.Broadcast(SkeletalMeshComponent,BoneNames);
	// OnDeviceDisassemble.Broadcast(Device,Type);
}

void UDeviceManager::SetSelectedDeviceComponent(const FString& DeviceComponentName)
{
	if (DisassembleTypes.Contains(DeviceComponentName) && CurrentDisassembleDevice)
	{
		DisassembleDevice(CurrentDisassembleDevice,DeviceComponentName);
	}
	else
	{
		CurrentDeviceComponentName = DeviceComponentName;
		OnSelectedDeviceComponent.Broadcast(DeviceComponentName);
	}
}

void UDeviceManager::GetSelectedDeviceComponent(FString& DeviceComponentName)
{
	DeviceComponentName = CurrentDeviceComponentName;
}

void UDeviceManager::ReSetDeviceInfo(const TArray<FString>& DeviceInfos)
{
	for (auto LastComponentDeviceActorKey : LastComponentDeviceActorKeys)
	{
		PushDevicePOIPool(LastComponentDeviceActorKey);
	}
	if (StateSubsystem)
	{
		FGameplayTag GameplayTag;
		StateSubsystem->GetSystemState(GameplayTag);
		if (GameplayTag == CommonToolTags::State_DeviceManager)
		{
			for (auto CurrentComponentDeviceActorKey : DeviceInfos)
			{
					
				PopDevicePOIPool(CurrentComponentDeviceActorKey);
			}
		}
	}
	LastComponentDeviceActorKeys = DeviceInfos;
}

AActor* UDeviceManager::GetDevice(const FString& DeviceID) const
{
	return ComponentDeviceActorMap.FindRef(DeviceID);
}

UUserWidget* UDeviceManager::GetDevicePOI(const FString& DeviceID) const
{
	if (DevicePOIPoolMap.FindRef(DeviceID))
	{
		return DevicePOIPoolMap.FindRef(DeviceID)->GetDevicePOI();
	}
	return nullptr;
}

void UDeviceManager::SetFocusDevice(const FString& DeviceID)
{
	if (InteractiveSubsystem && ComponentDeviceActorMap.FindRef(DeviceID))
	{
		if (DeviceID.IsEmpty())
		{
			if (FocusDevice)
			{
				StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Null,FocusDeviceID);
				LastFocusDevice = FocusDevice;
				FocusDevice = nullptr;
				FocusDeviceID = "";
			}
		}
		else
		{
			if (StateSubsystem)
			{
				FGameplayTag GameplayTag = CommonToolTags::DeviceState_Null;
				StateSubsystem->GetDeviceState(GameplayTag);
				
				if (FocusDevice == ComponentDeviceActorMap.FindRef(DeviceID))
				{
					if (GameplayTag == CommonToolTags::DeviceState_Null)
					{
						StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Ledger,FocusDeviceID);
					}
					else
					{
						StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Null,FocusDeviceID);
					}
				}
				else
				{
					LastFocusDevice = FocusDevice;
					FocusDeviceID = DeviceID;
					FocusDevice = ComponentDeviceActorMap.FindRef(DeviceID);
					StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Ledger,FocusDeviceID);
				}
			}
		}
	}
	else
	{
		if (FocusDevice)
		{
			StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Null,FocusDeviceID);
			LastFocusDevice = FocusDevice;
			FocusDevice = nullptr;
			FocusDeviceID = "";
		}
	}
	OnFocusDeviceChange.Broadcast(FocusDeviceID);
}

FString UDeviceManager::GetFocusDeviceID()
{
	return FocusDeviceID;
}

AActor* UDeviceManager::GetFocusDevice()
{
	return FocusDevice;
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

void UDeviceManager::InitializeDevicePOIPool()
{
	if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
	{
		if (CommonSettings->bInitializeDeviceInfoPool)
		{
			if (auto SpawnActor= CommonSettings->DeviceInfoActor.LoadSynchronous())
			{
				UWorld* World = GetWorld();
				for (int32 i = 0; i <  CommonSettings->InitializeDeviceInfoPoolSize; i++)
				{
					ADeviceInfo* Actor = World->SpawnActor<ADeviceInfo>(SpawnActor, FTransform::Identity);
					DevicePOIPool.Add(Actor);
					Actor->SetActorHiddenInGame(true);
				}
			}
		}
	}
}

AActor* UDeviceManager::PopDevicePOIPool(const FString& ID)
{
	//存在才生成POI 不存在就不生成
	if (!ID.IsEmpty() && !DevicePOIPoolMap.Contains(ID))
	{
		if (ADeviceInfo* DeviceInfo = DevicePOIPool.Pop())
		{
			DevicePOIPoolMap.Add(ID,DeviceInfo);
			DeviceInfo->Pop(ID);
			if (GetDevice(ID))
			{
				InitDevicePOI(ID,true);
			}
			return DeviceInfo;
		}
	}
	return nullptr;
}

bool UDeviceManager::PushDevicePOIPool(const FString& ID)
{
	if (!ID.IsEmpty() && DevicePOIPoolMap.Contains(ID))
	{
		if (ADeviceInfo* DeviceInfo = *DevicePOIPoolMap.Find(ID))
		{
			DeviceInfo->Push(ID);
			InitDevicePOI(ID,false);
			DevicePOIPool.Push(DeviceInfo);
			DevicePOIPoolMap.Remove(ID);
			return true;
		}
	}
	return false;
}
