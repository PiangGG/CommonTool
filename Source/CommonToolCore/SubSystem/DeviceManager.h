// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DeviceManager.generated.h"

class ADevice;
class ULevelStreamingDynamic;
class USceneManagerSubsystem;
class ADeviceInfo;
class UInteractiveSubsystem;
class UHttpSubSystem;
class UStateSubsystem;

struct FGameplayTag;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeviceDataRefresh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDevicePOIPoolRefresh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDevicePOIInit,const FString&,ID,const FVector&,Location,const bool,bShow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeviceDisassemble,ADevice*, Device,const FString& ,Type);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedDeviceComponent,const FString &,DeviceComponentName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusDeviceChange,const FString &,GUID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDisassembleDeviceSignature, const FHitResult&, HitResult);
/**
 * 
 */
UCLASS()
class COMMONTOOLCORE_API UDeviceManager  : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UDeviceManager();

	static UDeviceManager* Get(const UObject* WorldContextObject);
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	//UFUNCTION(BlueprintCallable, Category="DeviceManager")
	UFUNCTION()
	void GetDeviceListResult(const FString& JsonStr);
	/*
	 * param ID 设备ID
	 * return JsonStringData 返回设备数据
	 */
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	bool GetDeviceData(const FString& ID ,FString & JsonStringData);

	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	bool GetDeviceAllKeys(const FString& ID ,TArray<FString>& DataKeys);
	//根据类型获取设备数组
	/*
	 * Type 设备类型字段
	 * DeviceIDs 返回一个设备ID数组
	 */
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void GetDevices(const FString& Type,const FString& Type2,TArray<FString>& DeviceIDs);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void GetDeviceListByType(const FString& Type,const FString& Type2,TArray<FString>& deviceList);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void GetDeviceType(const FString& GUID,FString& Type,FString& Type2);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void GetDeviceTypes(TArray<FString>& Types);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CommonToolCore|DeviceManager")
	int32 GetDeviceTypeCount(const FString& type);
	UFUNCTION(BlueprintCallable)
	void GetDevicePsrTypes(TArray<FString>& psrTypes);

	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void GetDeviceStates(TArray<FString>& states);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CommonToolCore|DeviceManager")
	int32 GetDeviceStateCount(const FString& state);

	UFUNCTION()
	void OnDeviceDataRefreshFunc();
	UFUNCTION()
	void OnDeviceInfoPoolRefreshFunc();
	UFUNCTION()
	void OnFocusDeviceChangeFunc(const FString &GUID);
	UPROPERTY(BlueprintAssignable)
	FOnDeviceDataRefresh OnDeviceDataRefresh;
	UPROPERTY(BlueprintAssignable)
	FOnDevicePOIPoolRefresh OnDevicePOIPoolRefresh;
	//过滤当前已选择的类型
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	FString GetCurrentFilterType();
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void SetCurrentFilterType(const FString& type);

	//过滤当前已选择的类型
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	FString GetCurrentFilterpsrType();
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void SetCurrentFilterpsrType(const FString& type);
	//启用名字模糊搜索
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	FString GetCurrentFilterName();
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void SetCurrentFilterName(const FString& name);
	
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void GetDeviceList(TArray<FString>& ResultList);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void GetDeviceListID(TArray<FString>& ResultList);
	/*
	 * 设备信息对象池管理
	 */
	// 初始化对象池
	void InitializeDevicePOIPool();
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	AActor* PopDevicePOIPool(const FString& ID);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	bool PushDevicePOIPool(const FString& ID);

	//设备POI与拆解相关
	//------------------------
	UPROPERTY(BlueprintAssignable)
	FOnDevicePOIInit OnDevicePOIInit;
	UPROPERTY(BlueprintAssignable)
	FOnDeviceDisassemble OnDeviceDisassemble;
	UPROPERTY(BlueprintAssignable)
	FOnSelectedDeviceComponent OnSelectedDeviceComponent;
	void InitDevicePOI(const FString& ID,const bool bShow);
	UFUNCTION(BlueprintCallable)
	void DisassembleDevice(ADevice* Device,const FString& Type);
	UFUNCTION(BlueprintCallable)
	void SetSelectedDeviceComponent(const FString& DeviceComponentName);
	UFUNCTION(BlueprintCallable)
	void GetSelectedDeviceComponent(FString& DeviceComponentName);
	//------------------------

	UFUNCTION()
	void ReSetDeviceInfo(const TArray<FString>& DeviceInfos);
	
	UFUNCTION(BlueprintPure,Category="CommonToolCore|DeviceManager")
	AActor* GetDevice(const FString& DeviceID) const;
	UFUNCTION(BlueprintPure,Category="CommonToolCore|DeviceManager")
	UUserWidget* GetDevicePOI(const FString& DeviceID) const;
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|DeviceManager")
	void SetFocusDevice(const FString& DeviceID);
	//获取当前聚焦设备ID
	UFUNCTION(BlueprintPure,Category="CommonToolCore|DeviceManager")
	FString GetFocusDeviceID();
	//获取当前聚焦设备
	UFUNCTION(BlueprintPure,Category="CommonToolCore|DeviceManager")
	AActor* GetFocusDevice();
	UPROPERTY(BlueprintAssignable)
	FOnFocusDeviceChange OnFocusDeviceChange;
	
	virtual void RegisterDevice(const FString& GUID,AActor* actor);
	
	virtual void UnRegisterDevice(const FString& GUID);

	UFUNCTION(BlueprintPure,Category="CommonToolCore|DeviceManager")
	AActor* GetCurrentDisassembleDevice() const;
	UFUNCTION(BlueprintPure,Category="CommonToolCore|DeviceManager")
	void GetCurrentDisassembleDeviceType(FString&  DeviceType);
protected:
	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag newState);
	UFUNCTION()
	void OnDeviceStateOnChange(FGameplayTag newState,const FString& ID);

	UFUNCTION()
	void OnSceneChangeComplete();
	
	void ReInitTrainStation(const FString& Data);

	UPROPERTY()
	const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>();
private:
	UPROPERTY()
	UInteractiveSubsystem* InteractiveSubsystem;
	
	UPROPERTY()
	USceneManagerSubsystem* SceneManagerSubsystem;
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	
	FDelegateHandle ActorSpawnedDelegateHandle;
	
	FDelegateHandle ActorDestroyedDelegateHandle;
	UPROPERTY()
	TMap<FString, FString> DeviceDataMap;
	UPROPERTY()
	AActor* FocusDevice;
	UPROPERTY()
	AActor* LastFocusDevice;
	UPROPERTY()
	FString FocusDeviceID;
	//台账选项UI
	UPROPERTY()
	UUserWidget* UserWidget;
	//根据类型分类
	UPROPERTY()
	TArray<FString> typeIds;
	UPROPERTY()
	FString CurrentType;
	UPROPERTY()
	FString CurrentpsrType;
	UPROPERTY()
	FString FilterName;
	UPROPERTY()
	TMap<FString, int32> typeCount;
	UPROPERTY()
	TArray<FString> psrTypeIds;

	//根据状态分类
	UPROPERTY()
	TArray<FString> States;
	UPROPERTY()
	TMap<FString, int32> StateCount;
	UPROPERTY()
	TArray<FString> DeviceList;
	UPROPERTY()
	TArray<FString> DeviceListKey;

	UPROPERTY()
	TArray<FString> LastComponentDeviceActorKeys;

	//测试用的使用组件挂载的方式
	UPROPERTY()
	TMap<FString,AActor*> ComponentDeviceActorMap;

	//设备POI对象池
	UPROPERTY()
	TArray<ADeviceInfo*> DevicePOIPool;
	//当前需要显示设备信息的设备信息Map
	UPROPERTY()
	TMap<FString,ADeviceInfo*> DevicePOIPoolMap;
	UPROPERTY()
	ADevice* CurrentDisassembleDevice;
	UPROPERTY()
	FString CurrentDeviceComponentName;

	UPROPERTY()
	TArray<FString> DisassembleTypes;

	UPROPERTY()
	FString CurrentDisassembleDeviceType;
};