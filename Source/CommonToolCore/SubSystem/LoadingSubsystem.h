// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LoadingSubsystem.generated.h"

/**
 * Load界面操作的子系统
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadScreenShowChange, bool, bShow);

UCLASS()
class COMMONTOOLCORE_API ULoadingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static ULoadingSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	//显示与隐藏加载界面
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Load")
	static void ShowLoadScreen(bool bShow);

	UFUNCTION()
	bool GetLoading()
	{
		return Loading;
	}

	UFUNCTION()
	void SetLoading(bool bLoad);

	UPROPERTY(BlueprintAssignable, Category = "CommonToolCore|State")
	FOnLoadScreenShowChange OnLoadScreenShowChange;

private:
	UPROPERTY()
	bool Loading = false;
	//UPROPERTY()
	static int32 num;
};
