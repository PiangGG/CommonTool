// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "GizmoManager.generated.h"

class UStateSubsystem;
class UInteractiveSubsystem;
struct FGameplayTag;
class ACommonGizmoTransform;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeTransformMode,FGameplayTag ,InTransformMode, bool, CombineTranslateRotate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeAlignSpace,FGameplayTag ,AlignSpace);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivateGizmo, AActor*, TransformActor);
UCLASS()
class COMMONTOOLGIZMO_API UGizmoManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UGizmoManager();

	static UGizmoManager* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	//TODO 改成世界开始时候生成
	UFUNCTION()
	void InitializeScene();

	//设置编辑模式
	UFUNCTION(BlueprintCallable)
	void SetTransformMode(FGameplayTag InTransformMode, bool CombineTranslateRotate);

	//设置编辑空间位置
	UFUNCTION(BlueprintCallable)
	void SetAlignSpace(FGameplayTag InAlignSpace);
protected:
	UFUNCTION()
	void OnInteractiveActor(AActor* Actor);
	UFUNCTION()
	void OnPointerEnter(UPrimitiveComponent* Component);
	UFUNCTION()
	void OnPointerMove(UPrimitiveComponent* Component);
	UFUNCTION()
	void OnPointerLeave(UPrimitiveComponent* Component);
	UFUNCTION()
	void OnPointerUp(UPrimitiveComponent* Component);
	UFUNCTION()
	void OnPointerDrag(UPrimitiveComponent*Component,const FVector&RayStart,const FVector&RayEnd);
	UFUNCTION()
	void OnPointerDown(UPrimitiveComponent*Component,const FVector&RayStart,const FVector&RayEnd);
public:
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Gizmo")	
	FOnChangeTransformMode OnChangeTransformMode;
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Gizmo")	
	FOnChangeAlignSpace OnChangeAlignSpace;
	UPROPERTY(BlueprintAssignable)
	FOnActivateGizmo OnActivateGizmo;
private:
	UPROPERTY()
	UInteractiveSubsystem* InteractiveSubsystem;
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	//编辑actorTransform的交互actor
	UPROPERTY()
	ACommonGizmoTransform* CommonGizmoTransform;
};

