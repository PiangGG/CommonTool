// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonToolTags.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MessageSubsystem.generated.h"

struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDeviceMessage, FString ,msg,FGameplayTag ,Type);
/**
 * 
 */
UCLASS()
class COMMONTOOL_API UMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UMessageSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	//显示与隐藏加载界面
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Load")
	static void Message(FString msg,FGameplayTag Type);

	UFUNCTION()
	void SendMessage(const FString& msg,FGameplayTag Type) const;

	UPROPERTY(BlueprintAssignable, Category = "CommonLibrary|State")
	FDeviceMessage CurrentDeviceMessage;
};
