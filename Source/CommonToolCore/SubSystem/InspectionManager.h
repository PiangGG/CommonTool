// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InspectionManager.generated.h"

class UStateSubsystem;
struct FGameplayTag;
class UConfigSubSystem;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInspectionCheckSignature, bool, InspectionStarted, const FTransform&, Transform);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInspectionTransformSignature, const FTransform&, Transform);

UCLASS()
class COMMONTOOLCORE_API UInspectionManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UInspectionManager();

	static UInspectionManager* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION()
	void InspectionCheck(bool bInspection,const FTransform& Transform);
	
	UFUNCTION()
	void OnConfigChanged(const FString& ConfigString);
	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag State);
	UFUNCTION()
	void OnUserStateChangeCompletedSignature(const FTransform& Transform, float Zoom);
	/*
	 *自动巡检相关
	 * @param InspectionIndex 当前巡检到的设备的索引
	 * @param bReStart 是否重新开始
	 * @param InspectionCheckSignature 点击自动巡检触发委托
	 */
	//Inspection
	void RegisterActor(AActor* Actor);
	
	void UnRegisterActor(AActor* Actor);

	void RegisterLineActor(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Inspection")
	void StartInspection(bool bReStart = true);
	UFUNCTION()
	void InspectionIng();
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Inspection")
	void StopInspection();
	UFUNCTION(BlueprintPure, Category="CommonToolCore|Inspection")
	bool GetInspectionStarted();
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Inspection")
	void StartInspectionLine(bool bReStart = true);
	UFUNCTION()
	void InspectionIngLine();
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Inspection")
	void StopInspectionLine();
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Inspection")
	void SetInspectionLineSpeed(float speed);
	//是否重新还是巡检
	UPROPERTY(BlueprintReadOnly)
	bool bInspectionStarted = false;
	//自动巡检切换委托
	UPROPERTY(BlueprintAssignable)
	FInspectionCheckSignature InspectionCheckSignature;
	//==========
	UPROPERTY(BlueprintAssignable)
	FInspectionTransformSignature InspectionTransformSignature;

	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Inspection")
	void GetInspectionLineLocationAndDirection(const FVector& BaseLocation,FVector & ResultLocation,FVector&ResultDirection);
	
	void DebugSplineInfo();
private:
	UPROPERTY()
	UConfigSubSystem* ConfigSubSystem;

	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	UPROPERTY()
	FTimerHandle InspectionTimerHandle;
	//当前巡检设备索引
	UPROPERTY()
	int32 InspectionIndex = 0;
	//巡检速率
	UPROPERTY()
	int32 InspectionRate = 2;
	//需要巡检的actor包括静态和动态的
	UPROPERTY()
	TArray<AActor*> InspectionActors;
	UPROPERTY()
	AActor* InspectionLineActor;
	UPROPERTY()
	float CurrentProgress = 0.0f;
	UPROPERTY()
	float MoveSpeed = 3.0f;
	UPROPERTY()
	FVector InspectionLocation;
};
