// Fill out your copyright notice in the Description page of Project Settings.


#include "DocumentManager.h"

#include "CommonToolCoreTags.h"
#include "ConfigSubSystem.h"
#include "HttpSubSystem.h"
#include "SceneManagerSubsystem.h"
#include "StateSubsystem.h"
#include "CommonToolCore/Library/JsonToolLibrary.h"

UDocumentManager::UDocumentManager()
{
	
}

UDocumentManager* UDocumentManager::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return UWorld::GetSubsystem<UDocumentManager>(GWorld);
	}
	return nullptr;
}

void UDocumentManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	OnDocumentDataRefresh.AddDynamic(this,&ThisClass::OnDocumentDataRefreshFunc);
	OnUnitPersonListDataRefresh.AddDynamic(this,&ThisClass::OnUnitPersonDataRefreshFunc);
	OnAlarmInfoListDataRefresh.AddDynamic(this,&ThisClass::OnGetAlarmInfoListResultFunc);
	
	SceneManagerSubsystem = USceneManagerSubsystem::Get(this);
	
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->OnSceneChangeSignature.AddDynamic(this,&ThisClass::OnSceneChange);
		SceneManagerSubsystem->OnSceneChangeCompleteSignature.AddDynamic(this,&ThisClass::OnSceneChangeComplete);
	}
	StateSubsystem = UStateSubsystem::Get(this);
	if (StateSubsystem)
	{
		StateSubsystem->ChildSystemStateOnChange.AddDynamic(this, &ThisClass::OnChildSystemStateOnChange);
	}

	InitializeDocument();
}

void UDocumentManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDocumentManager::Deinitialize()
{
	Super::Deinitialize();
}

void UDocumentManager::InitializeDocument()
{
	if (UHttpSubSystem* HttpSubSystem = UHttpSubSystem::Get(this))
	{
		if (UConfigSubSystem *ConfigSubSystem = UConfigSubSystem::Get(this))
		{
			//文档数据
			{
				FString DocumentDataURL;
				UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"DocumentData",DocumentDataURL);
				FString Data;
				FHttpSingleCallBack SingleCallBack;
				SingleCallBack.BindDynamic(this,&ThisClass::GetDocumentListResult);
				HttpSubSystem->HttpGetCB(DocumentDataURL,Data,SingleCallBack);
			}
			//单位人员数据
			{
				FString UnitPersonDataURL;
				UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"UnitPersonData",UnitPersonDataURL);
				FString Data;
				FHttpSingleCallBack SingleCallBack;
				SingleCallBack.BindDynamic(this,&ThisClass::GetUnitPersonListResult);
				HttpSubSystem->HttpGetCB(UnitPersonDataURL,Data,SingleCallBack);
			}
			//告警数据
			{
				FString AlarmInfoURL;
				UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"AlarmInfo",AlarmInfoURL);
				FString Data;
				FHttpSingleCallBack SingleCallBack;
				SingleCallBack.BindDynamic(this,&ThisClass::GetAlarmInfoListResult);
				HttpSubSystem->HttpGetCB(AlarmInfoURL,Data,SingleCallBack);
			}
		}
	}
}

void UDocumentManager::OnSceneChange(const FString& name)
{
	for (auto Element : DocumentPOIMap)
	{
		
	}
}

void UDocumentManager::OnSceneChangeComplete()
{
	if (SceneManagerSubsystem)
	{
		FString JsonSceneData;
		SceneManagerSubsystem->GetCurrentJsonSceneData(JsonSceneData);
		UJsonToolLibrary::GetStringFromJsonString(JsonSceneData,TEXT("Id"),CurrentStationId);
	}
	CurrentStage.Empty();
	CurrentdocType.Empty();
	docFilterName.Empty();
	OnDocumentDataRefresh.Broadcast(CurrentStationId,CurrentStage,CurrentdocType,docFilterName);
	// if (UHttpSubSystem* HttpSubSystem = UHttpSubSystem::Get(this))
	// {
	// 	if (UConfigSubSystem *ConfigSubSystem = UConfigSubSystem::Get(this))
	// 	{
	// 		//文档数据
	// 		{
	// 			FString DocumentDataURL;
	// 			UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"DocumentData",DocumentDataURL);
	// 			FString Data;
	// 			FHttpSingleCallBack SingleCallBack;
	// 			
	// 			SingleCallBack.BindDynamic(this,&ThisClass::GetDocumentListResult);
	// 			HttpSubSystem->HttpGetCB(DocumentDataURL,Data,SingleCallBack);
	// 		}
	// 		//单位人员数据
	// 		{
	// 			FString UnitPersonDataURL;
	// 			UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"UnitPersonData",UnitPersonDataURL);
	// 			FString Data;
	// 			FHttpSingleCallBack SingleCallBack;
	// 			SingleCallBack.BindDynamic(this,&ThisClass::GetUnitPersonListResult);
	// 			HttpSubSystem->HttpGetCB(UnitPersonDataURL,Data,SingleCallBack);
	// 		}
	// 		//告警数据
	// 		{
	// 			FString AlarmInfoURL;
	// 			UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"AlarmInfo",AlarmInfoURL);
	// 			FString Data;
	// 			FHttpSingleCallBack SingleCallBack;
	// 			SingleCallBack.BindDynamic(this,&ThisClass::GetAlarmInfoListResult);
	// 			HttpSubSystem->HttpGetCB(AlarmInfoURL,Data,SingleCallBack);
	// 		}
	// 	}
	// }
}

void UDocumentManager::RegisterDocumentPOI(const FString& GUID, AActor* actor)
{
	if (!DocumentPOIMap.Contains(GUID))
	{
		DocumentPOIMap.Add(GUID,actor);
	}
}

void UDocumentManager::UnRegisterDocumentPOI(const FString& GUID)
{
	if (DocumentPOIMap.Contains(GUID))
	{
		DocumentPOIMap.Remove(GUID);
	}
}

void UDocumentManager::InitDocumentPOI(const FString& ID, const bool bShow, UUserWidget* UserWidget,
	const FVector& Offsize)
{
	if (UserWidget && DocumentPOIMap.Contains(ID))
	{
		FVector Origin;
		if (bShow)
		{
			Origin = DocumentPOIMap.FindRef(ID)->GetActorLocation();
		}
		else
		{
			Origin = FVector(0,0,-100000);
		}
		OnDocumentPOIInit.Broadcast(ID,Origin+Offsize,bShow,UserWidget);
	}
}

void UDocumentManager::ReInitDocumentListResult(const FString& Data)
{
	//获取到文档列表之后进行处理
	TArray<FString> JsonStringArray;
	UJsonToolLibrary::GetJsonStringArrayFromJsonString(Data,"Data",JsonStringArray);

	DocumentDataMap.Empty();
	StagetypeIds.Empty();
	StagetypeCount.Empty();
	
	for (auto Item : JsonStringArray)
	{
		FString ID;
		UJsonToolLibrary::GetStringFromJsonString(Item,"id",ID);
		DocumentDataMap.Add(ID,Item);

		int32 affiliationStageId;
		UJsonToolLibrary::GetInteger32FromJsonString(Item,"affiliationStageId",affiliationStageId);
		
		if (StagetypeIds.Contains(affiliationStageId))
		{
			int32 Count = *StagetypeCount.Find(affiliationStageId);
			StagetypeCount.Add(affiliationStageId,Count+1);
		}
		else
		{
			StagetypeIds.AddUnique(affiliationStageId);
			StagetypeCount.Add(affiliationStageId,1);
		}
	}
	OnDocumentDataRefresh.Broadcast(CurrentStationId,CurrentStage,CurrentdocType,docFilterName);
}

void UDocumentManager::GetDocumentList(TArray<FString>& documentList)
{
	documentList = DocumentList;
}

void UDocumentManager::GetAlarmInfoList(TArray<FString>& AlarmList)
{
	AlarmList = AlarmInfoList;
}

void UDocumentManager::SetAlarmInfoType(int32 type)
{
	OnAlarmInfoListDataRefresh.Broadcast("","",FString::FromInt(type),"");
}

void UDocumentManager::GetDocumentTypes(TArray<FString>& Types)
{
	Types = documenttypeIds;
}

void UDocumentManager::GetCurrentdocType(FString& Type)
{
	Type = CurrentdocType;
}

int32 UDocumentManager::GetTypedocCount(const FString& type)
{
	int32 Count = 0;

	if (doctypeCount.Find(type))
	{
		Count = *doctypeCount.Find(type);
	}
	
	return Count;
}
void UDocumentManager::SetCurrentdocType(const FString& Type)
{
	CurrentdocType = Type;
	OnDocumentDataRefresh.Broadcast(CurrentStationId,CurrentStage,CurrentdocType,docFilterName);
}

void UDocumentManager::GetUnitPersonTypes(TArray<FString>& Types)
{
	Types = unitpersontypeIds;
}

void UDocumentManager::GetCurrentunitType(FString& Type)
{
	CurrentunitType = Type;
}

int32 UDocumentManager::GetTypeunitCount(const FString& type)
{
	int32 Count = 0;

	if (unittypeCount.Contains(type))
	{
		Count = *unittypeCount.Find(type);
	}
	
	return Count;
}

void UDocumentManager::GetStageTypes(TArray<int32>& Types)
{
	Types = StagetypeIds;
}

int32 UDocumentManager::GeStageTypedocCount(const FString& type)
{
	int32 Count = 0;
	int32 FindType = 1; 
	LexFromString(FindType, *type);
	if (StagetypeIds.Contains(FindType))
	{
		Count = *StagetypeCount.Find(FindType);
	}
	return Count;
}
void UDocumentManager::SetCurrentunitType(const FString& Type)
{
	CurrentunitType = Type;
	OnUnitPersonListDataRefresh.Broadcast(CurrentStationId,CurrentStage,CurrentunitType,unitFilterName);
}

void UDocumentManager::GetCurrentStage(FString& Stage)
{
	Stage = CurrentStage;
}

void UDocumentManager::SetCurrentStage(const FString& Stage)
{
	CurrentStage = Stage;
	OnDocumentDataRefresh.Broadcast(CurrentStationId,CurrentStage,CurrentdocType,docFilterName);
	OnUnitPersonListDataRefresh.Broadcast(CurrentStationId,CurrentStage,CurrentunitType,unitFilterName);
}

FString UDocumentManager::GetCurrentdocFilterName()
{
	return docFilterName;
}

void UDocumentManager::SetCurrentdocFilterName(const FString& name)
{
	docFilterName = name;
	OnDocumentDataRefresh.Broadcast(CurrentStationId,CurrentStage,CurrentdocType,docFilterName);
}

FString UDocumentManager::GetCurrentunitFilterName()
{
	return unitFilterName;
}

void UDocumentManager::SetCurrentunitFilterName(const FString& name)
{
	unitFilterName = name;
	OnUnitPersonListDataRefresh.Broadcast(CurrentStationId,CurrentStage,CurrentunitType,unitFilterName);
}

void UDocumentManager::ReInitUnitPersonListResult(const FString& Data)
{
	//获取到人员列表之后进行处理
	TArray<FString> JsonStringArray;
	UJsonToolLibrary::GetJsonStringArrayFromJsonString(Data,"Data",JsonStringArray);

	UnitPersonDataMap.Empty();
	
	for (auto Item : JsonStringArray)
	{
		FString ID;
		UJsonToolLibrary::GetStringFromJsonString(Item,"id",ID);
		UnitPersonDataMap.Add(ID,Item);
	}
}

void UDocumentManager::GetUnitPersonList(TArray<FString>& unitPersonList)
{
	unitPersonList = UnitPersonList;
}

void UDocumentManager::OnChildSystemStateOnChange(FGameplayTag State)
{
	if (State == CommonToolTags::State_DigitalArchives_Design)
	{
		SetCurrentStage(FString("1"));
	}
	else if (State == CommonToolTags::State_DigitalArchives_Build)
	{
		SetCurrentStage(FString("2"));
	}else if (State == CommonToolTags::State_DigitalArchives_CheckAndAccept)
	{
		SetCurrentStage(FString("3"));
	}else if (State == CommonToolTags::State_DigitalArchives_Operation)
	{
		SetCurrentStage(FString("4"));
	}
}

void UDocumentManager::OnDocumentDataRefreshFunc(const FString& stationId,const FString& stage,const FString&type,const FString& filterName)
{
	DocumentList.Empty();
	documenttypeIds.Empty();
	doctypeCount.Empty();

	//stationId
	TMap<FString, FString> TempDocumentDataMap;

	for (auto Element : DocumentDataMap)
	{
		FString StationId;
		UJsonToolLibrary::GetStringFromJsonString(Element.Value,"stationId",StationId);
		
		if (stationId.Equals(StationId)||StationId.IsEmpty()||stationId.IsEmpty())
		{
			TempDocumentDataMap.Add(Element);
		}
	}
	
	if (!stage.IsEmpty())
	{
		TMap<FString, FString> NewTempDocumentDataMap = TempDocumentDataMap;
		TempDocumentDataMap.Empty();
		for (auto Element : NewTempDocumentDataMap)
		{
			int32 affiliationStageId;
			UJsonToolLibrary::GetInteger32FromJsonString(Element.Value,"affiliationStageId",affiliationStageId);
		
			if (stage.Equals(FString::FromInt(affiliationStageId)))
			{
				TempDocumentDataMap.Add(Element);
			}
		}
	}
	else
	{
		
	}

	for (auto Element : TempDocumentDataMap)
	{
		FString fileType = "";
		UJsonToolLibrary::GetStringFromJsonString(Element.Value,"fileType",fileType);
		
		if (documenttypeIds.Contains(fileType))
		{
			int32 Count = *doctypeCount.Find(fileType);
			doctypeCount.Add(fileType,Count+1);
		}
		else
		{
			documenttypeIds.AddUnique(fileType);
			doctypeCount.Add(fileType,1);
		}
	}
	
	if (documenttypeIds.Contains(type))
	{
		
	}
	else
	{
		if (documenttypeIds.Num()>0 && !stationId.IsEmpty())
		{
			CurrentdocType = documenttypeIds[0];
		}
	}
	
	for (auto Element : TempDocumentDataMap)
	{
		FString fileName;
		UJsonToolLibrary::GetStringFromJsonString(Element.Value,"fileName",fileName);

		FString fileType = "";
		UJsonToolLibrary::GetStringFromJsonString(Element.Value,"fileType",fileType);
		
		if (filterName.IsEmpty())
		{
			if (stationId.IsEmpty())
			{
				DocumentList.AddUnique(Element.Value);
			}
			else
			{
				if (fileType.Equals(CurrentdocType))
				{
					DocumentList.AddUnique(Element.Value);
				}
			}
		}
		else
		{
			if (stationId.IsEmpty())
			{
				if (fileName.Contains(filterName))
				{
					DocumentList.AddUnique(Element.Value);
				}
			}
			else
			{
				if (fileName.Contains(filterName)&&fileType.Equals(CurrentdocType))
				{
					DocumentList.AddUnique(Element.Value);
				}
			}
		}
	}
	
	OnDocumentDataRefreshComplete.Broadcast();
}

void UDocumentManager::GetDocumentListResult(const FString& JsonStr)
{
	ReInitDocumentListResult(JsonStr);
}

void UDocumentManager::OnUnitPersonDataRefreshFunc(const FString& stationId,const FString& stage, const FString& type, const FString& filterName)
{
	UnitPersonList.Empty();
	unitpersontypeIds.Empty();
	unittypeCount.Empty();
	
	TMap<FString, FString> TempDocumentDataMap;
	for (auto Element : UnitPersonDataMap)
	{
		int32 affiliationStageId;
		UJsonToolLibrary::GetInteger32FromJsonString(Element.Value,"affiliationStageId",affiliationStageId);
		
		if (stage.Equals(FString::FromInt(affiliationStageId)))
		{
			TempDocumentDataMap.Add(Element);
		}
	}
	
	for (auto Element : TempDocumentDataMap)
	{
		FString deptName = "";
		UJsonToolLibrary::GetStringFromJsonString(Element.Value,"deptName",deptName);
		
		if (unitpersontypeIds.Contains(deptName))
		{
			int32 Count = *unittypeCount.Find(deptName);
			unittypeCount.Add(deptName,Count+1);
		}
		else
		{
			unitpersontypeIds.AddUnique(deptName);
			unittypeCount.Add(deptName,1);
		}
	}
	
	if (unitpersontypeIds.Contains(type))
	{
		
	}
	else
	{
		if (unitpersontypeIds.Num()>0)
		{
			CurrentunitType = unitpersontypeIds[0];
		}
	}
	
	for (auto Element : TempDocumentDataMap)
	{
		FString personName;
		UJsonToolLibrary::GetStringFromJsonString(Element.Value,"personName",personName);

		FString deptName = "";
		UJsonToolLibrary::GetStringFromJsonString(Element.Value,"deptName",deptName);
		
		if (filterName.IsEmpty())
		{
			if (deptName.Equals(CurrentunitType))
			{
				UnitPersonList.AddUnique(Element.Value);
			}
		}
		else
		{
			if (personName.Contains(filterName)&&deptName.Equals(CurrentunitType))
			{
				UnitPersonList.AddUnique(Element.Value);
			}
		}
	}
	
	OnUnitPersonListRefreshComplete.Broadcast();
}

void UDocumentManager::GetUnitPersonListResult(const FString& JsonStr)
{
	ReInitUnitPersonListResult(JsonStr);
}

void UDocumentManager::OnGetAlarmInfoListResultFunc(const FString& stationId,const FString& stage, const FString& type,
	const FString& filterName)
{
	AlarmInfoList.Empty();
	
	for (auto Element : AlarmInfoDataMap)
	{
		int32 Alarmtypeid;
		UJsonToolLibrary::GetInteger32FromJsonString(Element.Value,"alarmType",Alarmtypeid);
		if (type.Equals(FString::FromInt(Alarmtypeid)))
		{
			AlarmInfoList.AddUnique(Element.Value);
		}
	}
	OnAlarmInfoListRefreshComplete.Broadcast();
}

void UDocumentManager::GetAlarmInfoListResult(const FString& JsonStr)
{
	TArray<FString> JsonStringArray;
	UJsonToolLibrary::GetJsonStringArrayFromJsonString(JsonStr,"Data",JsonStringArray);

	AlarmInfoDataMap.Empty();
	
	for (auto Item : JsonStringArray)
	{
		FString ID;
		UJsonToolLibrary::GetStringFromJsonString(Item,"id",ID);
		AlarmInfoDataMap.Add(ID,Item);
	}
}

