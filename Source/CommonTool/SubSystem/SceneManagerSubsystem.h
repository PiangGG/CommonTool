// Fill out your copyright notice in the Description page of Project Settings.

/*
 * @author piang
 * 场景管理子系统
 */
#pragma once

#include "CoreMinimal.h"
#include "CommonTool/Library/StructLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SceneManagerSubsystem.generated.h"

class URegionManager;
class UDeviceManager;
class FSceneManagerRunnable;
class ACommonDataSmithRuntimeActor;
class ACommonLidarPointCloudActor;
class ACesium3DTileset;
class ACommonInfo;
class ACommonGizmoTransform;
class USaveSubsystem;
class UStateSubsystem;
class UConfigSubSystem;
class UImportExportSubsystem;
class UInteractiveSubsystem;
class UUIManagerSubsystem;
class UWorldAssetTreeNode;
class APositioning;

struct FGameplayTag;
struct FEditItem;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSceneManagerBuild, bool, bBuild);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSaveActorAttributes,const FString &, ActorGUID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneChanged, const FString&,SceneName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInspectionCheckSignature, bool, InspectionStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldAssetTreeNodesUpdate,const TArray<UWorldAssetTreeNode*>&, MyWorldAssetTreeNodes);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectionActor,bool,bSelection,AActor*,SelectionActor);

//当前区域设置
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAreaSet,const FString &, AreaSet);
UCLASS()
class COMMONTOOL_API USceneManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//Test
	UFUNCTION(BlueprintCallable)
	void PrintTree();
	
	USceneManagerSubsystem();

	static USceneManagerSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	UFUNCTION()
	void OnUserStateOnChange(FGameplayTag State, const FTransform& Transform, float Zoom,bool bblend);
	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag State);
	UFUNCTION()
	void OnDeviceStateOnChange(FGameplayTag newState);
	UFUNCTION()
	void OnConfigChanged(const FString& ConfigString);
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	//TODO 改成世界开始时候生成
	UFUNCTION(BlueprintCallable)
	void InitializeScene();

	//设置当前场景名称
	UFUNCTION()
	void SetCurrentSceneName(const FString& NewSceneName) const;
	
	UFUNCTION()
	void PointCloudRebuilt();
	UFUNCTION()
	void PointCloudRebuiltNextTick();

	//设置编辑模式
	UFUNCTION(BlueprintCallable)
	void SetTransformMode(FGameplayTag InTransformMode, bool CombineTranslateRotate);

	//设置编辑空间位置
	UFUNCTION(BlueprintCallable)
	void SetAlignSpace(FGameplayTag InAlignSpace);
	
	void SpawnActor(TSoftClassPtr<AActor> actor);
	
	UFUNCTION(BlueprintCallable)
	void SpawnActor(FEditItem ActorEditItem);

	UFUNCTION()
	void UISectionChange(const FEditItem& myItem, bool bSelected);
	UFUNCTION()
	void ForceDevice(const FString& DeviceID);

	UFUNCTION(BlueprintCallable)
	FString GetCurrentForceDeviceID();
	UPROPERTY()
	AActor* CurrentForceDevice;
	UPROPERTY()
	FString CurrentForceDeviceID;
	UFUNCTION(BlueprintCallable)
	void SelectionActor(bool IsSelected, AActor* Actor);
	UFUNCTION(BlueprintCallable)
	void InteractiveHit(FHitResult HitResult);
	UFUNCTION(BlueprintCallable)
	void DeInteractive();

	//序列化场景中有该结构的actor
	//获取所有父节点为根的actor
	UFUNCTION()
	bool SaveSceneHierarchyToJson(const FString& FileName = TEXT(""));
	TSharedPtr<FJsonObject> SerializeActor(const FString& GUID);
	void GatherAttachedActors(const AActor* Parent, TArray<TSharedPtr<FJsonValue>>& ChildrenArray);
	
	UFUNCTION()
	void OnActorInitializationCompletedDelegateCheckedDelay(AActor * Actor);
	//更新场景资源 更新资源树
	void UpdateWorldHierarchyToJson(const FString& FileName = TEXT(""));
	//构建场景中的资源树
	UWorldAssetTreeNode* DeSerializeWorldAssetTreeNode(const TSharedPtr<FJsonObject>& JsonObject,UWorldAssetTreeNode* Parent = nullptr,int32 layer = 0);
	void SatherAttachedWorldAssetTreeNodes(TArray<TSharedPtr<FJsonValue>> ChildrenArray,UWorldAssetTreeNode* Parent);

	UFUNCTION(BlueprintCallable)
	bool GetActorGUID(AActor* actor,FString& GUID);
	
	UFUNCTION(BlueprintCallable)
	void AddWorldAssetTreeNode(UWorldAssetTreeNode* TreeNode,UWorldAssetTreeNode* NewParentTreeNode = nullptr);

	UFUNCTION(BlueprintCallable)
	void RegisterActor(FString& GUID,AActor* Actor,bool bRegenerate = false);
	UFUNCTION()
	void UnRegisterActor(const FString& GUID);

	/*
	 *自动巡检相关
	 * @param InspectionIndex 当前巡检到的设备的索引
	 * @param bReStart 是否重新开始
	 * @param InspectionCheckSignature 点击自动巡检触发委托
	 */
	//Inspection
	void AddInspectionActor(AActor* Actor);
	void RemoveInspectionActor(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Inspection")
	void StartInspection(bool bReStart = true);
	UFUNCTION()
	void InspectionIng();
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Inspection")
	void StopInspection();
	FTimerHandle InspectionTimerHandle;
	//当前巡检设备索引
	UPROPERTY()
	int32 InspectionIndex = 0;
	//是否重新还是巡检
	UPROPERTY(BlueprintReadOnly)
	bool bInspectionStarted = false;
	//巡检速率
	UPROPERTY()
	int32 InspectionRate = 2;
	//自动巡检切换委托
	UPROPERTY(BlueprintAssignable)
	FInspectionCheckSignature InspectionCheckSignature;
	//==========

	//获取属性值
	/*
	 * @param AttributeType 0为字符串 1为bool
	 */
	bool GetStringAttribute(const TSharedPtr<FJsonObject>& JsonObject,const FString& AttributeName, FString &AttributeValue);

	bool GetBoolAttribute(const TSharedPtr<FJsonObject>& JsonObject,const FString& AttributeName, bool &AttributeValue);
	
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Attribute")
	static bool GetActorAttributes(const FString& SceneName,const FString& GUID,TArray<FString>& Attributes);

	/*
	 * @param SceneName为空表示当前地图
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Attribute")
	bool GetActorStatic(FString SceneName,const FString& GUID);
	/*
	 * 通过键值对创建一个关于电站的动态属性(目前只支持动态创建字符串类型)
	 * @param AttributeName 属性名称
	 * @param AttributeValue 属性数值
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Attribute")
	bool MakeAttribute(const FString& AttributeName, const FString &AttributeValue,FString& ResultAttribute);

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|SceneManager")
	void MoveToPosition(const FString& PositionName);

	UFUNCTION(BlueprintCallable)
	void GetPositionings(TArray<FString>&Positionings);

	
	UFUNCTION(BlueprintCallable)
	void GetAllMarkTypes(TArray<FGameplayTag>& MarkTypes);

	UFUNCTION(BlueprintCallable)
	void SetMarkType(const FGameplayTag& MarkType,bool bHideOtherActor ,const bool& ForceReset = false);

	UFUNCTION(BlueprintCallable)
	void GetCurrentMarkType(FGameplayTag& MarkType);
	
	UFUNCTION(BlueprintCallable)
	void GetMarkTypeActors(const FGameplayTag& MarkType,TArray<AActor*>& Actors);

	UFUNCTION(BlueprintCallable)
	float GetCurrentMarkScale();

	UFUNCTION(BlueprintCallable)
	void SetCurrentMarkScale(float currentMarkScale);

	UFUNCTION()
	void GetSceneAllDevice(TArray<FString>& ResultList);

	UFUNCTION()
	void FindSceneDevice(const FString& DeviceID, AActor*& Actor);
	
private:
	//反序列化场景中有该结构的actor
	bool LoadSceneHierarchyFromJson(const FString& FileName = TEXT(""));
	AActor* DeSerializeActor(const TSharedPtr<FJsonObject>& JsonObject,AActor* Parent = nullptr);
	void SatherAttachedActors(TArray<TSharedPtr<FJsonValue>> ChildrenArray,AActor* Parent);
	bool UnLoadSceneHierarchyFromJson(const FString& FileName = TEXT(""));

	//总体保存
	UFUNCTION(BlueprintCallable)
	void OnSave();
	
	/*设备保存时候
	 * @param GUID 保存的actor的GUID
	 * @param Attributes 需要保存的属性数组
	 */
	UFUNCTION()
	void OnSaveActor(const FString& GUID,const TArray<FString>& Attributes);
	
	UFUNCTION(BlueprintCallable)
	void SaveActorAttributes(const FString& GUID,const TArray<FString>& Attributes, const FOnSaveActorAttributes& OnSaveActorAttributes);
	
	/*
	 *@param GUID 需要保存Actor的本地化GUID
	 *@param JsonString 修改Actor Json字符串元素
	 *@return JsonObject 返回的JsonObject
	 *@return 为是否保存成功 返回有更改或添加true
	 */
	bool SaveActorJson(const FString& GUID,const TArray<FString>& Attributes,TSharedPtr<FJsonObject>& JsonObject);
	//当前actor生成完成时候触发,比如线和面多次点击触发
	UFUNCTION()
	void OnActorInitializationCompleted(UObject* Object);

	UFUNCTION(BlueprintCallable,Category="CommonLibrary|SceneManager")
	void SaveNewActorToJson(AActor* actor,FString& GUID,const TArray<FString>& Attributes);
	//当前场景改变时
	UFUNCTION()
	void HandleOnSceneChanged(const FString& NewSceneName);
	
	UFUNCTION()
	void OnTrainStationSelectedComplete();
	
	UFUNCTION()
	void OnDeviceInfoPoolRefresh();

	UFUNCTION()
	void ReSetDeviceInfo(const TArray<FString>& DeviceInfos);

	UFUNCTION()
	void InitSceneAllDevice();
private:
	//在场景中显示设备数据的actor
	UPROPERTY()
	ACommonInfo* CommonInfo;

	//编辑actorTransform的交互actor
	UPROPERTY()
	ACommonGizmoTransform* CommonGizmoTransform;

	//动态actor 被用户标记修改属性的actor或者用户生成的actor的键值对
	UPROPERTY()
	TMap<FString,AActor*> ActorsMap;
	
	//需要巡检的actor包括静态和动态的
	UPROPERTY()
	TArray<AActor*> ViewActors;
	
	//场景正在加载的定时器
	UPROPERTY()
	FTimerHandle SceneBuildTimer;
	
	/*
	 * 其他单例子系统
	 */
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	UPROPERTY()
	UConfigSubSystem* ConfigSubSystem;
	UPROPERTY()
	USaveSubsystem*SaveSubsystem;
	UPROPERTY()
	UImportExportSubsystem *ImportExportSubsystem;
	UPROPERTY()
	UInteractiveSubsystem* InteractiveSubsystem;
	UPROPERTY()
	URegionManager*RegionManager;
	UPROPERTY()
	UDeviceManager*DeviceManager;
	UPROPERTY()
	UUIManagerSubsystem *UIManagerSubsystem;
	//当前加载的关卡
	UPROPERTY()
	FString CurrentSceneName;
	
	//台账选项UI
	UPROPERTY()
	UUserWidget* UserWidget;
	//上一次加载的关卡
	UPROPERTY()
	FString LastSceneName;
	
	UPROPERTY()
	FEditItem EditItem;

	//UI中是否已经选择
	UPROPERTY()
	bool bIsSelection = false;

	//当前生成的编辑actor
	UPROPERTY()
	AActor * CurrentSpawningActor = nullptr;

	//编辑模块中的资产树
	UPROPERTY()
	TArray<UWorldAssetTreeNode*> WorldAssetTreeNodes;
	//场景中已经存在的静态actor设备
	UPROPERTY()
	TMap<FString,AActor*> StaticDeviceActorMap;

	//测试用的使用组件挂载的方式
	UPROPERTY()
	TMap<FString,AActor*> ComponentDeviceActorMap;

	UPROPERTY()
	TArray<FString> LastComponentDeviceActorKeys;

	UPROPERTY()
	TMap<FString,APositioning*> PositioningsMap;
	
	TMap<FGameplayTag,TArray<AActor*>> MarkTypeMap;

	UPROPERTY()
	TArray<AActor*> LastMarkActors;

	//设置过actor的材质的原始材质
	TMap<UStaticMeshComponent*,TArray<UMaterialInterface*>> LastMarkActorsMaterialMap;
	
	UPROPERTY()
	FGameplayTag CurrentMarkType;

	UPROPERTY()
	float CurrentMarkScale = 0.2f;
public:
	UPROPERTY(BlueprintAssignable)
	FOnWorldAssetTreeNodesUpdate OnWorldAssetTreeNodesUpdate;
	UPROPERTY(BlueprintAssignable)
	FOnSelectionActor OnSelectionActor;
	UPROPERTY(BlueprintAssignable)
	FOnSceneChanged OnSceneChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAreaSet OnAreaSet;
};
