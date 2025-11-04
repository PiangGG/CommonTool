// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonToolCoreTags.h"
#include "GameplayTagContainer.h"
#include "StructLibrary.generated.h"
/**
 * 
 */
class COMMONTOOLCORE_API StructLibrary
{
public:
	StructLibrary();
	~StructLibrary();
};

USTRUCT(BlueprintType)
struct FIconItem:public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag Tag;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FEditItem:public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString Name;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSoftClassPtr<AActor>Actor;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag Tag;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> ItemIcon;
};

USTRUCT(BlueprintType)
struct FInteractionSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|InteractionSettings",
		meta = (AllowPrivateAccess = "true"))
	FGameplayTag InteractionSource = CommonToolTags::PointerInteractionSource_World;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|InteractionSettings",
		meta = (AllowPrivateAccess = "true"))
	float InteractionDistance = 4000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|InteractionSettings",
		meta = (AllowPrivateAccess = "true"))
	bool bUseMultiRayHit = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|InteractionSettings", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> InteractableTypes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|InteractionSettings",
		meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* OverlapTestComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|InteractionSettings",
		meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> ExcludeActors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|InteractionSettings",
		meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> IncludeActors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|InteractionSettings",
		meta = (AllowPrivateAccess = "true"))
	bool bAutoStartDrag = true;
};

USTRUCT(BlueprintType)
struct FDeviceTreeNodeStruct
{
	GENERATED_BODY()
public:
	//设备路径与名字
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString Name;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString Type;
	//设备单独描述
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString DeviceComponentDescription;
	//设备组件
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh>StaticMesh;
	//拆解之后目标点
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FVector DisassembleTargetPosition;
	//第二次选中的偏移方向
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FVector DisassembleDirection;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bShowPOI = false;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float DelayTime = 1.f;
};

USTRUCT(BlueprintType)
struct FDeviceConstructStruct:public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<FDeviceTreeNodeStruct> DeviceTreeNodeStruct;
};