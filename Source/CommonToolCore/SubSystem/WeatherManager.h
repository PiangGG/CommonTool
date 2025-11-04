// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonToolCoreTags.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "WeatherManager.generated.h"

/**
 * 
 */
//天气
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeatherChangeSignature, FGameplayTag, State);
//时间
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimeChangeSignature, float, time);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSimulateCheckSignature, bool, bSimulateCheck);

UCLASS()
class COMMONTOOLCORE_API UWeatherManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UWeatherManager* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	UFUNCTION(BlueprintCallable, Category = "CommonLibrary|Weather")
	void ChangeWeatherState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonLibrary|Weather")
	void GetWeatherState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonLibrary|Weather")
	FWeatherChangeSignature WeatherChangeSignature;

	UFUNCTION(BlueprintCallable, Category = "CommonLibrary|Time")
	void ChangeTime(const float& time);
	UFUNCTION(BlueprintPure, BlueprintPure, Category = "CommonLibrary|Time")
	float GetTime();
	UPROPERTY(BlueprintAssignable, Category = "CommonLibrary|Time")
	FTimeChangeSignature TimeChangeSignature;

	UFUNCTION(BlueprintCallable, Category = "CommonLibrary|Weather")
	void SetSimulateCheck(const bool& Simulate);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonLibrary|Weather")
	bool GetSimulateCheck();
	UPROPERTY(BlueprintAssignable, Category = "CommonLibrary|Weather")
	FSimulateCheckSignature SimulateCheckSignature;

	UFUNCTION()
	void OnSimulateCheck(bool SimulateCheck);
	UFUNCTION()
	void GetWeatherDataResult(const FString& JsonStr);
	UFUNCTION(BlueprintPure, Category = "CommonLibrary|Weather")
	void GetWeatherData(FString& JsonStr);
private:
	UPROPERTY()
	FGameplayTag CurrentWeatherStateTag = CommonToolTags::Weather_Cloudy;

	UPROPERTY()
	float CurrentTime = 9.0f;

	UPROPERTY()
	bool bSimulate = true;

	UPROPERTY()
	FString WeatherData;
};


