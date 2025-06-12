// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTool/Library/StructLibrary.h"
#include "CommonTool/RuntimeGizemo/CommonGizmoHandle.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InteractiveSubsystem.generated.h"

/**
 * 
 */
class UStateSubsystem;
class USceneManagerSubsystem;
class USaveSubsystem;
class UTrainStationManager;
class UGameplayTask_Interactive;

struct FGameplayTag;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractiveOnClickedSignature, AActor*, Device);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractiveOnClickedDisassembleDeviceSignature, const FHitResult&, HitResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FForceDeviceSignature,const FString&,DeviceName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractiveInfo, FString, Info);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivateGizmo, AActor*, TransformActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTransformModeChange,FGameplayTag ,InTransformMode, bool, CombineTranslateRotate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlignSpaceChange,FGameplayTag ,AlignSpace);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUISectionChange ,const FEditItem& ,myItem, bool, bSelected);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetDeviceDataResultSignature,const FString& ,DataResult);
UCLASS()
class COMMONTOOL_API UInteractiveSubsystem : public UWorldSubsystem
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
	
	UFUNCTION()
	void OnUserStateOnChange(FGameplayTag newState, const FTransform& Transform, float Zoom,bool bblend);

	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag newState);
	
	UFUNCTION()
	void OnDeviceStateOnChange(FGameplayTag newState);
	
	UFUNCTION()
	void DeviceOnClick(AActor* Device, FKey ButtonPressed);

	UFUNCTION(BlueprintCallable)
	AActor* LineTraceActor();
	UFUNCTION()
	void InteractiveOnClicked(AActor* Actor);
	UFUNCTION()
	void InteractiveRoamOnClicked(AActor* Actor);
	UFUNCTION(BlueprintCallable)
	void InteractiveSimulate();
	UFUNCTION()
	void InteractiveOnClickedOption(AActor* Actor);
	UFUNCTION()
	void InteractiveOnClickedEdit(AActor* Actor);
	UFUNCTION()
	void CleanCurrentInteractiveActor();
	UFUNCTION()
	void BroadCastInfo(AActor* actor);

	UFUNCTION()
	void GetPointerRay(FVector& RayStart, FVector& RayEnd);

	UFUNCTION(BlueprintCallable)
	void Interactive();
	UFUNCTION(BlueprintCallable)
	void DeInteractive();

	UFUNCTION()
	void InteractiveRoam();

	UFUNCTION()
	void InteractiveDisassembleDevice();
	
	UFUNCTION()
	void SetInteractiveActor(AActor* Actor);
	UFUNCTION()
	void InteractiveInspection();
	UFUNCTION(BlueprintCallable)
	void InteractiveOption(AActor* Actor = nullptr);
	UFUNCTION()
	void DeInteractiveOption()const;
	UFUNCTION()
	void PressPointer();

	UFUNCTION()
	void ReleasePointer();
	UFUNCTION()
	void SetLockHover(bool LockHover);

	UFUNCTION()
	void OnSelectionActor(bool bSelection,AActor* SelectionActor);
	
	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Device")
	FInteractiveOnClickedSignature InteractiveOnClickedSignature;
	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Device")
	FInteractiveOnClickedSignature InteractiveRoamSignature;
	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Device")
	FInteractiveOnClickedSignature InteractiveRoamDevice;
	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Device")
	FInteractiveInfo InteractiveInfo;
	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Device")
	FInteractiveOnClickedSignature InteractiveOptionSignature;
	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Device")
	FInteractiveOnClickedSignature InteractiveEditSignature;

	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Device")
	FInteractiveOnClickedDisassembleDeviceSignature InteractiveDisassembleDeviceSignature;
	UFUNCTION()
	void DisableInteraction();
	
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
	void OverlapTest(AActor* OutActor,UPrimitiveComponent* OutComponent);

	UFUNCTION()
	bool GetIsDragging() const;

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void SetTransformMode(FGameplayTag InTransformMode, bool CombineTranslateRotate);
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void SetAlignSpace(FGameplayTag AlignSpace);

	//UI中选择改变的委托
	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|SpawnAsset")
	FOnUISectionChange OnUISectionChange;
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SpawnAsset")
	void SetCurrentSectionItem(FEditItem editItem, bool bSelected);
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SpawnAsset")
	void UISectionChange(const FEditItem& Item, bool bSelected);
	UFUNCTION(BlueprintPure, Category="CommonLibrary|Interactive")
	AActor* GetSelectedActor() const;
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SpawnAsset")
	bool GetUIIsSelected() const;

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	void EditForceActor(AActor *  actor = nullptr);
	
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	void EditEditActor(AActor * actor = nullptr);
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	void DeleteEditActor();

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	AActor* GetEditActor();

	//设置选中材质
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	void ForceDeviceSetMaterial(AActor * device,bool SetOrCancel);
	//获取聚焦材质id
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	void ForceDeviceByID(const FString&  DeviceID);
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	void GetForceDeviceID(FString&  DeviceID);
	UFUNCTION(BlueprintCallable,BlueprintPure, Category="CommonLibrary|SceneManager")
	bool IsCurrentForceDevice(const FString&  DeviceID);
	//设置聚焦到actor
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	void ForceActor(AActor *  Actor);

	void GetDeviceDatabyID(const FString&  DeviceID,const FGameplayTag& newState);
	
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	void ReturnRootView();
	
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	void ReturnCurrentSceneView(bool breturnMain = true);

	UFUNCTION()
	void OnGetDeviceDataResult(const FString& JsonStr);

	UPROPERTY(BlueprintAssignable)
	FGetDeviceDataResultSignature GetDeviceDataResultSignature;
private:
	UPROPERTY()
	AActor* CurrentInteractiveActor;

	UPROPERTY()
	AActor* SelectedActor;
	
	UPROPERTY()
	AActor* HitActor;
	UPROPERTY()
	AActor* EditActor;
	UPROPERTY()
	FString CurrentForceDeviceID;
	UPROPERTY()
	UPrimitiveComponent* HitComponent;
	
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	UPROPERTY()
	USceneManagerSubsystem*SceneManagerSubsystem;
	UPROPERTY()
	USaveSubsystem *SaveSubsystem;
	UPROPERTY()
	UTrainStationManager*TrainStationManager;
	UPROPERTY()
	FTimerHandle SetInfoTimer;
	UPROPERTY()
	FInteractionSettings InteractionSettings;
	UPROPERTY()
	float InteractionScanRate = 0.001;
	UPROPERTY()
	FTimerHandle QueryTimerHandle;
	UPROPERTY()
	UGameplayTask_Interactive* Task;
	UPROPERTY()
	bool bIsDragging = false;
	
	UPROPERTY()
	FHitResult SavedHitResult;
	UPROPERTY()
	FEditItem EditItem;

	UPROPERTY()
	bool bUIIsSelected = false;
public:
	UPROPERTY(BlueprintAssignable)
	FOnActivateGizmo OnActivateGizmo;
	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Gizmo")	
	FOnTransformModeChange OnTransformModeChange;

	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Gizmo")	
	FOnAlignSpaceChange OnAlignSpaceChange;

	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Interactive")	
	FForceDeviceSignature ForceDeviceSignature;
	UPROPERTY()
	bool bIsHoverLocked = false;
};
