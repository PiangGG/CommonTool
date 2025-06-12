// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DocumentManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDocumentDataRefresh,const FString&,stage,const FString&,type,const FString&,FilterName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDocumentDataRefreshComplete);
/**
 * 
 */
class UTrainStationManager;
class UStateSubsystem;

struct FGameplayTag;
UCLASS()
class COMMONTOOL_API UDocumentManager : public UWorldSubsystem
{
	GENERATED_BODY()

	UDocumentManager();

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	UFUNCTION()
	void OnTrainStationSelected(const FString& TrainStation);
	
	UFUNCTION()
	void ReInitDocumentListResult(const FString& Data);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void GetDocumentList(TArray<FString>& documentList);
	
	//根据文档数据获取拥有的类型
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void GetDocumentTypes(TArray<FString>& Types);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void GetCurrentdocType(FString & Type);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CommonLibrary|DeviceManager")
	int32 GetTypedocCount(const FString& type);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void SetCurrentdocType(const FString & Type);

	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void GetStageTypes(TArray<int32>& Types);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CommonLibrary|DeviceManager")
	int32 GeStageTypedocCount(const FString& type);
	
	//根据单位数据获取拥有的类型
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void GetUnitPersonTypes(TArray<FString>& Types);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void GetCurrentunitType(FString & Type);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CommonLibrary|DeviceManager")
	int32 GetTypeunitCount(const FString& type);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void SetCurrentunitType(const FString & Type);
	
	//当前阶段的文档分类
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void GetCurrentStage(FString & Stage);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void SetCurrentStage(const FString & Stage);

	//启用名字模糊搜索
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	FString GetCurrentdocFilterName();
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void SetCurrentdocFilterName(const FString& name);

	//启用名字模糊搜索
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	FString GetCurrentunitFilterName();
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void SetCurrentunitFilterName(const FString& name);
	
	UFUNCTION()
	void ReInitUnitPersonListResult(const FString& Data);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DocumentManager")
	void GetUnitPersonList(TArray<FString>& unitPersonList);
	
	UFUNCTION()
	void OnChildSystemStateOnChange(FGameplayTag State);
	/*
	 * 文档资料
	 */
	UFUNCTION()
	void OnDocumentDataRefreshFunc(const FString& stage,const FString&type,const FString& filterName);
	UFUNCTION()
	void GetDocumentListResult(const FString& JsonStr);
	UPROPERTY(BlueprintAssignable)
	FOnDocumentDataRefresh OnDocumentDataRefresh;
	UPROPERTY(BlueprintAssignable)
	FOnDocumentDataRefreshComplete OnDocumentDataRefreshComplete;

	UFUNCTION()
	void OnUnitPersonDataRefreshFunc(const FString& stage,const FString&type,const FString& filterName);
	UFUNCTION()
	void GetUnitPersonListResult(const FString& JsonStr);
	UPROPERTY(BlueprintAssignable)
	FOnDocumentDataRefresh OnUnitPersonListDataRefresh;
	UPROPERTY(BlueprintAssignable)
	FOnDocumentDataRefreshComplete OnUnitPersonListRefreshComplete;
	
private:
	UPROPERTY()
	UTrainStationManager * TrainStationManager;
	UPROPERTY()
	UStateSubsystem * StateSubsystem;
	UPROPERTY()
	TMap<FString, FString> DocumentDataMap;
	UPROPERTY()
	TMap<FString, FString> UnitPersonDataMap;
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
	FString CurrentStage;
	
	UPROPERTY()
	FString CurrentdocType;
	UPROPERTY()
	FString CurrentunitType;
	UPROPERTY()
	FString docFilterName = "";
	UPROPERTY()
	FString unitFilterName = "";
	UPROPERTY()
	TArray<FString> DocumentList;
	UPROPERTY()
	TArray<FString> UnitPersonList;
};
