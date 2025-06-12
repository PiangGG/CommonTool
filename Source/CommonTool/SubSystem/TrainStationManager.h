// Fill out your copyright notice in the Description page of Project Settings.

/*
 * @author piang
 * 线路切换,站点切换,站点数据管理,站点关卡流数据加载
 */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TrainStationManager.generated.h"

struct FGameplayTag;
/**
 * 
 */
//线路切换
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrainsStationSelected,const FString&,TrainsStation);
//线路上站点切换
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrainStationSelected,const FString&,TrainStation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrainStationSelectedComplete);

UCLASS()
class COMMONTOOL_API UTrainStationManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UTrainStationManager();

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;
	
	//根据数据初始化所有线路
	void IntiTrainStation();

	UPROPERTY()
	FTimerHandle LoadMapsTimerHandle;
	UFUNCTION()
	void LoadMaps(TArray<FString> LoadLevels);

	UFUNCTION()
	void UpdateStreamLevel(const FString& LoadLevel,const TArray<FString>&LoadLevels,const TArray<FString>&AllLoadLevels,FTimerHandle & TimerHandle);
	//选择铁路改变的委托绑定
	UFUNCTION()
	void OnTrainsStationSelected(const FString& TrainsStation);

	//选择铁路上站点改变的委托绑定
	UFUNCTION()
	void OnTrainStationSelected(const FString& TrainStation);
	//线路切换委托
	UPROPERTY(BlueprintAssignable)
	FOnTrainsStationSelected TrainsStationSelected;

	//线路上站点切换委托
	UPROPERTY(BlueprintAssignable)
	FOnTrainStationSelected TrainStationSelected;

	UPROPERTY(BlueprintAssignable)
	FOnTrainStationSelectedComplete OnTrainStationSelectedComplete;
	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag newState);

	UFUNCTION()
	void OnUnLevelLoaded();
	UFUNCTION()
	void OnLevelLoaded();

public:
	
	static UTrainStationManager* Get(const UObject* WorldContextObject);
	
	//添加站点
	/*
	 * @parma TrainStationName 站点名称
	 * @parma TrainStation 站点在场景中实例的actor
	 */
	UFUNCTION()
	void AddTrainStation(const FString &TrainStationName,AActor *TrainStation);

	//切换选择的线路
	/*
	 * @parma TrainsStationName 线路名称:广湛线
	 */
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	void ChangedSelectedTrainsStation(const FString& TrainsStationName);

	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	void ChangedSelectedTrainStation(const FString& TrainStationName);
	
	//获取当前的所有线路
	//@return TrainsStations 返回所有线路名称
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	void GetCurrentTrainsStations(TArray<FString>& TrainsStations);
	
	//获取当前选择铁路数据
	//@parma TrainsStationName 线路名称
	//@return TrainsStationData 线路该线路的数据JsonString
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	bool GetCurrentTrainStationsData(const FString& TrainsStationName, FString& TrainsStationData);

	/*
	 * @return TrainStation 站点
	 */
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	void GetCurrentTrainStation(FString& TrainsStation);
	//根据线路获取当前站点数据
	/*
	 * @parma TrainsStationName 线路名称
	 * @parma TrainStationName 站点名称
	 * @return TrainStationData 站点数据JsonString
	 */
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	bool GeTrainStationData(const FString& TrainsStationName,const FString& TrainStationName, FString& TrainStationData);
	//获取当前站点数据
	/*
	 * @parma TrainStationName 站点名称
	 * @return TrainStationData 站点数据JsonString
	 */
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	bool GetCurrentTrainStationData(const FString& TrainStationName, FString& TrainStationData);
	//获取对应显示的actor
	/*
	 * @parma TrainStationName 站点名称
	 * @return AActor* 站点标记Actor
	 */
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	AActor* GetCurrentTrainStationActor(const FString& TrainStationName);

	//获取线路上的站点
	/*
	 * @parma TrainsStationName 线路名称
	 * @return TrainStationPoints 选路上标记点
	 */
	//UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	bool GetTrainStationPoints(const FString& TrainsStationName,TArray<FVector>& TrainStationPoints);

	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	bool LoadTrainStationFromJson(const FString& TrainStationName);

	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	void ReSetCurrentTrainStationView(bool breturnMain = true);
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	bool GetCurrentTrainStationViewTransform(FTransform& Transform);
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	bool GetTrainsStationData(const FString & TrainsStationName,FString& TrainsStationData);

	//添加点用于全景漫游中铁路小地图
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	void AddTrainStationMarkPosition(const FString& TrainsStationName,FVector Position);

	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	void RemoveTrainStationMarkPosition(const FString& TrainsStationName);
	
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	bool GetTrainStationPoints(TArray<FVector>& TrainStationPoints);

	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	bool GetTrainStationPoint(const FVector& Point);
private:
	//线路数据
	UPROPERTY()
	TMap<FString,FString> TrainsStationMap;
	//站点数据字典
	UPROPERTY()
	TMap<FString,FString> TrainStationMap;
	//站点标记actor
	UPROPERTY()
	TMap<FString,AActor*> TrainStationActorMap;
	//当前选择的线路
	UPROPERTY()
	FString CurrentTrainsStation;
	//当前选择的站点
	UPROPERTY()
	FString CurrentTrainStation;

	//当前站点加载的子关卡
	UPROPERTY()
	TArray<FString> CurrentLoadLevels;

	//站点在三维场景中的位置
	UPROPERTY()
	TMap<FString,FVector> TrainStationMarkPosition;
};