// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CommonGizmoDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(config=Game, Category = CommonTool, defaultconfig, meta = (DisplayName = "CommonGizmoDeveloperSettings"))
class COMMONTOOL_API UCommonGizmoDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	//材质软引用
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_Rotation_XAxis;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_XAxis;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_XAxis_Ghost;

	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_Rotation_YAxis;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_YAxis;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_YAxis_Ghost;

	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_Rotation_ZAxis;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_ZAxis;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_ZAxis_Ghost;

	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> M_GizmoAxis;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_Ghost;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_XYZAxis;

	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> M_AngleHelper;

	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> M_Hidden;

	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> M_ScreenRotation;

	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_Hovered;

	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_Rotation_Hovered;

	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UMaterialInterface> MI_ScreenRotation_Hovered;

	//模型软引用
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_AngleHelper;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_LineHelper;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_PlaneHandle;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_RotationHandle;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_RotationHandleCollision;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_ScaleHandle;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_ScaleHelper_Cube;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_ScaleHelper_Plane;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_ScaleHelper_UnitLine;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_ScreenRotationHandle;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_ScreenTranslationHandle;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_TrackballRotationHandle;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_TranslationHandle;
	UPROPERTY(EditAnywhere, config = CommonGizmo, Category="CommonLibrary|Gizmo")
	TSoftObjectPtr<UStaticMesh> SM_UniformScaleHandle;
};
