
// Fill out your copyright notice in the Description page of Project Settings.
//@author piang
//计划用CPP 管理所有UI逻辑实现UI栈
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CommonTool/Library/StructLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIManagerSubsystem.generated.h"

class UFileData;
class UTreeViewFileAction;

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnDeviceInfoReset,const FString&,ID,AActor*,Actor, bool, bAddOrRemove,const FGameplayTag&,Type,UUserWidget*,UserWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDisassembleDevice,USkeletalMeshComponent* ,SkeletalMeshComponen,const TArray<FName>&, DisassembleDeviceData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedDeviceComponent,const FString &,DeviceComponentName);
UCLASS()
class COMMONTOOL_API UUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UUIManagerSubsystem()
	{
		
	}

	static UUIManagerSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;
	
	void ReSetDeviceInfo(const FString& ID,AActor* Actor, bool bAddOrRemove,const FGameplayTag&type,UUserWidget*UserWidget);

	UFUNCTION(BlueprintCallable)
	void DisassembleDevice(USkeletalMeshComponent* SkeletalMeshComponent);

	UFUNCTION(BlueprintCallable)
	void SelectedDeviceComponent(const FString& DeviceComponentName);
public:
	//
	UPROPERTY(BlueprintAssignable)
	FOnDeviceInfoReset OnDeviceInfoReset;
	UPROPERTY(BlueprintAssignable)
	FOnDisassembleDevice OnDisassembleDevice;
	UPROPERTY(BlueprintAssignable)
	FOnSelectedDeviceComponent OnSelectedDeviceComponent;
private:
};
