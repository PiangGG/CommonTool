// Fill out your copyright notice in the Description page of Project Settings.

/*
 * @author piang
 * 场景管理子系统
 */
#pragma once

#include "CoreMinimal.h"
#include "CommonToolCore/Library/StructLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SceneManagerSubsystem.generated.h"

class ULevelStreamingDynamic;
class USceneTreeNode;
class AComLidarPointCloudActor;
class AComDatasmithActor;
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

//逻辑场景切换
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneChangeSignature,const FString&,SceneName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneChangeCompleteSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckLevelChangeSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnScenePOIInit,const FString&,ID,const FVector&,Location,const bool,bShow,UUserWidget* ,UserWidget);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldAssetTreeNodesUpdate,const TArray<UWorldAssetTreeNode*>&, WorldAssetTreeNodes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneTreeNodesUpdate,const USceneTreeNode*, SceneTreeNode);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSaveActorAttributes,const FString &, ActorGUID);

UCLASS()
class COMMONTOOLCORE_API USceneManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	USceneManagerSubsystem();

	static USceneManagerSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	//初始化场景
	UFUNCTION(BlueprintCallable)
	void InitializeScene();

	//初始化场景树
	UFUNCTION(BlueprintCallable)
	void InitializeSceneTree();

	//切换逻辑场景
	UFUNCTION(BlueprintCallable)
	void ChangeScene(FString name);
	UFUNCTION(BlueprintCallable)
	bool GetCurrentJsonSceneData(FString &JsonSceneData);
	UFUNCTION(BlueprintCallable)
	bool GetJsonSceneData(const FString &SceneName,FString &JsonSceneData);
	UFUNCTION(BlueprintCallable)
	bool GetCurrentSceneTransform(FTransform &Transform);
	UFUNCTION(BlueprintPure)
	AActor* GetTargetSceneActor(const FString &SceneName);
	UFUNCTION(BlueprintPure)
	FString GetCurrentSceneName() const;
	//TODO 待移除到各自类里面
	//场景相关动态导入DS 点云 倾斜摄影
	UFUNCTION()
	AComDatasmithActor* ResetDataSmithActor(const FString& Path,const bool& bLoad,AComDatasmithActor* Actor, const FTransform& Transform);
	UFUNCTION()
	AComLidarPointCloudActor* ResetPointCloudActor(const FString& Path,const bool& bLoad,AComLidarPointCloudActor* Actor, const FTransform& Transform);
	UFUNCTION()
	ACesium3DTileset* ResetCesium3DTileSetActor(const FString& Path,const bool& bLoad,ACesium3DTileset* Actor, const FTransform& Transform);

	UFUNCTION()
	void PointCloudRebuilt();
	UFUNCTION()
	void PointCloudRebuiltNextTick();
	
	UFUNCTION(BlueprintCallable)
	void InteractiveHit(FHitResult HitResult);
	
	//序列化场景中有该结构的actor
	//获取所有父节点为根的actor
	UFUNCTION()
	bool SaveSceneHierarchyToJson(const FString& FileName = TEXT(""));
	TSharedPtr<FJsonObject> SerializeActor(const FString& GUID);
	void GatherAttachedActors(const AActor* Parent, TArray<TSharedPtr<FJsonValue>>& ChildrenArray);
	
	//构建场景中的资源树
	UWorldAssetTreeNode* DeSerializeWorldAssetTreeNode(const TSharedPtr<FJsonObject>& JsonObject,UWorldAssetTreeNode* Parent = nullptr,int32 layer = 0);
	void SatherAttachedWorldAssetTreeNodes(TArray<TSharedPtr<FJsonValue>> ChildrenArray,UWorldAssetTreeNode* Parent);
	UFUNCTION(BlueprintCallable)
	void AddWorldAssetTreeNode(UWorldAssetTreeNode* TreeNode,UWorldAssetTreeNode* NewParentTreeNode = nullptr);
	UFUNCTION(BlueprintPure)
	TArray<UWorldAssetTreeNode*> GetWorldAssetTreeNodes();
	UFUNCTION(BlueprintCallable)
	void RegisterActor(FString& GUID,AActor* Actor,bool bRegenerate = false);
	UFUNCTION()
	void UnRegisterActor(const FString& GUID);
	UFUNCTION(BlueprintCallable)
	bool GetActorGUID(AActor* actor,FString& GUID);
	UFUNCTION(BlueprintCallable)
	AActor* GetActor(const FString& GUID);
	
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|SceneManager")
	void MoveToPosition(const FString& PositionName);
	
	UFUNCTION(BlueprintCallable)
	void GetPositionings(TArray<FString>&Positionings,const FGameplayTag& matchGameplayTag);
	
	//
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

	UFUNCTION(BlueprintCallable)
	USceneTreeNode* GetSceneTreeNode();

	UFUNCTION(BlueprintCallable)
	void GetRegisterSceneNumSort(int32 Num,TArray<FString>& ResultKeys);
	//virtual AActor* GetRegisterScene(const FString& GUID);
	
	UFUNCTION(BlueprintCallable,Category="TL|TrainStationManager")
	bool GetScenePoints(TArray<FVector>& ScenePoints);

	UFUNCTION(BlueprintCallable)
	void MoveToRecentlyLineScene();
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|SceneManager")
	void SaveActor(AActor* actor);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|SceneManager")
	void SaveNewActorToJson(AActor* actor,FString& GUID);
	UFUNCTION(BlueprintCallable,Category="CommonToolCore|SceneManager")
	void SaveActorToJson(FString& GUID,const TArray<FString> Attributes);

	virtual void RegisterScene(const FString& GUID,AActor* actor);
	
	virtual void UnRegisterScene(const FString& GUID);
	UFUNCTION()
	void InitScenePOI(const FString& ID,const bool bShow,UUserWidget* UserWidget,const FVector& Offsize = FVector::ZeroVector);
	
protected:
	
	UFUNCTION()
	void OnSceneChange(const FString& name);
	UFUNCTION()
	void OnSceneChangeComplete();
	UFUNCTION()
	void OnCheckLevelChange();
	
	virtual void OnActorSpawned(AActor* Actor);
	
	virtual void OnActorDestroyed(AActor* Actor);
	
	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag State);
	//动态导入DS文件时的正在加载触发函数
	UFUNCTION()
	void OnLoad(const AComDatasmithActor* CommonDataSmithRuntimeActor);
	UFUNCTION()
	void OnUnLevelLoaded();
	UFUNCTION()
	void OnLevelLoaded();

private:
	//根据数据构建场景树
	//UFUNCTION(BlueprintCallable, Category = "Tree")
	USceneTreeNode* BuildSceneTree(const FString& JsonString);
	
	//反序列化场景中有该结构的actor
	bool LoadSceneHierarchyFromJson(const FString& FileName);
	bool UnLoadSceneHierarchyFromJson(const FString& FileName);
	//更新场景资源 更新资源树
	void UpdateWorldHierarchyToJson(const FString& FileName);
	
	AActor* DeSerializeActor(const TSharedPtr<FJsonObject>& JsonObject,AActor* Parent = nullptr);
	void SatherAttachedActors(TArray<TSharedPtr<FJsonValue>> ChildrenArray,AActor* Parent);

	UFUNCTION()
	void LoadMaps(TArray<FString> LoadLevelsData);
	UFUNCTION()
	void UpdateStreamLevel(const FString& LoadLevelData,const TArray<FString>&LoadLevelsData,const TArray<FString>&AllLoadLevelsData);
	UFUNCTION()
	void RequestAsyncLoadMap(const FString& LoadMapData);
	int32 RequestAsyncLoadMapNum = 0;
	UFUNCTION()
	void RequestAsyncUnLoadMap(const FString& LoadMapData);
	int32 RequestAsyncUnLoadMapNum = 0;
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
public:
	//场景切换和场景切换完成的委托
	UPROPERTY(BlueprintAssignable)
	FOnSceneChangeSignature OnSceneChangeSignature;
	UPROPERTY(BlueprintAssignable)
	FOnSceneChangeCompleteSignature OnSceneChangeCompleteSignature;
	UPROPERTY(BlueprintAssignable)
	FOnCheckLevelChangeSignature OnCheckLevelChangeSignature;
	//场景树与场景载入之后的资产树
	UPROPERTY(BlueprintAssignable)
	FOnSceneTreeNodesUpdate OnSceneTreeNodesUpdate;
	UPROPERTY(BlueprintAssignable)
	FOnWorldAssetTreeNodesUpdate OnWorldAssetTreeNodesUpdate;
	UPROPERTY(BlueprintAssignable)
	FOnScenePOIInit OnScenePOIInit;
private:
	friend class URegionMarkComponent;
	/*
	 * 其他单例子系统
	 */
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	UPROPERTY()
	USaveSubsystem*SaveSubsystem;
	UPROPERTY()
	UInteractiveSubsystem* InteractiveSubsystem;
	
	UPROPERTY()
	UDeviceManager*DeviceManager;
	UPROPERTY()
	UUIManagerSubsystem *UIManagerSubsystem;

	FDelegateHandle ActorSpawnedDelegateHandle;
	FDelegateHandle ActorDestroyedDelegateHandle;
	// 使用哈希表快速查找节点
	TMap<FString, USceneTreeNode*> SceneTreeNodeMap;
	TMap<FString, USceneTreeNode*> SceneLeafNodeMap;
	//在场景中显示设备数据的actor
	UPROPERTY()
	ACommonInfo* CommonInfo;

	//动态actor 被用户标记修改属性的actor或者用户生成的actor的键值对
	UPROPERTY()
	TMap<FString,AActor*> DynamicActorsMap;
	//场景中已经存在的静态actor设备
	UPROPERTY()
	TMap<FString,AActor*> StaticActorsMap;
	
	//场景正在加载的定时器
	UPROPERTY()
	FTimerHandle LoadSceneTimerHandle;

	UPROPERTY()
	FTimerHandle CheckSceneActorsTimerHandle;
	
	//当前加载的关卡
	UPROPERTY()
	FString CurrentSceneName;
	
	//上一次加载的关卡
	UPROPERTY()
	FString LastSceneName;

	//当前区域加载的子关卡
	UPROPERTY()
	TArray<FString> CurrentLoadLevelsData;
	//当前动态加载的地图
	UPROPERTY()
	TMap<FString,ULevelStreamingDynamic*> CurrentStreamLevelMap;
	
	//编辑模块中的资产树
	UPROPERTY()
	TArray<UWorldAssetTreeNode*> WorldAssetTreeNodes;

	UPROPERTY()
	USceneTreeNode* SceneTreeNode;
	
	UPROPERTY()
	TMap<FString,APositioning*> PositioningsMap;
	
	TMap<FGameplayTag,TArray<AActor*>> MarkTypeMap;

	UPROPERTY()
	TArray<AActor*> LastMarkActors;

	//设置过actor的材质的原始材质
	//UPROPERTY()
	TMap<TObjectPtr<UStaticMeshComponent>,TArray<TObjectPtr<UMaterialInterface>>> LastMarkActorsMaterialMap;
	
	UPROPERTY()
	FGameplayTag CurrentMarkType;

	UPROPERTY()
	float CurrentMarkScale = 0.2f;

	//站点在三维场景中的位置
	UPROPERTY()
	TMap<FString,AActor*> SceneActorMap;
};
