// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonToolCore/Library/StructLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InteractiveSubsystem.generated.h"

class UDeviceManager;
/**
 * 
 */
class UStateSubsystem;
class USceneManagerSubsystem;
class USaveSubsystem;
class UGameplayTask_Interactive;

struct FGameplayTag;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractiveActorSignature, AActor*, Actor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractiveActorRoamSignature, const FHitResult&,HitResult );

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FForceDeviceSignature,const FString&,DeviceName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractiveInfo, FString, Info);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeSectionItem ,const FEditItem& ,Item);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetDeviceDataResultSignature,const FString& ,DataResult);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointerSignature,UPrimitiveComponent*,Component);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPointerLineTraceSignature,UPrimitiveComponent*,Component,const FVector&,RayStart,const FVector&,RayEnd);

UCLASS()
class COMMONTOOLCORE_API UInteractiveSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UInteractiveSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Interactive")
	AActor* LineTraceActor(FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Interactive")
	AActor* GetInteractiveActor();
protected:
	UFUNCTION()
	void OnUserStateOnChange(FGameplayTag newState, const FTransform& Transform, float Zoom,bool bblend);
	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag newState);
	UFUNCTION()
	void OnDeviceStateOnChange(FGameplayTag newState,const FString& ID);
	
	UFUNCTION()
	void OnInteractiveActor( AActor* Actor);
	UFUNCTION()
	void OnInteractiveRoamActor(const FHitResult& HitResult);
	UFUNCTION()
	void InteractiveRoamOnClicked(AActor* Actor,const FVector& HitLocation);
	UFUNCTION(BlueprintCallable)
	void InteractiveSimulate();
	UFUNCTION()
	void InteractiveOnClickedOption(AActor* Actor);
	UFUNCTION()
	void BroadCastInfo(AActor* actor);

	UFUNCTION()
	void GetPointerRay(FVector& RayStart, FVector& RayEnd);

	UFUNCTION(BlueprintCallable)
	void SetInteractiveActor();
	UFUNCTION(BlueprintCallable)
	void ClearInteractiveActor();

	UFUNCTION()
	void InteractiveRoam();

	UFUNCTION()
	void InteractiveDevice();
	
	UFUNCTION()
	void InteractiveInspection();
	UFUNCTION(BlueprintCallable)
	void InteractiveOption(AActor* Actor = nullptr);
	UFUNCTION()
	void OnPressLeftPointer();
	UFUNCTION()
	void OnReleaseLeftPointer();
	UFUNCTION()
	void OnPressRightPointer();
	UFUNCTION()
	void OnReleaseRightPointer();
	UFUNCTION()
	void StartQueryInteractables();
	UFUNCTION()
	void EndQueryInteractables();
	UFUNCTION()
	void QueryInteractables();

	UFUNCTION()
	void DragPointer();
	UFUNCTION()
	void UpdatePointerHover();

	UFUNCTION()
	bool IsActorInteractable(AActor* Actor);
	UFUNCTION()
	bool RaycastTest(FHitResult &HitResult);
	UFUNCTION()
	void OverlapTest(UPrimitiveComponent* OutComponent);

	UFUNCTION()
	bool GetIsDragging() const;

	//編輯頁面相關
	//左邊
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Interactive")
	void SetSectionItem(FEditItem editItem);
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Interactive")
	FEditItem GetSectionItem(bool& HasSection) const;
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Interactive")
	void OnSectionChange(const FEditItem& Item);

	/*
	 * 对actor进行增删改查
	 * TODO 缺少增
	 */
	void SpawnActor(TSoftClassPtr<AActor> actor);
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Interactive")
	void SpawnActor(FEditItem ActorEditItem);
	//当前actor生成完成时候触发,比如线和面多次点击触发
	UFUNCTION()
	void OnActorInitializationCompleted(UObject* Object);
	
	//设置聚焦到actor
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Interactive")
	void FocusActor(AActor *  actor);
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Interactive")
	//编辑actor
	void EditActor(AActor * actor);
	//删除actor
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Interactive")
	void DeleteActor(AActor * actor);
	

	//设置选中材质
	UFUNCTION()
	void SetDeviceMaterial(AActor * device);
	
	void GetDeviceDatabyID(const FString&  DeviceID,const FGameplayTag& newState);
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|SceneManager")
	void ReturnCurrentSceneView(bool breturnMain = true);
	UFUNCTION()
	void OnGetDeviceDataResult(const FString& JsonStr);
	
public:
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FOnInteractiveActorSignature InteractiveActorSignature;
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FOnInteractiveActorRoamSignature InteractiveActorRoamSignature;
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FOnInteractiveActorSignature InteractiveOptionSignature;
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FInteractiveInfo InteractiveInfo;
	// UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Device")
	// FInteractiveOnClickedDisassembleDeviceSignature InteractiveDisassembleDeviceSignature;
	UPROPERTY(BlueprintAssignable)
	FGetDeviceDataResultSignature GetDeviceDataResultSignature;
	//UI中选择改变的委托
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FOnChangeSectionItem OnChangeSectionItem;

	//Gizmo
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FOnPointerSignature OnPointerEnter;
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FOnPointerSignature OnPointerMove;
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FOnPointerSignature OnPointerLeave;
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FOnPointerSignature OnPointerUp;
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FOnPointerLineTraceSignature OnPointerDrag;
	UPROPERTY(BlueprintAssignable, Category="CommonToolCore|Interactive")
	FOnPointerLineTraceSignature OnPointerDown;
private:
	UPROPERTY()
	FVector2D LeftMousePosition;
	UPROPERTY()
	FVector2D RightMousePosition;
	UPROPERTY()
	AActor* InteractiveActor;
	UPROPERTY()
	UPrimitiveComponent* HitComponent;
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	UPROPERTY()
	USceneManagerSubsystem*SceneManagerSubsystem;
	UPROPERTY()
	USaveSubsystem *SaveSubsystem;
	UPROPERTY()
	UDeviceManager*DeviceManager;
	UPROPERTY()
	FTimerHandle SetInfoTimer;
	UPROPERTY()
	FInteractionSettings InteractionSettings;
	UPROPERTY()
	float InteractionScanRate = 0.1;
	UPROPERTY()
	FTimerHandle QueryTimerHandle;
	UPROPERTY()
	bool bIsDragging = false;
	UPROPERTY()
	FHitResult SavedHitResult;
	UPROPERTY()
	FEditItem EditItem;
	UPROPERTY()
	bool bSelectedItem = false;
	friend class ACommonCharacter;
	friend class USceneManagerSubsystem;
};
