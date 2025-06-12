// Fill out your copyright notice in the Description page of Project Settings.

/*
 * @author piang
 * 状态管理,系统模块状态,用户视角状态
 * 
 */
#pragma once

#include "CoreMinimal.h"
#include "CommonToolTags.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StateSubsystem.generated.h"

class UConfigSubSystem;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSystemChangeSignature, FGameplayTag, State);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FUserStateChangeSignature, FGameplayTag, State, const FTransform&, Transform,
                                               float, Zoom,bool,bblend);

UCLASS()
class COMMONTOOL_API UStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UStateSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "CommonLibrary|State")
	void ChangeSystemState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonLibrary|State")
	void GetSystemState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonLibrary|State")
	FSystemChangeSignature SystemStateOnChange;

	UFUNCTION(BlueprintCallable, Category = "CommonLibrary|State")
	void ChangeChildSystemState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonLibrary|State")
	void GetChildSystemState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonLibrary|State")
	FSystemChangeSignature ChildSystemStateOnChange;
	
	UFUNCTION(BlueprintCallable, Category = "CommonLibrary|State")
	void ChangeUserState(const FGameplayTag& NewState, FTransform Transform, float Zoom,bool bblend = true);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonLibrary|State")
	void GetUserState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonLibrary|State")
	FUserStateChangeSignature UserStateOnChange;

	UFUNCTION(BlueprintCallable, Category = "CommonLibrary|State")
	void ChangeEditState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonLibrary|State")
	void GetEditState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonLibrary|State")
	FSystemChangeSignature EditStateOnChange;

	UFUNCTION(BlueprintCallable, Category = "CommonLibrary|State")
	void ChangeDialogState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonLibrary|State")
	void GetDialogState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonLibrary|State")
	FSystemChangeSignature DialogStateOnChangeSignature;

	UFUNCTION(BlueprintCallable, Category = "CommonLibrary|State")
	void ChangeDeviceState(const FGameplayTag& NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CommonLibrary|State")
	void GetDeviceState(FGameplayTag& CurState);
	UPROPERTY(BlueprintAssignable, Category = "CommonLibrary|State")
	FSystemChangeSignature DeviceStateOnChange;

private:
	UPROPERTY()
	FGameplayTag CurrentSystemStateTag;

	UPROPERTY()
	FGameplayTag CurrentChildStateTag;
	
	UPROPERTY()
	FGameplayTag CurrentUserStateTag;

	UPROPERTY()
	FGameplayTag CurrentEditStateTag;

	UPROPERTY()
	FGameplayTag CurrentDialogStateTag;
	
	UPROPERTY()
	FGameplayTag CurrentDeviceStateTag = CommonToolTags::DeviceState_Null;

	UPROPERTY()
	UConfigSubSystem* ConfigSubSystem;
};
