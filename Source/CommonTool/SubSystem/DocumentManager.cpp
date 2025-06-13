// Fill out your copyright notice in the Description page of Project Settings.


#include "DocumentManager.h"

#include "CommonToolTags.h"
#include "ConfigSubSystem.h"
#include "HttpSubSystem.h"
#include "LoadingSubsystem.h"
#include "StateSubsystem.h"
#include "RegionManager.h"
#include "CommonTool/Library/JsonToolLibrary.h"

UDocumentManager::UDocumentManager()
{
	
}

void UDocumentManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	OnDocumentDataRefresh.AddDynamic(this,&ThisClass::OnDocumentDataRefreshFunc);
	OnUnitPersonListDataRefresh.AddDynamic(this,&ThisClass::UDocumentManager::OnUnitPersonDataRefreshFunc);
	
	RegionManager = URegionManager::Get(this);
	
	if (RegionManager)
	{
		RegionManager->TrainStationSelected.AddDynamic(this,&ThisClass::OnTrainStationSelected);
	}
	StateSubsystem = UStateSubsystem::Get(this);
	if (StateSubsystem)
	{
		StateSubsystem->ChildSystemStateOnChange.AddDynamic(this, &ThisClass::OnChildSystemStateOnChange);
	}
}

void UDocumentManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDocumentManager::Deinitialize()
{
	Super::Deinitialize();
}

void UDocumentManager::OnTrainStationSelected(const FString& TrainStation)
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
				HttpSubSystem->HttpGetCallBack(DocumentDataURL,Data,SingleCallBack);
			}
			//单位人员数据
			{
				FString UnitPersonDataURL;
				UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"UnitPersonData",UnitPersonDataURL);
				FString Data;
				FHttpSingleCallBack SingleCallBack;
				SingleCallBack.BindDynamic(this,&ThisClass::GetUnitPersonListResult);
				HttpSubSystem->HttpGetCallBack(UnitPersonDataURL,Data,SingleCallBack);
			}
		}
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
}

void UDocumentManager::GetDocumentList(TArray<FString>& documentList)
{
	documentList = DocumentList;
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
	OnDocumentDataRefresh.Broadcast(CurrentStage,CurrentdocType,docFilterName);
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
	OnUnitPersonListDataRefresh.Broadcast(CurrentStage,CurrentunitType,unitFilterName);
}

void UDocumentManager::GetCurrentStage(FString& Stage)
{
	Stage = CurrentStage;
}

void UDocumentManager::SetCurrentStage(const FString& Stage)
{
	CurrentStage = Stage;
	OnDocumentDataRefresh.Broadcast(CurrentStage,CurrentdocType,docFilterName);
	OnUnitPersonListDataRefresh.Broadcast(CurrentStage,CurrentunitType,unitFilterName);
}

FString UDocumentManager::GetCurrentdocFilterName()
{
	return docFilterName;
}

void UDocumentManager::SetCurrentdocFilterName(const FString& name)
{
	docFilterName = name;
	OnDocumentDataRefresh.Broadcast(CurrentStage,CurrentdocType,docFilterName);
}

FString UDocumentManager::GetCurrentunitFilterName()
{
	return unitFilterName;
}

void UDocumentManager::SetCurrentunitFilterName(const FString& name)
{
	unitFilterName = name;
	OnUnitPersonListDataRefresh.Broadcast(CurrentStage,CurrentunitType,unitFilterName);
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
	}
}

void UDocumentManager::OnDocumentDataRefreshFunc(const FString& stage,const FString&type,const FString& filterName)
{
	DocumentList.Empty();
	documenttypeIds.Empty();
	doctypeCount.Empty();
	
	TMap<FString, FString> TempDocumentDataMap;
	for (auto Element : DocumentDataMap)
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
		if (documenttypeIds.Num()>0)
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
			if (fileType.Equals(CurrentdocType))
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
	
	OnDocumentDataRefreshComplete.Broadcast();
}

void UDocumentManager::GetDocumentListResult(const FString& JsonStr)
{
	ReInitDocumentListResult(JsonStr);
}

void UDocumentManager::OnUnitPersonDataRefreshFunc(const FString& stage, const FString& type, const FString& filterName)
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

