// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSubsystem.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneSave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSceneSaveActor,const FString& ,GUID,const TArray<FString>&, Attributes);
UCLASS()
class COMMONTOOLCORE_API USaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static USaveSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void SaveCurrentScene();

	UFUNCTION(BlueprintCallable)
	void SaveImportScenePath(const FString& ScenePath);

	static void GetSceneName(FString ScenePath,FString& Key,int32& start, int32& end);
public:
	FOnSceneSave OnSceneSave;
	
	FOnSceneSaveActor OnSceneSaveActor;
};
