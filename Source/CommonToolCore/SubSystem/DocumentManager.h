// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DocumentManager.generated.h"

class USceneManagerSubsystem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDocumentDataRefresh,const FString&, stationId ,const FString&, stage, const FString&, type,
                                               const FString&, FilterName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDocumentDataRefreshComplete);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDocumentPOIInit,const FString&,ID,const FVector&,Location,const bool,bShow,UUserWidget* ,UserWidget);
/**
 * 
 */
class UStateSubsystem;

struct FGameplayTag;
UCLASS()
class COMMONTOOLCORE_API UDocumentManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UDocumentManager();

	static UDocumentManager* Get(const UObject* WorldContextObject);
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	void InitializeDocument();

protected:
	UFUNCTION()
	void OnSceneChange(const FString& name);
	UFUNCTION()
	void OnSceneChangeComplete();

	virtual void RegisterDocumentPOI(const FString& GUID,AActor* actor);

	virtual void UnRegisterDocumentPOI(const FString& GUID);

	UFUNCTION()
	void InitDocumentPOI(const FString& ID,const bool bShow,UUserWidget* UserWidget,const FVector& Offsize = FVector::ZeroVector);
	UFUNCTION()
	void ReInitDocumentListResult(const FString& Data);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void GetDocumentList(TArray<FString>& documentList);

	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void GetAlarmInfoList(TArray<FString>& AlarmList);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void SetAlarmInfoType(int32 type);
	
	//根据文档数据获取拥有的类型
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void GetDocumentTypes(TArray<FString>& Types);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void GetCurrentdocType(FString & Type);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CommonToolCore|DeviceManager")
	int32 GetTypedocCount(const FString& type);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void SetCurrentdocType(const FString & Type);

	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void GetStageTypes(TArray<int32>& Types);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CommonToolCore|DeviceManager")
	int32 GeStageTypedocCount(const FString& type);
	
	//根据单位数据获取拥有的类型
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void GetUnitPersonTypes(TArray<FString>& Types);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void GetCurrentunitType(FString & Type);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CommonToolCore|DeviceManager")
	int32 GetTypeunitCount(const FString& type);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void SetCurrentunitType(const FString & Type);
	
	//当前阶段的文档分类
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void GetCurrentStage(FString & Stage);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void SetCurrentStage(const FString & Stage);

	//启用名字模糊搜索
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	FString GetCurrentdocFilterName();
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void SetCurrentdocFilterName(const FString& name);

	//启用名字模糊搜索
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	FString GetCurrentunitFilterName();
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void SetCurrentunitFilterName(const FString& name);
	
	UFUNCTION()
	void ReInitUnitPersonListResult(const FString& Data);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DocumentManager")
	void GetUnitPersonList(TArray<FString>& unitPersonList);
	
	UFUNCTION()
	void OnChildSystemStateOnChange(FGameplayTag State);
	/*
	 * 文档资料
	 */
	UFUNCTION()
	void OnDocumentDataRefreshFunc(const FString& stationId,const FString& stage,const FString&type,const FString& filterName);
	UFUNCTION()
	void GetDocumentListResult(const FString& JsonStr);
	UPROPERTY(BlueprintAssignable)
	FOnDocumentDataRefresh OnDocumentDataRefresh;
	UPROPERTY(BlueprintAssignable)
	FOnDocumentDataRefreshComplete OnDocumentDataRefreshComplete;

	UFUNCTION()
	void OnUnitPersonDataRefreshFunc(const FString& stationId,const FString& stage,const FString&type,const FString& filterName);
	UFUNCTION()
	void GetUnitPersonListResult(const FString& JsonStr);
	UPROPERTY(BlueprintAssignable)
	FOnDocumentDataRefresh OnUnitPersonListDataRefresh;
	UPROPERTY(BlueprintAssignable)
	FOnDocumentDataRefreshComplete OnUnitPersonListRefreshComplete;

	UFUNCTION()
	void OnGetAlarmInfoListResultFunc(const FString& stationId,const FString& stage,const FString&type,const FString& filterName);
	UFUNCTION()
	void GetAlarmInfoListResult(const FString& JsonStr);
	UPROPERTY(BlueprintAssignable)
	FOnDocumentDataRefresh OnAlarmInfoListDataRefresh;
	UPROPERTY(BlueprintAssignable)
	FOnDocumentDataRefreshComplete OnAlarmInfoListRefreshComplete;
	
private:

	friend class UDocumentMarkComponent;
	UPROPERTY()
	USceneManagerSubsystem * SceneManagerSubsystem;
	UPROPERTY()
	UStateSubsystem * StateSubsystem;
	UPROPERTY()
	TMap<FString, FString> DocumentDataMap;
	UPROPERTY()
	TMap<FString, FString> UnitPersonDataMap;
	UPROPERTY()
	TMap<FString, FString>  AlarmInfoDataMap;
	//根据类型分类
	UPROPERTY()
	TArray<FString> documenttypeIds;
	UPROPERTY()
	TArray<FString> unitpersontypeIds;
	UPROPERTY()
	TArray<int32> StagetypeIds;
	UPROPERTY()
	TMap<FString, int32> doctypeCount;
	UPROPERTY()
	TMap<FString, int32> unittypeCount;
	UPROPERTY()
	TMap<int32, int32> StagetypeCount;
	UPROPERTY()
	FString CurrentStage = "";
	UPROPERTY()
	FString CurrentStationId = "";
	UPROPERTY()
	FString CurrentdocType = "";
	UPROPERTY()
	FString CurrentunitType = "";
	UPROPERTY()
	FString docFilterName = "";
	UPROPERTY()
	FString unitFilterName = "";
	UPROPERTY()
	TArray<FString> DocumentList;
	UPROPERTY()
	TArray<FString> UnitPersonList;
	UPROPERTY()
	TArray<FString> AlarmInfoList;

	UPROPERTY()
	TMap<FString,AActor*> DocumentPOIMap;

public:
	UPROPERTY(BlueprintAssignable)
	FOnDocumentPOIInit OnDocumentPOIInit;
};
