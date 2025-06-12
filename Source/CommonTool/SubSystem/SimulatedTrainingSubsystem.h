// Fill out your copyright notice in the Description page of Project Settings.

/*
 * @author piang
 * 模拟培训子模块
 * 目标通用模拟培训
 */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SimulatedTrainingSubsystem.generated.h"

class USimulatedTrainingQueueStep;
/**
 *通过监听与设备交互操作,来判断当前逻辑
 *与设备交互 设备广播步骤
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSimulatedTrainingQueueComplete,bool, bComplete);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSimulatedTrainingQueueStateChange, FString, SimulatedTrainingNodeName,
                                             bool, bComplete,const FOnSimulatedTrainingQueueComplete&,SingleCallBack);
UCLASS()
class COMMONTOOL_API USimulatedTrainingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USimulatedTrainingSubsystem()
	{
	}

	static USimulatedTrainingSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	/*
	 * @param SimulatedFlows 操作了数组
	 * @param SimulatedTrainingNodeName 当前操作流程K
	 */
	//初始化需要完成的任务步骤
	UFUNCTION(BlueprintCallable, Category = "CommonLibrary:SimulatedTrainingSubsystem")
	void ExecuteSimulatedTraining(const TArray<FString>& SimulatedFlows,const FOnSimulatedTrainingQueueComplete& SingleCallBack);
	//开始下一个流程从0开始
	UFUNCTION(BlueprintCallable, Category = "CommonLibrary:SimulatedTrainingSubsystem")
	void StartNextSimulatedTraining();
	//移除当前流程 开始下一个流程前调用
	UFUNCTION(BlueprintCallable, Category = "CommonLibrary:SimulatedTrainingSubsystem")
	void RemovePreviousSimulatedTraining();
	//尝试完成当前流程
	UFUNCTION(BlueprintCallable, Category = "CommonLibrary:SimulatedTrainingSubsystem")
	void CompleteCurrentSimulatedTraining(FString SimulatedTrainingNodeName);
	//结束整个流程
	UFUNCTION(BlueprintCallable, Category = "CommonLibrary:SimulatedTrainingSubsystem")
	void EndSimulatedTraining();

	UFUNCTION(BlueprintCallable)
	USimulatedTrainingQueueStep* GetCurrentSimulatedTrainingQueueStep();
	UFUNCTION()
	void OnSimulatedTrainingQueue(FString SimulatedTrainingNodeName);

	UPROPERTY()
	TArray<USimulatedTrainingQueueStep*> SimulatedTrainingQueueSteps;

	UPROPERTY()
	bool bSimulatedTrainingQueueStep = false;

	//整个任务流程完成或失败的委托
	UPROPERTY(BlueprintAssignable)
	FOnSimulatedTrainingQueueStateChange OnSimulatedTrainingQueueStateChangeCallBack;

	FOnSimulatedTrainingQueueComplete CurrentOnSimulatedTrainingQueueComplete;
	UFUNCTION()
	void OnSimulatedTrainingQueueStateChange(FString SimulatedTrainingNodeName, bool bComplete, const FOnSimulatedTrainingQueueComplete& SingleCallBack);
};
