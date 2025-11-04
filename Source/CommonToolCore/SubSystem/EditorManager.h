// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EditorManager.generated.h"

class ACommonGizmoTransform;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class COMMONTOOLCORE_API UEditorManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UEditorManager();

	static UEditorManager* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag State);
	
	//TODO 改成世界开始时候生成
	UFUNCTION()
	void InitializeScene();

	//设置编辑模式
	UFUNCTION(BlueprintCallable)
	void SetTransformMode(FGameplayTag InTransformMode, bool CombineTranslateRotate);

	//设置编辑空间位置
	UFUNCTION(BlueprintCallable)
	void SetAlignSpace(FGameplayTag InAlignSpace);

};
