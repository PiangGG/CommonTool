// Fill out your copyright notice in the Description page of Project Settings.
//设备管理的单例类子系统

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DeviceManager.generated.h"

struct FGameplayTagContainer;
class USceneManagerSubsystem;
class UHttpSubSystem;
class URegionManager;
class UStateSubsystem;

struct FGameplayTag;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeviceDatasRefresh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeviceInfoPoolRefresh);

/**
 * 
 */
UCLASS()
class COMMONTOOL_API UDeviceManager  : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UDeviceManager();

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	UFUNCTION()
	void OnTrainStationSelected(const FString& TrainStation);

	UFUNCTION()
	void GetDeviceListResult(const FString& JsonStr);

	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag newState);
	/*
	 * param ID 设备ID
	 * return JsonStringData 返回设备数据
	 */
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	bool GetDeviceData(const FString& ID ,FString & JsonStringData);

	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	bool GetDeviceAllKeys(const FString& ID ,TArray<FString>& DataKeys);
	//根据类型获取设备数组
	/*
	 * Type 设备类型字段
	 * DeviceIDs 返回一个设备ID数组
	 */
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void GetDevices(const FString& Type,const FString& Type2,TArray<FString>& DeviceIDs);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void GetDeviceListByType(const FString& Type,const FString& Type2,TArray<FString>& deviceList);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void GetDeviceTypes(TArray<FString>& Types);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CommonLibrary|DeviceManager")
	int32 GetDeviceTypeCount(const FString& type);
	UFUNCTION(BlueprintCallable)
	void GetDevicePsrTypes(TArray<FString>& psrTypes);

	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void GetDeviceStates(TArray<FString>& states);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CommonLibrary|DeviceManager")
	int32 GetDeviceStateCount(const FString& state);

	UFUNCTION()
	void OnDeviceDataRefreshFunc();
	UPROPERTY(BlueprintAssignable)
	FOnDeviceDatasRefresh OnDeviceDataRefresh;
	UPROPERTY(BlueprintAssignable)
	FOnDeviceInfoPoolRefresh OnDeviceInfoPoolRefresh;
	//过滤当前已选择的类型
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	FString GetCurrentFilterType();
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void SetCurrentFilterType(const FString& type);

	//过滤当前已选择的类型
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	FString GetCurrentFilterpsrType();
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void SetCurrentFilterpsrType(const FString& type);
	//启用名字模糊搜索
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	FString GetCurrentFilterName();
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void SetCurrentFilterName(const FString& name);
	
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void GetDeviceList(TArray<FString>& ResultList);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void GetDeviceListID(TArray<FString>& ResultList);

	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void GetSceneAllDevice(TArray<FString>& ResultList);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void FindSceneDevice(const FString& DeviceID, AActor*& Actor);
	
	/*
	 * 设备信息对象池管理
	 */
	// 初始化设备信息对象池
	void InitializeDeviceInfoPool();
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	AActor* PopDeviceInfoPool(const FString& ID,AActor*Device);
	UFUNCTION(BlueprintCallable,Category="CommonLibrary|DeviceManager")
	void PushDeviceInfoPool(const FString& ID,AActor*Device);

protected:
	void ReInitTrainStation(const FString& Data);
private:
	UPROPERTY()
	URegionManager * RegionManager;
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	UPROPERTY()
	USceneManagerSubsystem * SceneManagerSubsystem;

	/*
	 * <ID,Data>
	 * @param id 设备在数据库的唯一ID
	 * @param Data 通过ID在数据库查找到的Json String数据
	 */
	UPROPERTY()
	TMap<FString, FString> DeviceDataMap;
	
	//当前设备分类 数组对选分类 筛选
	UPROPERTY()
	FGameplayTagContainer CurrentTypeContainer;
	
	//筛选名字
	UPROPERTY()
	FString FilterName;

	//初始化设备展示对象池数组
	UPROPERTY()
	TArray<AActor*> DeviceInfoActorPool;
	
	//当前需要显示设备信息的设备信息Map
	UPROPERTY()
	TMap<FString,AActor*> DeviceInfoActorPoolShowMap; 
};
