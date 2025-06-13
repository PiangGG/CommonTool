// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RegionManager.generated.h"

class UStateSubsystem;
/*
 *区域管理子系统
 * @author piang
 * 区域切换,区域数据管理,区域关卡流数据加载
 */
//线路切换
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRegionChangingSignature,const FString&,Region);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRegionChangingCompleteSignature);

UCLASS()
class COMMONTOOL_API URegionManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static URegionManager* Get(const UObject* WorldContextObject);
	/*
	 * @return Region 获取当前区域
	 */
	UFUNCTION(BlueprintCallable,Category="RegionManager")
	void GetCurrentRegion(FString& Region);
	//获取当前区域数据
	/*
	 * @parma RegionName 区域名称
	 * @return RegionData 区域数据JsonString
	 * @return 是否获取成功
	 */
	UFUNCTION(BlueprintCallable,Category="RegionManager")
	bool GetRegionData(const FString& RegionName, FString& RegionData);
	//获取对应显示的actor
	/*
	 * @parma TrainStationName 站点名称
	 * @return AActor* 站点标记Actor
	 */
	// UFUNCTION(BlueprintCallable,Category="RegionManager")
	// AActor* GetCurrentRegionActor(const FString& Region);
	UFUNCTION(BlueprintCallable,Category="RegionManager")
	bool LoadRegionFromJson(const FString& Region);

	//返回区域视角
	UFUNCTION(BlueprintCallable,Category="RegionManager")
	void ReturnCurrentRegionView();
	
	UFUNCTION(BlueprintCallable,Category="RegionManager")
	void ChangedRegion(const FString& Region);
	//区域切换关卡加载
	UPROPERTY()
	FTimerHandle LoadMapsTimerHandle;
	UFUNCTION()
	void LoadMaps(TArray<FString> LoadLevels);
	UFUNCTION()
	void UpdateStreamLevel(const FString& LoadLevel,const TArray<FString>&LoadLevels,const TArray<FString>&AllLoadLevels,FTimerHandle & TimerHandle);
	//线路切换委托
	UPROPERTY(BlueprintAssignable)
	FOnRegionChangingSignature OnRegionChangingSignature;
	
	UPROPERTY(BlueprintAssignable)
	FOnRegionChangingCompleteSignature OnRegionChangingCompleteSignature;

protected:
	URegionManager();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;
	//根据数据初始化所有线路
	void IntiAllRegion();
	UFUNCTION()
	void OnUnLevelLoaded();
	UFUNCTION()
	void OnLevelLoaded();
	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag newState);
	//区域改变的委托绑定
	UFUNCTION()
	void OnRegionChanging(const FString& Region);
	//区域改变的委托绑定执行成功
	UFUNCTION()
	void OnRegionChangingComplete();
private:
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	//区域数据键值对
	UPROPERTY()
	TMap<FString,FString> RegionDataMap;
	//区域数标记actor
	UPROPERTY()
	TMap<FString,AActor*> RegionActorMap;
	//当前选择的站点
	UPROPERTY()
	FString CurrentRegion;
	//当前区域加载的子关卡
	UPROPERTY()
	TArray<FString> CurrentLoadLevels;
};
