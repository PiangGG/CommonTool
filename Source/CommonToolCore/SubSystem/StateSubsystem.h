// Fill out your copyright notice in the Description page of Project Settings.

/*
 * @author piang
 * 状态管理,系统模块状态,用户视角状态
 * 
 */
#pragma once

#include "CoreMinimal.h"
#include "CommonToolCoreTags.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StateSubsystem.generated.h"

/**
 * 
 */
//系统模块切换状态
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSystemChangeSignature, FGameplayTag, State);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDeviceStateChangeSignature, FGameplayTag, State,const FString&, ID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FUserStateChangeSignature, FGameplayTag, State, const FTransform&, Transform,
                                               float, Zoom,bool,bblend);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserStateChangeCompletedSignature, const FTransform&, Transform,
											   float, Zoom);
UCLASS()
class COMMONTOOLCORE_API UStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UStateSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	//主界面模块
	UFUNCTION(BlueprintCallable, Category = "CommonToolCore|State")
	void ChangeSystemState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonToolCore|State")
	void GetSystemState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonToolCore|State")
	FSystemChangeSignature SystemStateOnChange;

	//主界面子模块
	UFUNCTION(BlueprintCallable, Category = "CommonToolCore|State")
	void ChangeChildSystemState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonToolCore|State")
	void GetChildSystemState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonToolCore|State")
	FSystemChangeSignature ChildSystemStateOnChange;

	//玩家状态
	UFUNCTION(BlueprintCallable, Category = "CommonToolCore|State")
	void ChangeUserState(const FGameplayTag& NewState, FTransform Transform, float Zoom,bool bblend = true);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonToolCore|State")
	void GetUserState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonToolCore|State")
	FUserStateChangeSignature UserStateOnChange;
	UPROPERTY(BlueprintAssignable, Category = "CommonToolCore|State")
	FOnUserStateChangeCompletedSignature OnUserStateChangeCompletedSignature;
	//编辑状态
	UFUNCTION(BlueprintCallable, Category = "CommonToolCore|State")
	void ChangeEditState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonToolCore|State")
	void GetEditState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonToolCore|State")
	FSystemChangeSignature EditStateOnChange;

	//设备状态
	UFUNCTION(BlueprintCallable, Category = "CommonToolCore|State")
	void ChangeDeviceState(const FGameplayTag& NewState,const FString& ID);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonToolCore|State")
	void GetDeviceState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonToolCore|State")
	FDeviceStateChangeSignature DeviceStateOnChange;

	//操作状态(带电不带电)
	UFUNCTION(BlueprintCallable, Category = "CommonToolCore|State")
	void ChangeOperationState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonToolCore|State")
	void GetOperationState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonToolCore|State")
	FSystemChangeSignature OperationStateOnChange;
	
	//弹窗状态
	UFUNCTION(BlueprintCallable, Category = "CommonToolCore|State")
	void ChangeDialogState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonToolCore|State")
	void GetDialogState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable)
	FSystemChangeSignature DialogStateChangeSignature;

	UFUNCTION(BlueprintCallable, Category = "CommonToolCore|State")
	void ChangeMarkState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonToolCore|State")
	void GetMarkState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable)
	FSystemChangeSignature MarkStateChangeSignature;

	UFUNCTION(BlueprintCallable, Category = "CommonToolCore|State")
	void ChangeUIState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonToolCore|State")
	void GetUIState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable)
	FSystemChangeSignature UIChangeSignature;
	
private:
	//当前激活模块
	UPROPERTY()
	FGameplayTag CurrentStateTag;

	//当前激活子模块
	UPROPERTY()
	FGameplayTag CurrentChildStateTag;

	//当前用户状态(第一人称,第三人称,全景,聚焦)
	UPROPERTY()
	FGameplayTag CurrentUserStateTag = CommonToolTags::UserState_Null;
	
	//当前编辑状态
	UPROPERTY()
	FGameplayTag CurrentEditTag;

	//当前设备状态
	UPROPERTY()
	FGameplayTag CurrentDeviceStateTag = CommonToolTags::DeviceState_Null;

	UPROPERTY()
	FGameplayTag CurrentOperationStateTag = CommonToolTags::OperationState_1;
	
	//当前弹窗状态
	UPROPERTY()
	FGameplayTag CurrentDialogStateTag;

	//当前标记状态
	UPROPERTY()
	FGameplayTag CurrentMarkStateTag;

	//当前UI状态
	UPROPERTY()
	FGameplayTag CurrentUIStateTag = CommonToolTags::UIState_Show;
};
