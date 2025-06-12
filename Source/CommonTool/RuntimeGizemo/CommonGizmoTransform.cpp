// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonGizmoTransform.h"

#include "CommonGizmoDeveloperSettings.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "CommonTool/Interface/DeviceInterface.h"
#include "CommonTool/Library/ToolFunctionLibrary.h"
#include "CommonTool/SubSystem/InteractiveSubsystem.h"
#include "CommonTool/SubSystem/StateSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACommonGizmoTransform::ACommonGizmoTransform()
{
	//静态加载模型加载材质
	SM_AngleHelper = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_AngleHelper.SM_AngleHelper"));
	SM_LineHelper = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_LineHelper.SM_LineHelper"));
	SM_PlaneHandle = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_PlaneHandle.SM_PlaneHandle"));
	SM_RotationHandle = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_RotationHandle.SM_RotationHandle"));
	SM_RotationHandleCollision = LoadObject<UStaticMesh>(
		nullptr, TEXT(
			"/CommonToolLibrary/TransformGizmo/Meshes/SM_RotationHandleCollision.SM_RotationHandleCollision"));
	SM_ScaleHandle = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_ScaleHandle.SM_ScaleHandle"));
	SM_ScaleHelper_Cube = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_ScaleHelper_Cube.SM_ScaleHelper_Cube"));
	SM_ScaleHelper_Plane = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_ScaleHelper_Plane.SM_ScaleHelper_Plane"));
	SM_ScaleHelper_UnitLine = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_ScaleHelper_UnitLine.SM_ScaleHelper_UnitLine"));
	SM_ScreenRotationHandle = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_ScreenRotationHandle.SM_ScreenRotationHandle"));
	SM_ScreenTranslationHandle = LoadObject<UStaticMesh>(
		nullptr, TEXT(
			"/CommonToolLibrary/TransformGizmo/Meshes/SM_ScreenTranslationHandle.SM_ScreenTranslationHandle"));
	SM_TrackballRotationHandle = LoadObject<UStaticMesh>(
		nullptr, TEXT(
			"/CommonToolLibrary/TransformGizmo/Meshes/SM_TrackballRotationHandle.SM_TrackballRotationHandle"));
	SM_TranslationHandle = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_TranslationHandle.SM_TranslationHandle"));
	SM_UniformScaleHandle = LoadObject<UStaticMesh>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Meshes/SM_UniformScaleHandle.SM_UniformScaleHandle"));

	M_AngleHelper = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/M_AngleHelper.M_AngleHelper"));
	M_GizmoAxis = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/M_GizmoAxis.M_GizmoAxis"));
	M_Hidden = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/M_Hidden.M_Hidden"));
	M_ScreenRotation = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/M_ScreenRotation.M_ScreenRotation"));
	MI_Ghost = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_Ghost.MI_Ghost"));
	MI_Hovered = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_Hovered.MI_Hovered"));
	MI_Rotation_Hovered = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_Rotation_Hovered.MI_Rotation_Hovered"));
	MI_Rotation_XAxis = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_Rotation_XAxis.MI_Rotation_XAxis"));
	MI_Rotation_YAxis = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_Rotation_YAxis.MI_Rotation_YAxis"));
	MI_Rotation_ZAxis = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_Rotation_ZAxis.MI_Rotation_ZAxis"));
	MI_ScreenRotation_Hovered = LoadObject<UMaterialInterface>(
		nullptr,
		TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_ScreenRotation_Hovered.MI_ScreenRotation_Hovered"));
	MI_XAxis = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_XAxis.MI_XAxis"));
	MI_XAxis_Ghost = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_XAxis_Ghost.MI_XAxis_Ghost"));
	MI_XYZAxis = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_XYZAxis.MI_XYZAxis"));
	MI_YAxis = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_YAxis.MI_YAxis"));
	MI_YAxis_Ghost = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_YAxis_Ghost.MI_YAxis_Ghost"));
	MI_ZAxis = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_ZAxis.MI_ZAxis"));
	MI_ZAxis_Ghost = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/CommonToolLibrary/TransformGizmo/Materials/MI_ZAxis_Ghost.MI_ZAxis_Ghost"));

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TransformGizmoGroup = CreateDefaultSubobject<USceneComponent>("TransformGizmo");
	RootComponent = TransformGizmoGroup;

	
		
	TranslationGizmoGroup = CreateDefaultSubobject<USceneComponent>("TranslationGizmo");
	TranslationGizmoGroup->PrimaryComponentTick.bStartWithTickEnabled = false;
	TranslationGizmoGroup->SetupAttachment(TransformGizmoGroup);

	TranslationHandle_X = CreateDefaultSubobject<UStaticMeshComponent>("TranslationHandle_X");
	TranslationHandle_X->SetupAttachment(TranslationGizmoGroup);
	TranslationHandle_X->SetStaticMesh(SM_TranslationHandle);
	TranslationHandle_X->SetMaterial(0, MI_XAxis);
	TranslationHandle_X->SetGenerateOverlapEvents(false);
	TranslationHandle_X->SetCollisionProfileName(FName("Gizmo"), false);
	TranslationHandle_X->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TranslationHandle_X->CastShadow = false;
	TranslationHandle_X->bVisibleInReflectionCaptures = false;
	TranslationHandle_X->bVisibleInRealTimeSkyCaptures = false;
	TranslationHandle_X->bVisibleInRayTracing = false;
	TranslationHandle_X->bReceivesDecals = false;
	TranslationHandle_X->TranslucencySortPriority = 999999;

	TranslationHandle_Y = CreateDefaultSubobject<UStaticMeshComponent>("TranslationHandle_Y");
	TranslationHandle_Y->SetupAttachment(TranslationGizmoGroup);
	TranslationHandle_Y->SetRelativeRotation(FRotator(0, 90, 0));
	TranslationHandle_Y->SetStaticMesh(SM_TranslationHandle);
	TranslationHandle_Y->SetMaterial(0, MI_YAxis);
	TranslationHandle_Y->SetGenerateOverlapEvents(false);
	TranslationHandle_Y->SetCollisionProfileName(FName("Gizmo"), false);
	TranslationHandle_Y->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TranslationHandle_Y->CastShadow = false;
	TranslationHandle_Y->bVisibleInReflectionCaptures = false;
	TranslationHandle_Y->bVisibleInRealTimeSkyCaptures = false;
	TranslationHandle_Y->bVisibleInRayTracing = false;
	TranslationHandle_Y->bReceivesDecals = false;
	TranslationHandle_Y->TranslucencySortPriority = 999999;

	TranslationHandle_Z = CreateDefaultSubobject<UStaticMeshComponent>("TranslationHandle_Z");
	TranslationHandle_Z->SetupAttachment(TranslationGizmoGroup);
	TranslationHandle_Z->SetRelativeRotation(FRotator(90, 0, 0));
	TranslationHandle_Z->SetStaticMesh(SM_TranslationHandle);
	TranslationHandle_Z->SetMaterial(0, MI_ZAxis);
	TranslationHandle_Z->SetGenerateOverlapEvents(false);
	TranslationHandle_Z->SetCollisionProfileName(FName("Gizmo"), false);
	TranslationHandle_Z->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TranslationHandle_Z->CastShadow = false;
	TranslationHandle_Z->bVisibleInReflectionCaptures = false;
	TranslationHandle_Z->bVisibleInRealTimeSkyCaptures = false;
	TranslationHandle_Z->bVisibleInRayTracing = false;
	TranslationHandle_Z->bReceivesDecals = false;
	TranslationHandle_Z->TranslucencySortPriority = 999999;


	TranslationHelperGroup = CreateDefaultSubobject<USceneComponent>("TranslationHelperGroup");
	TranslationHelperGroup->PrimaryComponentTick.bStartWithTickEnabled = false;
	TranslationHelperGroup->SetupAttachment(TranslationGizmoGroup);

	TranslationHelper_X = CreateDefaultSubobject<UStaticMeshComponent>("TranslationHelper_X");
	TranslationHelper_X->SetupAttachment(TranslationHelperGroup);
	TranslationHelper_X->SetStaticMesh(SM_TranslationHandle);
	TranslationHelper_X->SetMaterial(0, MI_XAxis_Ghost);
	TranslationHelper_X->SetGenerateOverlapEvents(false);
	TranslationHelper_X->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	TranslationHelper_X->CastShadow = false;
	TranslationHelper_X->bVisibleInReflectionCaptures = false;
	TranslationHelper_X->bVisibleInRealTimeSkyCaptures = false;
	TranslationHelper_X->bVisibleInRayTracing = false;
	TranslationHelper_X->bReceivesDecals = false;
	TranslationHelper_X->TranslucencySortPriority = 999999;

	TranslationHelper_Y = CreateDefaultSubobject<UStaticMeshComponent>("TranslationHelper_Y");
	TranslationHelper_Y->SetupAttachment(TranslationHelperGroup);
	TranslationHelper_Y->SetRelativeRotation(FRotator(0, 90, 0));
	TranslationHelper_Y->SetStaticMesh(SM_TranslationHandle);
	TranslationHelper_Y->SetMaterial(0, MI_YAxis_Ghost);
	TranslationHelper_Y->SetGenerateOverlapEvents(false);
	TranslationHelper_Y->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	TranslationHelper_Y->CastShadow = false;
	TranslationHelper_Y->bVisibleInReflectionCaptures = false;
	TranslationHelper_Y->bVisibleInRealTimeSkyCaptures = false;
	TranslationHelper_Y->bVisibleInRayTracing = false;
	TranslationHelper_Y->bReceivesDecals = false;
	TranslationHelper_Y->TranslucencySortPriority = 999999;

	TranslationHelper_Z = CreateDefaultSubobject<UStaticMeshComponent>("TranslationHelper_Z");
	TranslationHelper_Z->SetupAttachment(TranslationHelperGroup);
	TranslationHelper_Z->SetRelativeRotation(FRotator(90, 0, 0));
	TranslationHelper_Z->SetStaticMesh(SM_TranslationHandle);
	TranslationHelper_Z->SetMaterial(0, MI_ZAxis_Ghost);
	TranslationHelper_Z->SetGenerateOverlapEvents(false);
	TranslationHelper_Z->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	TranslationHelper_Z->CastShadow = false;
	TranslationHelper_Z->bVisibleInReflectionCaptures = false;
	TranslationHelper_Z->bVisibleInRealTimeSkyCaptures = false;
	TranslationHelper_Z->bVisibleInRayTracing = false;
	TranslationHelper_Z->bReceivesDecals = false;
	TranslationHelper_Z->TranslucencySortPriority = 999999;


	ScreenTranslationHandle = CreateDefaultSubobject<UStaticMeshComponent>("ScreenTranslationHandle");
	ScreenTranslationHandle->SetupAttachment(TranslationGizmoGroup);
	ScreenTranslationHandle->SetRelativeRotation(FRotator(0, 0, 0));
	ScreenTranslationHandle->SetStaticMesh(SM_ScreenTranslationHandle);
	ScreenTranslationHandle->SetMaterial(0, MI_XYZAxis);
	ScreenTranslationHandle->SetGenerateOverlapEvents(false);
	ScreenTranslationHandle->SetCollisionProfileName(FName("Gizmo"), false);
	ScreenTranslationHandle->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	ScreenTranslationHandle->CastShadow = false;
	ScreenTranslationHandle->bVisibleInReflectionCaptures = false;
	ScreenTranslationHandle->bVisibleInRealTimeSkyCaptures = false;
	ScreenTranslationHandle->bVisibleInRayTracing = false;
	ScreenTranslationHandle->bReceivesDecals = false;
	ScreenTranslationHandle->TranslucencySortPriority = 999999;

	PlaneTranslationHandle_XY = CreateDefaultSubobject<UStaticMeshComponent>("PlaneTranslationHandle_XY");
	PlaneTranslationHandle_XY->SetupAttachment(TranslationGizmoGroup);
	PlaneTranslationHandle_XY->SetRelativeRotation(FRotator(-90, 0, 0));
	PlaneTranslationHandle_XY->SetStaticMesh(SM_PlaneHandle);
	PlaneTranslationHandle_XY->SetMaterial(0, MI_ZAxis);
	PlaneTranslationHandle_XY->SetGenerateOverlapEvents(false);
	PlaneTranslationHandle_XY->SetCollisionProfileName(FName("Gizmo"), false);
	PlaneTranslationHandle_XY->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	PlaneTranslationHandle_XY->CastShadow = false;
	PlaneTranslationHandle_XY->bVisibleInReflectionCaptures = false;
	PlaneTranslationHandle_XY->bVisibleInRealTimeSkyCaptures = false;
	PlaneTranslationHandle_XY->bVisibleInRayTracing = false;
	PlaneTranslationHandle_XY->bReceivesDecals = false;
	PlaneTranslationHandle_XY->TranslucencySortPriority = 999999;

	PlaneTranslationHandle_XZ = CreateDefaultSubobject<UStaticMeshComponent>("PlaneTranslationHandle_XZ");
	PlaneTranslationHandle_XZ->SetupAttachment(TranslationGizmoGroup);
	PlaneTranslationHandle_XZ->SetRelativeRotation(FRotator(0, -90, 0));
	PlaneTranslationHandle_XZ->SetStaticMesh(SM_PlaneHandle);
	PlaneTranslationHandle_XZ->SetMaterial(0, MI_YAxis);
	PlaneTranslationHandle_XZ->SetGenerateOverlapEvents(false);
	PlaneTranslationHandle_XZ->SetCollisionProfileName(FName("Gizmo"), false);
	PlaneTranslationHandle_XZ->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	PlaneTranslationHandle_XZ->CastShadow = false;
	PlaneTranslationHandle_XZ->bVisibleInReflectionCaptures = false;
	PlaneTranslationHandle_XZ->bVisibleInRealTimeSkyCaptures = false;
	PlaneTranslationHandle_XZ->bVisibleInRayTracing = false;
	PlaneTranslationHandle_XZ->bReceivesDecals = false;
	PlaneTranslationHandle_XZ->TranslucencySortPriority = 999999;

	PlaneTranslationHandle_YZ = CreateDefaultSubobject<UStaticMeshComponent>("PlaneTranslationHandle_YZ");
	PlaneTranslationHandle_YZ->SetupAttachment(TranslationGizmoGroup);
	PlaneTranslationHandle_YZ->SetRelativeRotation(FRotator(0, 0, 0));
	PlaneTranslationHandle_YZ->SetStaticMesh(SM_PlaneHandle);
	PlaneTranslationHandle_YZ->SetMaterial(0, MI_XAxis);
	PlaneTranslationHandle_YZ->SetGenerateOverlapEvents(false);
	PlaneTranslationHandle_YZ->SetCollisionProfileName(FName("Gizmo"), false);
	PlaneTranslationHandle_YZ->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	PlaneTranslationHandle_YZ->CastShadow = false;
	PlaneTranslationHandle_YZ->bVisibleInReflectionCaptures = false;
	PlaneTranslationHandle_YZ->bVisibleInRealTimeSkyCaptures = false;
	PlaneTranslationHandle_YZ->bVisibleInRayTracing = false;
	PlaneTranslationHandle_YZ->bReceivesDecals = false;
	PlaneTranslationHandle_YZ->TranslucencySortPriority = 999999;


		
	RotationGizmoGroup = CreateDefaultSubobject<USceneComponent>("RotationGizmo");
	RotationGizmoGroup->PrimaryComponentTick.bStartWithTickEnabled = false;
	RotationGizmoGroup->SetupAttachment(TransformGizmoGroup);

	RotationHandle_X = CreateDefaultSubobject<UStaticMeshComponent>("RotationHandle_X");
	RotationHandle_X->SetupAttachment(RotationGizmoGroup);
	RotationHandle_X->SetStaticMesh(SM_RotationHandle);
	RotationHandle_X->SetMaterial(0, MI_Rotation_XAxis);
	RotationHandle_X->SetGenerateOverlapEvents(false);
	RotationHandle_X->SetCollisionProfileName(FName("Gizmo"), false);
	RotationHandle_X->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	RotationHandle_X->CastShadow = false;
	RotationHandle_X->bVisibleInReflectionCaptures = false;
	RotationHandle_X->bVisibleInRealTimeSkyCaptures = false;
	RotationHandle_X->bVisibleInRayTracing = false;
	RotationHandle_X->bReceivesDecals = false;
	RotationHandle_X->TranslucencySortPriority = 999999;

	RotationHandle_Y = CreateDefaultSubobject<UStaticMeshComponent>("RotationHandle_Y");
	RotationHandle_Y->SetupAttachment(RotationGizmoGroup);
	RotationHandle_Y->SetRelativeRotation(FRotator(0, 90, 0));
	RotationHandle_Y->SetStaticMesh(SM_RotationHandle);
	RotationHandle_Y->SetMaterial(0, MI_Rotation_YAxis);
	RotationHandle_Y->SetGenerateOverlapEvents(false);
	RotationHandle_Y->SetCollisionProfileName(FName("Gizmo"), false);
	RotationHandle_Y->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	RotationHandle_Y->CastShadow = false;
	RotationHandle_Y->bVisibleInReflectionCaptures = false;
	RotationHandle_Y->bVisibleInRealTimeSkyCaptures = false;
	RotationHandle_Y->bVisibleInRayTracing = false;
	RotationHandle_Y->bReceivesDecals = false;
	RotationHandle_Y->TranslucencySortPriority = 999999;

	RotationHandle_Z = CreateDefaultSubobject<UStaticMeshComponent>("RotationHandle_Z");
	RotationHandle_Z->SetupAttachment(RotationGizmoGroup);
	RotationHandle_Z->SetRelativeRotation(FRotator(90, 0, 0));
	RotationHandle_Z->SetStaticMesh(SM_RotationHandle);
	RotationHandle_Z->SetMaterial(0, MI_Rotation_ZAxis);
	RotationHandle_Z->SetGenerateOverlapEvents(false);
	RotationHandle_Z->SetCollisionProfileName(FName("Gizmo"), false);
	RotationHandle_Z->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	RotationHandle_Z->CastShadow = false;
	RotationHandle_Z->bVisibleInReflectionCaptures = false;
	RotationHandle_Z->bVisibleInRealTimeSkyCaptures = false;
	RotationHandle_Z->bVisibleInRayTracing = false;
	RotationHandle_Z->bReceivesDecals = false;
	RotationHandle_Z->TranslucencySortPriority = 999999;


	RotationHelperGroup = CreateDefaultSubobject<USceneComponent>("RotationHelperGroup");
	RotationHelperGroup->PrimaryComponentTick.bStartWithTickEnabled = false;
	RotationHelperGroup->SetupAttachment(RotationGizmoGroup);

	AngleHelperMesh = CreateDefaultSubobject<UStaticMeshComponent>("AngleHelperMesh");
	AngleHelperMesh->SetupAttachment(RotationHelperGroup);
	AngleHelperMesh->SetStaticMesh(SM_AngleHelper);
	AngleHelperMesh->SetMaterial(0, M_AngleHelper);
	AngleHelperMesh->SetGenerateOverlapEvents(false);
	AngleHelperMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	AngleHelperMesh->CastShadow = false;
	AngleHelperMesh->bVisibleInReflectionCaptures = false;
	AngleHelperMesh->bVisibleInRealTimeSkyCaptures = false;
	AngleHelperMesh->bVisibleInRayTracing = false;
	AngleHelperMesh->bReceivesDecals = false;
	AngleHelperMesh->TranslucencySortPriority = 999999;

	ScreenRotationHandle = CreateDefaultSubobject<UStaticMeshComponent>("ScreenRotationHandle");
	ScreenRotationHandle->SetupAttachment(RotationGizmoGroup);
	ScreenRotationHandle->SetStaticMesh(SM_ScreenRotationHandle);
	ScreenRotationHandle->SetMaterial(0, M_ScreenRotation);
	ScreenRotationHandle->SetGenerateOverlapEvents(false);
	ScreenRotationHandle->SetCollisionProfileName(FName("Gizmo"), false);
	ScreenRotationHandle->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	ScreenRotationHandle->CastShadow = false;
	ScreenRotationHandle->bVisibleInReflectionCaptures = false;
	ScreenRotationHandle->bVisibleInRealTimeSkyCaptures = false;
	ScreenRotationHandle->bVisibleInRayTracing = false;
	ScreenRotationHandle->bReceivesDecals = false;
	ScreenRotationHandle->TranslucencySortPriority = 999999;

	TrackballRotationHandle = CreateDefaultSubobject<UStaticMeshComponent>("TrackballRotationHandle");
	TrackballRotationHandle->SetupAttachment(RotationGizmoGroup);
	TrackballRotationHandle->SetStaticMesh(SM_TrackballRotationHandle);
	TrackballRotationHandle->SetMaterial(0, M_ScreenRotation);
	TrackballRotationHandle->SetGenerateOverlapEvents(false);
	TrackballRotationHandle->SetCollisionProfileName(FName("Gizmo"), false);
	TrackballRotationHandle->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TrackballRotationHandle->CastShadow = false;
	TrackballRotationHandle->bVisibleInReflectionCaptures = false;
	TrackballRotationHandle->bVisibleInRealTimeSkyCaptures = false;
	TrackballRotationHandle->bVisibleInRayTracing = false;
	TrackballRotationHandle->bReceivesDecals = false;
	TrackballRotationHandle->TranslucencySortPriority = 999999;


	
	ScaleGizmoGroup = CreateDefaultSubobject<USceneComponent>("ScaleGizmo");
	ScaleGizmoGroup->PrimaryComponentTick.bStartWithTickEnabled = false;
	ScaleGizmoGroup->SetupAttachment(TransformGizmoGroup);

	ScaleHandle_X = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHandle_X");
	ScaleHandle_X->SetupAttachment(ScaleGizmoGroup);
	ScaleHandle_X->SetStaticMesh(SM_ScaleHandle);
	ScaleHandle_X->SetMaterial(0, MI_XAxis);
	ScaleHandle_X->SetGenerateOverlapEvents(false);
	ScaleHandle_X->SetCollisionProfileName(FName("Gizmo"), false);
	ScaleHandle_X->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	ScaleHandle_X->CastShadow = false;
	ScaleHandle_X->bVisibleInReflectionCaptures = false;
	ScaleHandle_X->bVisibleInRealTimeSkyCaptures = false;
	ScaleHandle_X->bVisibleInRayTracing = false;
	ScaleHandle_X->bReceivesDecals = false;
	ScaleHandle_X->TranslucencySortPriority = 999999;

	ScaleHandle_Y = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHandle_Y");
	ScaleHandle_Y->SetupAttachment(ScaleGizmoGroup);
	ScaleHandle_Y->SetRelativeRotation(FRotator(0, 90, 0));
	ScaleHandle_Y->SetStaticMesh(SM_ScaleHandle);
	ScaleHandle_Y->SetMaterial(0, MI_YAxis);
	ScaleHandle_Y->SetGenerateOverlapEvents(false);
	ScaleHandle_Y->SetCollisionProfileName(FName("Gizmo"), false);
	ScaleHandle_Y->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	ScaleHandle_Y->CastShadow = false;
	ScaleHandle_Y->bVisibleInReflectionCaptures = false;
	ScaleHandle_Y->bVisibleInRealTimeSkyCaptures = false;
	ScaleHandle_Y->bVisibleInRayTracing = false;
	ScaleHandle_Y->bReceivesDecals = false;
	ScaleHandle_Y->TranslucencySortPriority = 999999;

	ScaleHandle_Z = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHandle_Z");
	ScaleHandle_Z->SetupAttachment(ScaleGizmoGroup);
	ScaleHandle_Z->SetRelativeRotation(FRotator(90, 0, 0));
	ScaleHandle_Z->SetStaticMesh(SM_ScaleHandle);
	ScaleHandle_Z->SetMaterial(0, MI_ZAxis);
	ScaleHandle_Z->SetGenerateOverlapEvents(false);
	ScaleHandle_Z->SetCollisionProfileName(FName("Gizmo"), false);
	ScaleHandle_Z->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	ScaleHandle_Z->CastShadow = false;
	ScaleHandle_Z->bVisibleInReflectionCaptures = false;
	ScaleHandle_Z->bVisibleInRealTimeSkyCaptures = false;
	ScaleHandle_Z->bVisibleInRayTracing = false;
	ScaleHandle_Z->bReceivesDecals = false;
	ScaleHandle_Z->TranslucencySortPriority = 999999;

	PlaneScaleHandle_XY = CreateDefaultSubobject<UStaticMeshComponent>("PlaneScaleHandle_XY");
	PlaneScaleHandle_XY->SetupAttachment(ScaleGizmoGroup);
	PlaneScaleHandle_XY->SetRelativeRotation(FRotator(-90, 0, 0));
	PlaneScaleHandle_XY->SetStaticMesh(SM_PlaneHandle);
	PlaneScaleHandle_XY->SetMaterial(0, MI_ZAxis);
	PlaneScaleHandle_XY->SetGenerateOverlapEvents(false);
	PlaneScaleHandle_XY->SetCollisionProfileName(FName("Gizmo"), false);
	PlaneScaleHandle_XY->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	PlaneScaleHandle_XY->CastShadow = false;
	PlaneScaleHandle_XY->bVisibleInReflectionCaptures = false;
	PlaneScaleHandle_XY->bVisibleInRealTimeSkyCaptures = false;
	PlaneScaleHandle_XY->bVisibleInRayTracing = false;
	PlaneScaleHandle_XY->bReceivesDecals = false;
	PlaneScaleHandle_XY->TranslucencySortPriority = 999999;

	PlaneScaleHandle_XZ = CreateDefaultSubobject<UStaticMeshComponent>("PlaneScaleHandle_XZ");
	PlaneScaleHandle_XZ->SetupAttachment(ScaleGizmoGroup);
	PlaneScaleHandle_XZ->SetRelativeRotation(FRotator(0, -90, 0));
	PlaneScaleHandle_XZ->SetStaticMesh(SM_PlaneHandle);
	PlaneScaleHandle_XZ->SetMaterial(0, MI_YAxis);
	PlaneScaleHandle_XZ->SetGenerateOverlapEvents(false);
	PlaneScaleHandle_XZ->SetCollisionProfileName(FName("Gizmo"), false);
	PlaneScaleHandle_XZ->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	PlaneScaleHandle_XZ->CastShadow = false;
	PlaneScaleHandle_XZ->bVisibleInReflectionCaptures = false;
	PlaneScaleHandle_XZ->bVisibleInRealTimeSkyCaptures = false;
	PlaneScaleHandle_XZ->bVisibleInRayTracing = false;
	PlaneScaleHandle_XZ->bReceivesDecals = false;
	PlaneScaleHandle_XZ->TranslucencySortPriority = 999999;

	PlaneScaleHandle_YZ = CreateDefaultSubobject<UStaticMeshComponent>("PlaneScaleHandle_YZ");
	PlaneScaleHandle_YZ->SetupAttachment(ScaleGizmoGroup);
	PlaneScaleHandle_YZ->SetStaticMesh(SM_PlaneHandle);
	PlaneScaleHandle_YZ->SetMaterial(0, MI_XAxis);
	PlaneScaleHandle_YZ->SetGenerateOverlapEvents(false);
	PlaneScaleHandle_YZ->SetCollisionProfileName(FName("Gizmo"), false);
	PlaneScaleHandle_YZ->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	PlaneScaleHandle_YZ->CastShadow = false;
	PlaneScaleHandle_YZ->bVisibleInReflectionCaptures = false;
	PlaneScaleHandle_YZ->bVisibleInRealTimeSkyCaptures = false;
	PlaneScaleHandle_YZ->bVisibleInRayTracing = false;
	PlaneScaleHandle_YZ->bReceivesDecals = false;
	PlaneScaleHandle_YZ->TranslucencySortPriority = 999999;

	UniformScaleHandle = CreateDefaultSubobject<UStaticMeshComponent>("UniformScaleHandle");
	UniformScaleHandle->SetupAttachment(ScaleGizmoGroup);
	UniformScaleHandle->SetStaticMesh(SM_UniformScaleHandle);
	UniformScaleHandle->SetMaterial(0, MI_XYZAxis);
	UniformScaleHandle->SetGenerateOverlapEvents(false);
	UniformScaleHandle->SetCollisionProfileName(FName("Gizmo"), false);
	UniformScaleHandle->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	UniformScaleHandle->CastShadow = false;
	UniformScaleHandle->bVisibleInReflectionCaptures = false;
	UniformScaleHandle->bVisibleInRealTimeSkyCaptures = false;
	UniformScaleHandle->bVisibleInRayTracing = false;
	UniformScaleHandle->bReceivesDecals = false;
	UniformScaleHandle->TranslucencySortPriority = 999999;

	ScaleHelperGroup = CreateDefaultSubobject<USceneComponent>("ScaleHelperGroup");
	ScaleHelperGroup->PrimaryComponentTick.bStartWithTickEnabled = false;
	ScaleHelperGroup->SetupAttachment(ScaleGizmoGroup);

	ScaleHelper_UnitLine_X = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHelper_UnitLineX");
	ScaleHelper_UnitLine_X->SetupAttachment(ScaleHelperGroup);
	ScaleHelper_UnitLine_X->SetStaticMesh(SM_ScaleHelper_UnitLine);
	ScaleHelper_UnitLine_X->SetMaterial(0, MI_XAxis);
	ScaleHelper_UnitLine_X->SetGenerateOverlapEvents(false);
	ScaleHelper_UnitLine_X->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ScaleHelper_UnitLine_X->CastShadow = false;
	ScaleHelper_UnitLine_X->bVisibleInReflectionCaptures = false;
	ScaleHelper_UnitLine_X->bVisibleInRealTimeSkyCaptures = false;
	ScaleHelper_UnitLine_X->bVisibleInRayTracing = false;
	ScaleHelper_UnitLine_X->bReceivesDecals = false;
	ScaleHelper_UnitLine_X->TranslucencySortPriority = 999999;

	ScaleHelper_Cube_X = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHelper_CubeX");
	ScaleHelper_Cube_X->SetupAttachment(ScaleHelper_UnitLine_X);
	ScaleHelper_Cube_X->SetRelativeLocation(FVector(1.0f, 0.0f, 0.0f));
	ScaleHelper_Cube_X->SetStaticMesh(SM_ScaleHelper_Cube);
	ScaleHelper_Cube_X->SetMaterial(0, MI_XAxis);
	ScaleHelper_Cube_X->SetGenerateOverlapEvents(false);
	ScaleHelper_Cube_X->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ScaleHelper_Cube_X->CastShadow = false;
	ScaleHelper_Cube_X->bVisibleInReflectionCaptures = false;
	ScaleHelper_Cube_X->bVisibleInRealTimeSkyCaptures = false;
	ScaleHelper_Cube_X->bVisibleInRayTracing = false;
	ScaleHelper_Cube_X->bReceivesDecals = false;
	ScaleHelper_Cube_X->TranslucencySortPriority = 999999;



	ScaleHelper_UnitLine_Y = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHelper_UnitLineY");
	ScaleHelper_UnitLine_Y->SetupAttachment(ScaleHelperGroup);
	ScaleHelper_UnitLine_Y->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	ScaleHelper_UnitLine_Y->SetStaticMesh(SM_ScaleHelper_UnitLine);
	ScaleHelper_UnitLine_Y->SetMaterial(0, MI_YAxis);
	ScaleHelper_UnitLine_Y->SetGenerateOverlapEvents(false);
	ScaleHelper_UnitLine_Y->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ScaleHelper_UnitLine_Y->CastShadow = false;
	ScaleHelper_UnitLine_Y->bVisibleInReflectionCaptures = false;
	ScaleHelper_UnitLine_Y->bVisibleInRealTimeSkyCaptures = false;
	ScaleHelper_UnitLine_Y->bVisibleInRayTracing = false;
	ScaleHelper_UnitLine_Y->bReceivesDecals = false;
	ScaleHelper_UnitLine_Y->TranslucencySortPriority = 999999;

	ScaleHelper_Cube_Y = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHelper_CubeY");
	ScaleHelper_Cube_Y->SetupAttachment(ScaleHelper_UnitLine_Y);
	ScaleHelper_Cube_Y->SetRelativeLocation(FVector(1.0f, 0.0f, 0.0f));
	ScaleHelper_Cube_Y->SetStaticMesh(SM_ScaleHelper_Cube);
	ScaleHelper_Cube_Y->SetMaterial(0, MI_YAxis);
	ScaleHelper_Cube_Y->SetGenerateOverlapEvents(false);
	ScaleHelper_Cube_Y->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ScaleHelper_Cube_Y->CastShadow = false;
	ScaleHelper_Cube_Y->bVisibleInReflectionCaptures = false;
	ScaleHelper_Cube_Y->bVisibleInRealTimeSkyCaptures = false;
	ScaleHelper_Cube_Y->bVisibleInRayTracing = false;
	ScaleHelper_Cube_Y->bReceivesDecals = false;
	ScaleHelper_Cube_Y->TranslucencySortPriority = 999999;



	ScaleHelper_UnitLine_Z = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHelper_UnitLineZ");
	ScaleHelper_UnitLine_Z->SetupAttachment(ScaleHelperGroup);
	ScaleHelper_UnitLine_Z->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	ScaleHelper_UnitLine_Z->SetStaticMesh(SM_ScaleHelper_UnitLine);
	ScaleHelper_UnitLine_Z->SetMaterial(0, MI_ZAxis);
	ScaleHelper_UnitLine_Z->SetGenerateOverlapEvents(false);
	ScaleHelper_UnitLine_Z->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ScaleHelper_UnitLine_Z->CastShadow = false;
	ScaleHelper_UnitLine_Z->bVisibleInReflectionCaptures = false;
	ScaleHelper_UnitLine_Z->bVisibleInRealTimeSkyCaptures = false;
	ScaleHelper_UnitLine_Z->bVisibleInRayTracing = false;
	ScaleHelper_UnitLine_Z->bReceivesDecals = false;
	ScaleHelper_UnitLine_Z->TranslucencySortPriority = 999999;

	ScaleHelper_Cube_Z = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHelper_CubeZ");
	ScaleHelper_Cube_Z->SetupAttachment(ScaleHelper_UnitLine_Z);
	ScaleHelper_Cube_Z->SetRelativeLocation(FVector(1.0f, 0.0f, 0.0f));
	ScaleHelper_Cube_Z->SetStaticMesh(SM_ScaleHelper_Cube);
	ScaleHelper_Cube_Z->SetMaterial(0, MI_ZAxis);
	ScaleHelper_Cube_Z->SetGenerateOverlapEvents(false);
	ScaleHelper_Cube_Z->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ScaleHelper_Cube_Z->CastShadow = false;
	ScaleHelper_Cube_Z->bVisibleInReflectionCaptures = false;
	ScaleHelper_Cube_Z->bVisibleInRealTimeSkyCaptures = false;
	ScaleHelper_Cube_Z->bVisibleInRayTracing = false;
	ScaleHelper_Cube_Z->bReceivesDecals = false;
	ScaleHelper_Cube_Z->TranslucencySortPriority = 999999;



	ScaleHelperPlaneGroup = CreateDefaultSubobject<USceneComponent>("ScaleHelperPlaneGroup");
	ScaleHelperPlaneGroup->PrimaryComponentTick.bStartWithTickEnabled = false;
	ScaleHelperPlaneGroup->SetupAttachment(ScaleHelperGroup);

	ScaleHelper_Plane_XY = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHelperPlaneXY");
	ScaleHelper_Plane_XY->SetupAttachment(ScaleHelperPlaneGroup);
	ScaleHelper_Plane_XY->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	ScaleHelper_Plane_XY->SetStaticMesh(SM_ScaleHelper_Plane);
	ScaleHelper_Plane_XY->SetMaterial(0, MI_ZAxis);
	ScaleHelper_Plane_XY->SetGenerateOverlapEvents(false);
	ScaleHelper_Plane_XY->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ScaleHelper_Plane_XY->CastShadow = false;
	ScaleHelper_Plane_XY->bVisibleInReflectionCaptures = false;
	ScaleHelper_Plane_XY->bVisibleInRealTimeSkyCaptures = false;
	ScaleHelper_Plane_XY->bVisibleInRayTracing = false;
	ScaleHelper_Plane_XY->bReceivesDecals = false;
	ScaleHelper_Plane_XY->TranslucencySortPriority = 999999;

	ScaleHelper_Plane_XZ = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHelperPlaneXZ");
	ScaleHelper_Plane_XZ->SetupAttachment(ScaleHelperPlaneGroup);
	ScaleHelper_Plane_XZ->SetRelativeRotation(FRotator(0, -90.0f, 0.0f));
	ScaleHelper_Plane_XZ->SetStaticMesh(SM_ScaleHelper_Plane);
	ScaleHelper_Plane_XZ->SetMaterial(0, MI_YAxis);
	ScaleHelper_Plane_XZ->SetGenerateOverlapEvents(false);
	ScaleHelper_Plane_XZ->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ScaleHelper_Plane_XZ->CastShadow = false;
	ScaleHelper_Plane_XZ->bVisibleInReflectionCaptures = false;
	ScaleHelper_Plane_XZ->bVisibleInRealTimeSkyCaptures = false;
	ScaleHelper_Plane_XZ->bVisibleInRayTracing = false;
	ScaleHelper_Plane_XZ->bReceivesDecals = false;
	ScaleHelper_Plane_XZ->TranslucencySortPriority = 999999;

	ScaleHelper_Plane_YZ = CreateDefaultSubobject<UStaticMeshComponent>("ScaleHelperPlaneYZ");
	ScaleHelper_Plane_YZ->SetupAttachment(ScaleHelperPlaneGroup);
	ScaleHelper_Plane_YZ->SetStaticMesh(SM_ScaleHelper_Plane);
	ScaleHelper_Plane_YZ->SetMaterial(0, MI_XAxis);
	ScaleHelper_Plane_YZ->SetGenerateOverlapEvents(false);
	ScaleHelper_Plane_YZ->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ScaleHelper_Plane_YZ->CastShadow = false;
	ScaleHelper_Plane_YZ->bVisibleInReflectionCaptures = false;
	ScaleHelper_Plane_YZ->bVisibleInRealTimeSkyCaptures = false;
	ScaleHelper_Plane_YZ->bVisibleInRayTracing = false;
	ScaleHelper_Plane_YZ->bReceivesDecals = false;
	ScaleHelper_Plane_YZ->TranslucencySortPriority = 999999;
	
	InitializeGizmoHandleMap();
	SetActiveGizmoGroup(true, false, false);
	TranslationHelperGroup->SetVisibility(false, true);
	TranslationHelper_X->SetVisibility(false, true);
	TranslationHelper_Y->SetVisibility(false, true);
	TranslationHelper_Z->SetVisibility(false, true);
	
	RotationHelperGroup->SetVisibility(false, true);
	AngleHelperMesh->SetVisibility(false, true);

	ScaleHelperGroup->SetVisibility(false, true);
	ScaleHelper_UnitLine_X->SetVisibility(false, true);
	ScaleHelper_UnitLine_Y->SetVisibility(false, true);
	ScaleHelper_UnitLine_Z->SetVisibility(false, true);
	ScaleHelper_Cube_X->SetVisibility(false, true);
	ScaleHelper_Cube_Y->SetVisibility(false, true);
	ScaleHelper_Cube_Z->SetVisibility(false, true);
	ScaleHelper_Plane_XY->SetVisibility(false, true);
	ScaleHelper_Plane_XZ->SetVisibility(false, true);
	ScaleHelper_Plane_YZ->SetVisibility(false, true);
	
	TranslationHelperGroup->SetAbsolute(true, false, true);
	RotationHelperGroup->SetAbsolute(false, true, true);
	
	ScaleHelperGroup->SetAbsolute(false, false, true);
	ScaleHelper_Cube_X->SetAbsolute(false, false, true);
	ScaleHelper_Cube_Y->SetAbsolute(false, false, true);
	ScaleHelper_Cube_Z->SetAbsolute(false, false, true);
	ScaleHelper_Plane_XY->SetAbsolute(false, false, true);
	ScaleHelper_Plane_XZ->SetAbsolute(false, false, true);
	ScaleHelper_Plane_YZ->SetAbsolute(false, false, true);
}

void ACommonGizmoTransform::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// if (const UCommonGizmoDeveloperSettings* Settings = GetDefault<UCommonGizmoDeveloperSettings>())
	// {
	// 	if (UStaticMesh *TranslationHandle = Settings->SM_TranslationHandle.LoadSynchronous())
	// 	{
	// 		TranslationHandle_X->SetStaticMesh(TranslationHandle);
	// 	}
	// 	
	// }
}

void ACommonGizmoTransform::OnUserStateOnChange(FGameplayTag newState, const FTransform& Transform, float Zoom,bool bblend)
{
	
}

void ACommonGizmoTransform::OnSystemStateOnChange(FGameplayTag newState)
{
	if (newState != CommonToolTags::State_Edit)
	{
		DeactivateGizmo();
	}
}

void ACommonGizmoTransform::OnPointerEnter_Implementation(UPrimitiveComponent* Component)
{
	HandlePointerEnter(Component);
}

void ACommonGizmoTransform::OnPointerLeave_Implementation(UPrimitiveComponent* Component)
{
	HandlePointerLeave(Component);
}

void ACommonGizmoTransform::OnPointerDrag_Implementation(UPrimitiveComponent* Component)
{
	FVector RayStart; FVector RayEnd;
	InteractiveSubsystem->GetPointerRay(RayStart,RayEnd);
	HandlePointerDrag(Component, RayStart, RayEnd);
}

bool ACommonGizmoTransform::OnPointerDown_Implementation(UPrimitiveComponent* Component)
{
	InteractiveSubsystem->SetLockHover(true);
	FVector RayStart; FVector RayEnd;
	InteractiveSubsystem->GetPointerRay(RayStart,RayEnd);
	HandlePointerDown(Component, RayStart, RayEnd);
	return true;
}

bool ACommonGizmoTransform::OnPointerUp_Implementation(UPrimitiveComponent* Component)
{
	HandlePointerUp(Component);
	InteractiveSubsystem->SetLockHover(false);
	return false;
}

int32 ACommonGizmoTransform::GetHitPriority_Implementation(UPrimitiveComponent* Component)
{
	TArray<UPrimitiveComponent*> Components;
	GizmoHandleMap.GetKeys(Components);
	if (!Components.Contains(Component))
	{
		return -99999;
	}
	FCommonGizmoHandle CommonGizmoHandle = GizmoHandleMap.FindRef(Component);
	return CommonGizmoHandle.HitPriority;
}

void ACommonGizmoTransform::HandlePointerEnter(UPrimitiveComponent* Component)
{
	TArray<UPrimitiveComponent*> Components;
	GizmoHandleMap.GetKeys(Components);
	if (!Components.Contains(Component))
	{
		return;
	}
	
	//UToolFunctionLibrary::Debug(FString::Printf(TEXT("HandlePointerEnter::%s"),*Component->GetName()));

	FCommonGizmoHandle CommonGizmoHandle = GizmoHandleMap.FindRef(Component);

	if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Translation || CommonGizmoHandle.TransformMode
		== CommonToolTags::TransformMode_Scale)
	{
		Component->SetMaterial(0, MI_Hovered);
	}
	if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Rotation)
	{
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_X || CommonGizmoHandle.ConstraintAxis ==
			CommonToolTags::ConstraintAxis_Y || CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Z)
		{
			Component->SetMaterial(0, MI_Rotation_Hovered);
		}
		else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XYZ || CommonGizmoHandle.
			ConstraintAxis == CommonToolTags::ConstraintAxis_Screen)
		{
			Component->SetMaterial(0, MI_ScreenRotation_Hovered);
		}
		else
		{
			
		}
	}
}

void ACommonGizmoTransform::HandlePointerLeave(UPrimitiveComponent* Component)
{
	TArray<UPrimitiveComponent*> Components;
	GizmoHandleMap.GetKeys(Components);
	if (!Components.Contains(Component))
	{
		return;
	}
	//UToolFunctionLibrary::Debug(FString::Printf(TEXT("HandlePointerLeave::%s"),*Component->GetName()));
	FCommonGizmoHandle CommonGizmoHandle = GizmoHandleMap.FindRef(Component);
	if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Translation || CommonGizmoHandle.TransformMode
		== CommonToolTags::TransformMode_Scale)
	{
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_X || CommonGizmoHandle.ConstraintAxis ==
			CommonToolTags::ConstraintAxis_YZ)
		{
			Component->SetMaterial(0, MI_XAxis);
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Y || CommonGizmoHandle.ConstraintAxis ==
			CommonToolTags::ConstraintAxis_XZ)
		{
			Component->SetMaterial(0, MI_YAxis);
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Z || CommonGizmoHandle.ConstraintAxis ==
			CommonToolTags::ConstraintAxis_XY)
		{
			Component->SetMaterial(0, MI_ZAxis);
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XYZ || CommonGizmoHandle.ConstraintAxis
			== CommonToolTags::ConstraintAxis_Screen)
		{
			Component->SetMaterial(0, MI_XYZAxis);
		}
	}
	if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Rotation)
	{
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_X)
		{
			Component->SetMaterial(0, MI_Rotation_XAxis);
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Y)
		{
			Component->SetMaterial(0, MI_Rotation_YAxis);
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Z)
		{
			Component->SetMaterial(0, MI_Rotation_ZAxis);
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XYZ)
		{
			Component->SetMaterial(0, M_ScreenRotation);
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Screen)
		{
			Component->SetMaterial(0, M_ScreenRotation);
		}
	}
}

void ACommonGizmoTransform::HandlePointerDown(UPrimitiveComponent* Component, FVector PointerRayStart,
                                              FVector PointerRayEnd)
{
	bIsDragging = true;
	TArray<UPrimitiveComponent*> Components;
	GizmoHandleMap.GetKeys(Components);
	if (!Components.Contains(Component))
	{
		return;
	}
	
	FCommonGizmoHandle CommonGizmoHandle = GizmoHandleMap.FindRef(Component);
	WorldLocationOnDragStart = WorldLocation;
	WorldRotationOnDragStart = WorldRotation;
	WorldScaleOnDragStart = WorldScale;
	AlignRotator = (AlignSpace == CommonToolTags::AlignSpace_Local || CommonGizmoHandle.TransformMode ==
		               CommonToolTags::TransformMode_Scale)
		               ? WorldRotationOnDragStart
		               : FRotator();

	FVector AlignVector = FVector(0, 0, 0);
	
	if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Scale || CommonGizmoHandle.TransformMode ==
		CommonToolTags::TransformMode_Translation)
	{
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_X)
		{
			AlignVector = UKismetMathLibrary::GetForwardVector(AlignRotator);
			DragConstraintPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(
				WorldLocation,
				UKismetMathLibrary::Cross_VectorVector(AlignVector,
				                                       UKismetMathLibrary::Cross_VectorVector(
					                                       AlignVector, GetDirectionToView())));
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Y)
		{
			AlignVector = UKismetMathLibrary::GetRightVector(AlignRotator);
			DragConstraintPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(
				WorldLocation,
				UKismetMathLibrary::Cross_VectorVector(AlignVector,
				                                       UKismetMathLibrary::Cross_VectorVector(
					                                       AlignVector, GetDirectionToView())));
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Z)
		{
			AlignVector = UKismetMathLibrary::GetUpVector(AlignRotator);
			DragConstraintPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(
				WorldLocation,
				UKismetMathLibrary::Cross_VectorVector(AlignVector,
				                                       UKismetMathLibrary::Cross_VectorVector(
					                                       AlignVector, GetDirectionToView())));
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XY)
		{
			AlignVector = UKismetMathLibrary::GetUpVector(AlignRotator);
			DragConstraintPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(WorldLocation, AlignVector);
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XZ)
		{
			AlignVector = UKismetMathLibrary::GetRightVector(AlignRotator);
			DragConstraintPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(WorldLocation, AlignVector);
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_YZ)
		{
			AlignVector = UKismetMathLibrary::GetForwardVector(AlignRotator);
			DragConstraintPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(WorldLocation, AlignVector);
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XYZ)
		{
			if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Scale)
			{
				AlignVector = UKismetMathLibrary::GetForwardVector(AlignRotator);
				DragConstraintPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(
					WorldLocation,
					UKismetMathLibrary::Cross_VectorVector(AlignVector,
					                                       UKismetMathLibrary::Cross_VectorVector(
						                                       AlignVector, GetDirectionToView())));
			}
		}
		if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Screen)
		{
			if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Translation)
			{
				AlignRotator = GetViewRotation();
				AlignVector = GetViewDirection();
				DragConstraintPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(WorldLocation, AlignVector);
			}
		}
	}
	else if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Rotation)
	{
		DragConstraintPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(WorldLocation, GetDirectionToView());
		DraggedAngle = 0.0f;
		RotationAngle = 0.0f;
	}
	float T = 0.0f;
	FVector Intersection;
	UKismetMathLibrary::LinePlaneIntersection(PointerRayStart, PointerRayEnd, DragConstraintPlane, T, Intersection);
	StartDragLocation = Intersection;
	LastDragLocation = Intersection;

	FTransform BeginTransform;
	BeginTransform.SetLocation(WorldLocation);
	BeginTransform.SetRotation(WorldRotation.Quaternion());
	BeginTransform.SetScale3D(WorldScale);
	OnBeginTransform.Broadcast(BeginTransform);
	
	if (bDrawHelperGizmo)
	{
		if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Translation)
		{
			TranslationHelperGroup->SetWorldLocation(WorldLocationOnDragStart);
			TranslationHelperGroup->SetWorldScale3D(FVector(GizmoWorldScale));
			
			if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_X)
			{
				TranslationHelper_X->SetVisibility(true);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Y)
			{
				TranslationHelper_Y->SetVisibility(true);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Z)
			{
				TranslationHelper_Z->SetVisibility(true);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XY)
			{
				TranslationHelper_X->SetVisibility(true);
				TranslationHelper_Y->SetVisibility(true);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XZ)
			{
				TranslationHelper_X->SetVisibility(true);
				TranslationHelper_Z->SetVisibility(true);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_YZ)
			{
				TranslationHelper_Y->SetVisibility(true);
				TranslationHelper_Z->SetVisibility(true);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Screen)
			{
				TranslationHelper_X->SetVisibility(true);
				TranslationHelper_Y->SetVisibility(true);
				TranslationHelper_Z->SetVisibility(true);
			}
			else
			{
				return;
			}
		}else

		if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Rotation)
		{
			if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_X)
			{
				AlignVector = UKismetMathLibrary::GetForwardVector(AlignRotator);
				AngleHelperMesh->SetWorldScale3D(FVector(GizmoWorldScale * 90));
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Y)
			{
				AlignVector = UKismetMathLibrary::GetRightVector(AlignRotator);
				AngleHelperMesh->SetWorldScale3D(FVector(GizmoWorldScale * 90));
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Z)
			{
				AlignVector = UKismetMathLibrary::GetUpVector(AlignRotator);
				AngleHelperMesh->SetWorldScale3D(FVector(GizmoWorldScale * 90));
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Screen)
			{
				AlignVector = GetDirectionToView();
				AngleHelperMesh->SetWorldScale3D(FVector(GizmoWorldScale * 110));
			}
			else
			{
				return;
			}
			
			UKismetMathLibrary::LinePlaneIntersection_OriginNormal(PointerRayStart, PointerRayEnd, WorldLocation,
			                                                       AlignVector, T, Intersection);
			AngleHelperMesh->SetWorldRotation(
				UKismetMathLibrary::MakeRotFromXZ(Intersection - WorldLocation, AlignVector));
			AngleHelperMesh->SetScalarParameterValueOnMaterials(FName("Angle"), 0);
			AngleHelperMesh->SetVisibility(true);
		}else

		if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Scale)
		{
			ScaleHelper_UnitLine_X->SetWorldScale3D(FVector(GizmoWorldScale * 117, GizmoWorldScale, GizmoWorldScale));
			ScaleHelper_UnitLine_X->SetWorldScale3D(FVector(GizmoWorldScale * 117, GizmoWorldScale, GizmoWorldScale));
			ScaleHelper_UnitLine_X->SetWorldScale3D(FVector(GizmoWorldScale * 117, GizmoWorldScale, GizmoWorldScale));
			
			ScaleHelper_Cube_X->SetWorldScale3D(FVector(GizmoWorldScale));
			ScaleHelper_Cube_Y->SetWorldScale3D(FVector(GizmoWorldScale));
			ScaleHelper_Cube_Z->SetWorldScale3D(FVector(GizmoWorldScale));
			
			ScaleHelperPlaneGroup->SetWorldScale3D(FVector(GizmoWorldScale * 80, GizmoWorldScale, GizmoWorldScale));
			ScaleHelper_Plane_XY->SetWorldScale3D(FVector(GizmoWorldScale));
			ScaleHelper_Plane_XZ->SetWorldScale3D(FVector(GizmoWorldScale));
			ScaleHelper_Plane_YZ->SetWorldScale3D(FVector(GizmoWorldScale));
			
			if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_X)
			{
				ScaleHelper_UnitLine_X->SetMaterial(0, MI_Hovered);
				ScaleHelper_UnitLine_X->SetVisibility(true);
				ScaleHelper_Cube_X->SetMaterial(0, MI_Hovered);
				ScaleHelper_Cube_X->SetVisibility(true);
				ScaleHandle_X->SetMaterial(0, MI_Ghost);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Y)
			{
				ScaleHelper_UnitLine_Y->SetMaterial(0, MI_Hovered);
				ScaleHelper_UnitLine_Y->SetVisibility(true);
				ScaleHelper_Cube_Y->SetMaterial(0, MI_Hovered);
				ScaleHelper_Cube_Y->SetVisibility(true);
				ScaleHandle_Y->SetMaterial(0, MI_Ghost);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Z)
			{
				ScaleHelper_UnitLine_Z->SetMaterial(0, MI_Hovered);
				ScaleHelper_UnitLine_Z->SetVisibility(true);
				ScaleHelper_Cube_Z->SetMaterial(0, MI_Hovered);
				ScaleHelper_Cube_Z->SetVisibility(true);
				ScaleHandle_Z->SetMaterial(0, MI_Ghost);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XY)
			{
				ScaleHelper_Plane_XY->SetMaterial(0, MI_Hovered);
				ScaleHelper_Plane_XY->SetVisibility(true);
				PlaneScaleHandle_XY->SetMaterial(0, MI_Ghost);
			
				ScaleHelper_UnitLine_X->SetMaterial(0, MI_Hovered);
				ScaleHelper_UnitLine_X->SetVisibility(true);
				ScaleHelper_Cube_X->SetMaterial(0, MI_Hovered);
				ScaleHelper_Cube_X->SetVisibility(true);
				ScaleHandle_X->SetMaterial(0, MI_Ghost);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XZ)
			{
				ScaleHelper_Plane_XZ->SetMaterial(0, MI_Hovered);
				ScaleHelper_Plane_XZ->SetVisibility(true);
				PlaneScaleHandle_XZ->SetMaterial(0, MI_Ghost);
			
				ScaleHelper_UnitLine_X->SetMaterial(0, MI_Hovered);
				ScaleHelper_UnitLine_X->SetVisibility(true);
				ScaleHelper_Cube_X->SetMaterial(0, MI_Hovered);
				ScaleHelper_Cube_X->SetVisibility(true);
				ScaleHandle_X->SetMaterial(0, MI_Ghost);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_YZ)
			{
				ScaleHelper_Plane_YZ->SetMaterial(0, MI_Hovered);
				ScaleHelper_Plane_YZ->SetVisibility(true);
				ScaleHelper_Plane_YZ->SetMaterial(0, MI_Ghost);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XYZ)
			{
				ScaleHelper_UnitLine_X->SetMaterial(0, MI_XAxis);
				ScaleHelper_UnitLine_X->SetVisibility(true);
				ScaleHelper_Cube_X->SetMaterial(0, MI_XAxis);
				ScaleHelper_Cube_X->SetVisibility(true);
				ScaleHandle_X->SetMaterial(0, MI_Ghost);
			
				ScaleHelper_UnitLine_Y->SetMaterial(0, MI_YAxis);
				ScaleHelper_UnitLine_Y->SetVisibility(true);
				ScaleHelper_Cube_Y->SetMaterial(0, MI_YAxis);
				ScaleHelper_Cube_Y->SetVisibility(true);
				ScaleHandle_Y->SetMaterial(0, MI_Ghost);
			
				ScaleHelper_UnitLine_Z->SetMaterial(0, MI_ZAxis);
				ScaleHelper_UnitLine_Z->SetVisibility(true);
				ScaleHelper_Cube_Z->SetMaterial(0, MI_ZAxis);
				ScaleHelper_Cube_Z->SetVisibility(true);
				ScaleHandle_Z->SetMaterial(0, MI_Ghost);
			
				ScaleHelper_Plane_XY->SetMaterial(0, M_Hidden);
				ScaleHelper_Plane_XZ->SetMaterial(0, M_Hidden);
				ScaleHelper_Plane_YZ->SetMaterial(0, M_Hidden);
			}
			else
			{
				
			}
		}
	}
}

void ACommonGizmoTransform::HandlePointerDrag(UPrimitiveComponent* Component, FVector PointerRayStart,
                                              FVector PointerRayEnd)
{
	FCommonGizmoHandle CommonGizmoHandle = GizmoHandleMap.FindRef(Component);

	float T = 0.0f;
	FVector Intersection;
	if (UKismetMathLibrary::LinePlaneIntersection(PointerRayStart, PointerRayEnd, DragConstraintPlane, T, Intersection))
	{
		FVector CurrentDragLocation = Intersection;
		FVector AlignVector = FVector(0.0f, 0.0f, 0.0f);

		if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Translation)
		{
			if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_X)
			{
				AlignVector = UKismetMathLibrary::GetForwardVector(AlignRotator);
				WorldLocation = WorldLocationOnDragStart + GetSnappedLocation(
					UKismetMathLibrary::ProjectVectorOnToVector(CurrentDragLocation - StartDragLocation, AlignVector));
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Y)
			{
				AlignVector = UKismetMathLibrary::GetRightVector(AlignRotator);
				WorldLocation = WorldLocationOnDragStart + GetSnappedLocation(
					UKismetMathLibrary::ProjectVectorOnToVector(CurrentDragLocation - StartDragLocation, AlignVector));
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Z)
			{
				AlignVector = UKismetMathLibrary::GetUpVector(AlignRotator);
				WorldLocation = WorldLocationOnDragStart + GetSnappedLocation(
					UKismetMathLibrary::ProjectVectorOnToVector(CurrentDragLocation - StartDragLocation, AlignVector));
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XY)
			{
				WorldLocation = WorldLocationOnDragStart + GetSnappedLocation(CurrentDragLocation - StartDragLocation);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XZ)
			{
				WorldLocation = WorldLocationOnDragStart + GetSnappedLocation(CurrentDragLocation - StartDragLocation);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_YZ)
			{
				WorldLocation = WorldLocationOnDragStart + GetSnappedLocation(CurrentDragLocation - StartDragLocation);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Screen)
			{
				WorldLocation = WorldLocationOnDragStart + GetSnappedLocation(CurrentDragLocation - StartDragLocation);
			}
			else
			{
				return;
			}
		}else
		if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Rotation)
		{
			if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_X)
			{
				AlignVector = UKismetMathLibrary::GetForwardVector(AlignRotator);
				RotationAngle = GetSnappedAngle(
					RotateFactor * (UKismetMathLibrary::Dot_VectorVector(
						UKismetMathLibrary::Cross_VectorVector(AlignVector, GetDirectionToView()),
						CurrentDragLocation - StartDragLocation) / GizmoWorldScale));
				WorldRotation = UKismetMathLibrary::ComposeRotators(WorldRotationOnDragStart,
				                                                    UKismetMathLibrary::RotatorFromAxisAndAngle(
					                                                    AlignVector, RotationAngle));
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Y)
			{
				AlignVector = UKismetMathLibrary::GetRightVector(AlignRotator);
				RotationAngle = GetSnappedAngle(
					RotateFactor * (UKismetMathLibrary::Dot_VectorVector(
						UKismetMathLibrary::Cross_VectorVector(AlignVector, GetDirectionToView()),
						CurrentDragLocation - StartDragLocation) / GizmoWorldScale));
				WorldRotation = UKismetMathLibrary::ComposeRotators(WorldRotationOnDragStart,
				                                                    UKismetMathLibrary::RotatorFromAxisAndAngle(
					                                                    AlignVector, RotationAngle));
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Z)
			{
				AlignVector = UKismetMathLibrary::GetUpVector(AlignRotator);
				RotationAngle = GetSnappedAngle(
					RotateFactor * (UKismetMathLibrary::Dot_VectorVector(
						UKismetMathLibrary::Cross_VectorVector(AlignVector, GetDirectionToView()),
						CurrentDragLocation - StartDragLocation) / GizmoWorldScale));
				WorldRotation = UKismetMathLibrary::ComposeRotators(WorldRotationOnDragStart,
				                                                    UKismetMathLibrary::RotatorFromAxisAndAngle(
					                                                    AlignVector, RotationAngle));
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XYZ)
			{
				AlignVector = UKismetMathLibrary::Normal(
					UKismetMathLibrary::Cross_VectorVector(CurrentDragLocation - StartDragLocation,
					                                       GetDirectionToView()), 0.0001);
				RotationAngle = GetSnappedAngle(
					RotateFactor * (UKismetMathLibrary::Dot_VectorVector(
						UKismetMathLibrary::Cross_VectorVector(AlignVector, GetDirectionToView()),
						CurrentDragLocation - StartDragLocation) / GizmoWorldScale));
				WorldRotation = UKismetMathLibrary::ComposeRotators(WorldRotationOnDragStart,
				                                                    UKismetMathLibrary::RotatorFromAxisAndAngle(
					                                                    AlignVector, RotationAngle));
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XYZ)
			{
				DraggedAngle = DraggedAngle +
					UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(
						UKismetMathLibrary::Normal(CurrentDragLocation - WorldLocation, 0.0001),
						UKismetMathLibrary::Normal(LastDragLocation - WorldLocation, 0.0001))) *
					UKismetMathLibrary::Dot_VectorVector(UKismetMathLibrary::Normal(
						                                     UKismetMathLibrary::Cross_VectorVector(
							                                     LastDragLocation - WorldLocation,
							                                     CurrentDragLocation - WorldLocation), 0.0001),
					                                     GetDirectionToView());
				WorldRotation = UKismetMathLibrary::ComposeRotators(WorldRotationOnDragStart,
				                                                    UKismetMathLibrary::RotatorFromAxisAndAngle(
					                                                    GetDirectionToView(), RotationAngle));
			}
			else
			{
				return;
			}
		}else
		if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Scale)
		{
			if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_X)
			{
				AlignVector = FVector(1.0, 0.0, 0.0);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Y)
			{
				AlignVector = FVector(0.0, 1.0, 0.0);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_Z)
			{
				AlignVector = FVector(0.0, 0.0, 1.0);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XY)
			{
				AlignVector = FVector(1.0, 1.0, 0.0);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XZ)
			{
				AlignVector = FVector(1.0, 0.0, 1.0);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_YZ)
			{
				AlignVector = FVector(0.0, 1.0, 1.0);
			}
			else if (CommonGizmoHandle.ConstraintAxis == CommonToolTags::ConstraintAxis_XYZ)
			{
				AlignVector = FVector(1.0, 1.0, 1.0);
			}
			else
			{
				return;
			}
			//WorldScale =WorldScale+;
			float num = UKismetMathLibrary::Dot_VectorVector(CurrentDragLocation - StartDragLocation,
			                                                 UKismetMathLibrary::Normal(
				                                                 UKismetMathLibrary::GreaterGreater_VectorRotator(
					                                                 AlignVector, AlignRotator),
				                                                 0.0001)) * ScaleFactor * 0.02;

			WorldScale = WorldScaleOnDragStart + (WorldScaleOnDragStart * AlignVector) * GetSnappedScale(
				(num / (WorldScaleOnDragStart * AlignVector).Length()));
		}

		LastDragLocation = CurrentDragLocation;
		UpdateGizmoScale();

		FTransform Transform;
		Transform.SetLocation(WorldLocation);
		Transform.SetRotation(WorldRotation.Quaternion());
		Transform.SetScale3D(WorldScale);

		OnUpdateTransform.Broadcast(Transform);

		if (bDrawHelperGizmo)
		{
			if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Translation)
			{
				TranslationHelperGroup->SetWorldLocation(WorldLocationOnDragStart);
			}
			if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Rotation)
			{
				if (CommonGizmoHandle.TransformMode == CommonToolTags::ConstraintAxis_X)
				{
					AngleHelperMesh->SetScalarParameterValueOnMaterials(FName(TEXT("Angle")), RotationAngle);
				}
				else if (CommonGizmoHandle.TransformMode == CommonToolTags::ConstraintAxis_Y)
				{
					AlignVector = FVector(0.0, 1.0, 0.0);
				}
				else if (CommonGizmoHandle.TransformMode == CommonToolTags::ConstraintAxis_Z)
				{
					AlignVector = FVector(0.0, 0.0, 1.0);
				}
				else if (CommonGizmoHandle.TransformMode == CommonToolTags::ConstraintAxis_Screen)
				{
					AlignVector = FVector(1.0, 1.0, 0.0);
				}
				else
				{
					return;
				}
			}
			if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Scale)
			{
				float num = UKismetMathLibrary::Dot_VectorVector(WorldScale - WorldScaleOnDragStart,
				                                                 UKismetMathLibrary::Normal(
					                                                 WorldScaleOnDragStart * AlignVector,
					                                                 0.0001)) / 0.02;
				ScaleHelper_UnitLine_X->SetWorldScale3D(FVector(GizmoWorldScale * 117  +num, GizmoWorldScale, GizmoWorldScale));
				ScaleHelper_UnitLine_Y->SetWorldScale3D(FVector(GizmoWorldScale * 117  +num, GizmoWorldScale, GizmoWorldScale));
				ScaleHelper_UnitLine_Z->SetWorldScale3D(FVector(GizmoWorldScale * 117  +num, GizmoWorldScale, GizmoWorldScale));
				
				ScaleHelperPlaneGroup->SetWorldScale3D(FVector(GizmoWorldScale * 80 + num));
			}
		}
	}
}

void ACommonGizmoTransform::HandlePointerUp(UPrimitiveComponent* Component)
{
	bIsDragging = false;
	TArray<UPrimitiveComponent*> Components;
	GizmoHandleMap.GetKeys(Components);
	if (!Components.Contains(Component))
	{
		return;
	}
	FCommonGizmoHandle CommonGizmoHandle = GizmoHandleMap.FindRef(Component);
	FTransform BeginTransform;
	BeginTransform.SetLocation(WorldLocation);
	BeginTransform.SetRotation(WorldRotation.Quaternion());
	BeginTransform.SetScale3D(WorldScale);
	OnFinishTransform.Broadcast(BeginTransform);

	if (bDrawHelperGizmo)
	{
		if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Translation)
		{
			TranslationHelperGroup->SetVisibility(false, true);
		}
		if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Rotation)
		{
			RotationHelperGroup->SetVisibility(false, true);
			AngleHelperMesh->SetVisibility(false, true);
		}
		if (CommonGizmoHandle.TransformMode == CommonToolTags::TransformMode_Scale)
		{
			ScaleHandle_X->SetMaterial(0, MI_XAxis);
			ScaleHandle_Y->SetMaterial(0, MI_YAxis);
			ScaleHandle_Z->SetMaterial(0, MI_ZAxis);

			PlaneScaleHandle_XY->SetMaterial(0, MI_ZAxis);
			PlaneScaleHandle_XZ->SetMaterial(0, MI_YAxis);
			PlaneScaleHandle_YZ->SetMaterial(0, MI_XAxis);

			ScaleHelperGroup->SetVisibility(false, true);
		}
	}
}

FVector ACommonGizmoTransform::GetViewLocation() const
{
	return UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
}

FRotator ACommonGizmoTransform::GetViewRotation() const
{
	return UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraRotation();
}

FVector ACommonGizmoTransform::GetViewDirection()
{
	return UKismetMathLibrary::GetForwardVector(GetViewRotation());
}

FVector ACommonGizmoTransform::GetDirectionToView()
{
	if (IsOrthographicView())
	{
		return UKismetMathLibrary::NegateVector(GetViewDirection());
	}
	return UKismetMathLibrary::GetDirectionUnitVector(WorldLocation, GetViewLocation());
}

bool ACommonGizmoTransform::IsOrthographicView() const
{
	if (UGameplayStatics::GetPlayerCameraManager(this, 0)->GetViewTarget())
	{
		return UGameplayStatics::GetPlayerCameraManager(this, 0)->GetViewTarget()->GetComponentByClass<
			UCameraComponent>()->ProjectionMode == ECameraProjectionMode::Type::Orthographic;
	}
	return false;
}

float ACommonGizmoTransform::GetOrthoWidth() const
{
	return UGameplayStatics::GetPlayerCameraManager(this, 0)->GetViewTarget()->GetComponentByClass<UCameraComponent>()->
	                                                          OrthoWidth;
}

float ACommonGizmoTransform::GetFieldOfView() const
{
	return UGameplayStatics::GetPlayerCameraManager(this, 0)->GetFOVAngle();
}

FVector ACommonGizmoTransform::GetSnappedLocation(FVector Location) const
{
	if (bEnableLocationSnap)
	{
		FTransform Transform = UKismetMathLibrary::Conv_RotatorToTransform(AlignRotator);
		return UKismetMathLibrary::TransformLocation(
			Transform, UKismetMathLibrary::Vector_SnappedToGrid(
				UKismetMathLibrary::InverseTransformLocation(Transform, Location), LocationGridSize));
	}
	return Location;
}

float ACommonGizmoTransform::GetSnappedAngle(float Angle) const
{
	if (bEnableRotationSnap)
	{
		return UKismetMathLibrary::GridSnap_Float(Angle, RotationGridSize);
	}
	return Angle;
}

float ACommonGizmoTransform::GetSnappedScale(float Scale) const
{
	if (bEnableScaleSnap)
	{
		return UKismetMathLibrary::GridSnap_Float(Scale, ScaleGridSize);
	}
	return Scale;
}

void ACommonGizmoTransform::OnActivateGizmo(AActor* TransformActor)
{
	if (TransformActor)
	{
		SelectedActor = TransformActor;
		FTransform Transform = TransformActor->GetTransform();
		
		// FVector Origin;
		// FVector BoxExtent;
		// TransformActor->GetActorBounds(false, Origin, BoxExtent);
		// Transform.SetLocation(Origin);
		
		ActivateGizmo(Transform);
	}
	else
	{
		DeactivateGizmo();
	}
}

void ACommonGizmoTransform::ActivateGizmo(const FTransform& InitialTransform)
{
	bIsActivated = true;
	OnBeginTransform.Clear();
	OnUpdateTransform.Clear();
	OnFinishTransform.Clear();
	
	SetActorTickEnabled(true);

	OnBeginTransform.AddDynamic(this, &ThisClass::OnSelectActorBeginTransform);
	OnUpdateTransform.AddDynamic(this, &ThisClass::OnSelectActorUpdateTransform);
	OnFinishTransform.AddDynamic(this,&ThisClass::OnSelectActorFinishTransform);
	WorldLocation = InitialTransform.GetLocation();
	WorldRotation = InitialTransform.Rotator();
	WorldScale = InitialTransform.GetScale3D();

	UpdateGizmoScale();

	SetTransformMode(ActiveTransformMode, bCombineTranslateRotate);
	
	SetAlignSpace(AlignSpace);

}

void ACommonGizmoTransform::DeactivateGizmo()
{
	bIsActivated = false;
	SetActiveGizmoGroup(false, false, false);
	SetActorTickEnabled(false);
	OnUpdateTransform.Clear();
}

void ACommonGizmoTransform::SetAlignSpace(FGameplayTag InAlignSpace)
{
	AlignSpace = InAlignSpace;

	if (bIsActivated)
	{
		FTransform Transform;
		Transform.SetLocation(WorldLocation);
		Transform.SetRotation(WorldRotation.Quaternion());
		Transform.SetScale3D(FVector(1));
		ApplyGizmoTransform(Transform);
	}
}

void ACommonGizmoTransform::SetTransformMode(FGameplayTag InTransformMode, bool CombineTranslateRotate)
{
	ActiveTransformMode = InTransformMode;
	bCombineTranslateRotate = CombineTranslateRotate;
	if (bIsActivated)
	{
		if (bCombineTranslateRotate)
		{
			SetActiveGizmoGroup(true, true, false);
		}
		else
		{
			if (ActiveTransformMode == CommonToolTags::TransformMode_Translation)
			{
				SetActiveGizmoGroup(true, false, false);
			}
			if (ActiveTransformMode == CommonToolTags::TransformMode_Rotation)
			{
				SetActiveGizmoGroup(false, true, false);
			}
			if (ActiveTransformMode == CommonToolTags::TransformMode_Scale)
			{
				SetActiveGizmoGroup(false, false, true);
			}
		}
	}
}

void ACommonGizmoTransform::SetTransform(FTransform InTransform)
{
	WorldLocation = InTransform.GetLocation();
	WorldRotation = InTransform.Rotator();
	WorldScale = InTransform.GetScale3D();

	FTransform Transform;
	Transform.SetLocation(WorldLocation);
	Transform.SetRotation(WorldRotation.Quaternion());
	Transform.SetScale3D(WorldScale);

	ApplyGizmoTransform(Transform);
}

void ACommonGizmoTransform::SetActiveGizmoGroup(bool bTranslation, bool bRotation, bool bScale) const
{
	TArray<USceneComponent*> ChildrenComponents;
	TranslationGizmoGroup->GetChildrenComponents(false, ChildrenComponents);

	ECollisionEnabled::Type NewType = bTranslation
		                                  ? ECollisionEnabled::Type::QueryOnly
		                                  : ECollisionEnabled::Type::NoCollision;
	for (auto Element : ChildrenComponents)
	{
		Element->SetVisibility(bTranslation);
		if (Cast<UPrimitiveComponent>(Element))
		{
			Cast<UPrimitiveComponent>(Element)->SetCollisionEnabled(NewType);
		}
	}
	RotationGizmoGroup->GetChildrenComponents(false, ChildrenComponents);
	NewType = bRotation ? ECollisionEnabled::Type::QueryOnly : ECollisionEnabled::Type::NoCollision;
	for (auto Element : ChildrenComponents)
	{
		Element->SetVisibility(bRotation);
		if (Cast<UPrimitiveComponent>(Element))
		{
			Cast<UPrimitiveComponent>(Element)->SetCollisionEnabled(NewType);
		}
	}
	ScaleGizmoGroup->GetChildrenComponents(false, ChildrenComponents);
	NewType = bScale ? ECollisionEnabled::Type::QueryOnly : ECollisionEnabled::Type::NoCollision;
	for (auto Element : ChildrenComponents)
	{
		Element->SetVisibility(bScale);
		if (Cast<UPrimitiveComponent>(Element))
		{
			Cast<UPrimitiveComponent>(Element)->SetCollisionEnabled(NewType);
		}
	}
}

void ACommonGizmoTransform::UpdateGizmoScale()
{
	if (IsOrthographicView())
	{
		GizmoWorldScale = 0.0005 * GetOrthoWidth() * (bScaledByDPI
			                                              ? GizmoScreenSize / UWidgetLayoutLibrary::GetViewportScale(
				                                              this)
			                                              : GizmoScreenSize);
	}
	else
	{
		GizmoWorldScale = UKismetMathLibrary::Dot_VectorVector(GetViewDirection(), WorldLocation - GetViewLocation()) *
			0.001 * (bScaledByDPI ? GizmoScreenSize / UWidgetLayoutLibrary::GetViewportScale(this) : GizmoScreenSize) *
			UKismetMathLibrary::DegTan(GetFieldOfView() * 0.5);
	}
	SetActorScale3D(FVector(GizmoWorldScale));
}

void ACommonGizmoTransform::InitializeGizmoHandleMap()
{
	GizmoHandleMap.Empty();

	GizmoHandleMap.Add(TranslationHandle_X, FCommonGizmoHandle(CommonToolTags::TransformMode_Translation,
	                                                           CommonToolTags::ConstraintAxis_X,
	                                                           BaseInteractionPriority + 3));
	GizmoHandleMap.Add(TranslationHandle_Y, FCommonGizmoHandle(CommonToolTags::TransformMode_Translation,
	                                                           CommonToolTags::ConstraintAxis_Y,
	                                                           BaseInteractionPriority + 3));
	GizmoHandleMap.Add(TranslationHandle_Z, FCommonGizmoHandle(CommonToolTags::TransformMode_Translation,
	                                                           CommonToolTags::ConstraintAxis_Z,
	                                                           BaseInteractionPriority + 3));
	GizmoHandleMap.Add(PlaneTranslationHandle_XY,
	                   FCommonGizmoHandle(CommonToolTags::TransformMode_Translation, CommonToolTags::ConstraintAxis_XY,
	                                      BaseInteractionPriority + 2));
	GizmoHandleMap.Add(PlaneTranslationHandle_XZ,
	                   FCommonGizmoHandle(CommonToolTags::TransformMode_Translation, CommonToolTags::ConstraintAxis_XZ,
	                                      BaseInteractionPriority + 2));
	GizmoHandleMap.Add(PlaneTranslationHandle_YZ,
	                   FCommonGizmoHandle(CommonToolTags::TransformMode_Translation, CommonToolTags::ConstraintAxis_YZ,
	                                      BaseInteractionPriority + 2));
	GizmoHandleMap.Add(ScreenTranslationHandle, FCommonGizmoHandle(CommonToolTags::TransformMode_Translation,
	                                                               CommonToolTags::ConstraintAxis_Screen,
	                                                               BaseInteractionPriority + 4));

	GizmoHandleMap.Add(RotationHandle_X, FCommonGizmoHandle(CommonToolTags::TransformMode_Rotation,
	                                                        CommonToolTags::ConstraintAxis_X,
	                                                        BaseInteractionPriority + 1));
	GizmoHandleMap.Add(RotationHandle_Y, FCommonGizmoHandle(CommonToolTags::TransformMode_Rotation,
	                                                        CommonToolTags::ConstraintAxis_Y,
	                                                        BaseInteractionPriority + 1));
	GizmoHandleMap.Add(RotationHandle_Z, FCommonGizmoHandle(CommonToolTags::TransformMode_Rotation,
	                                                        CommonToolTags::ConstraintAxis_Z,
	                                                        BaseInteractionPriority + 1));
	GizmoHandleMap.Add(TrackballRotationHandle, FCommonGizmoHandle(CommonToolTags::TransformMode_Rotation,
	                                                               CommonToolTags::ConstraintAxis_XYZ,
	                                                               BaseInteractionPriority + 1));
	GizmoHandleMap.Add(ScreenRotationHandle, FCommonGizmoHandle(CommonToolTags::TransformMode_Rotation,
	                                                            CommonToolTags::ConstraintAxis_Screen,
	                                                            BaseInteractionPriority));

	GizmoHandleMap.Add(ScaleHandle_X, FCommonGizmoHandle(CommonToolTags::TransformMode_Scale,
	                                                     CommonToolTags::ConstraintAxis_X,
	                                                     BaseInteractionPriority + 1));
	GizmoHandleMap.Add(ScaleHandle_Y, FCommonGizmoHandle(CommonToolTags::TransformMode_Scale,
	                                                     CommonToolTags::ConstraintAxis_Y,
	                                                     BaseInteractionPriority + 1));
	GizmoHandleMap.Add(ScaleHandle_Z, FCommonGizmoHandle(CommonToolTags::TransformMode_Scale,
	                                                     CommonToolTags::ConstraintAxis_Z,
	                                                     BaseInteractionPriority + 1));
	GizmoHandleMap.Add(PlaneScaleHandle_XY,
	                   FCommonGizmoHandle(CommonToolTags::TransformMode_Scale, CommonToolTags::ConstraintAxis_XY,
	                                      BaseInteractionPriority));
	GizmoHandleMap.Add(PlaneScaleHandle_XZ,
	                   FCommonGizmoHandle(CommonToolTags::TransformMode_Scale, CommonToolTags::ConstraintAxis_XZ,
	                                      BaseInteractionPriority));
	GizmoHandleMap.Add(PlaneScaleHandle_YZ,
	                   FCommonGizmoHandle(CommonToolTags::TransformMode_Scale, CommonToolTags::ConstraintAxis_YZ,
	                                      BaseInteractionPriority));
	GizmoHandleMap.Add(UniformScaleHandle, FCommonGizmoHandle(CommonToolTags::TransformMode_Scale,
	                                                          CommonToolTags::ConstraintAxis_XYZ,
	                                                          BaseInteractionPriority + 2));
}

void ACommonGizmoTransform::ApplyGizmoTransform(FTransform InTransform)
{
	if (AlignSpace == CommonToolTags::AlignSpace_Local)
	{
		SetActorLocationAndRotation(InTransform.GetLocation(), InTransform.GetRotation());
	}
	else if (AlignSpace == CommonToolTags::AlignSpace_World)
	{
		SetActorLocationAndRotation(InTransform.GetLocation(), FRotator(0));
	}
	ScaleGizmoGroup->SetWorldRotation(InTransform.GetRotation());
}

void ACommonGizmoTransform::EnableSnap(bool bEnableSnap)
{
	bEnableLocationSnap = bEnableSnap;
	bEnableRotationSnap = bEnableSnap;
	bEnableScaleSnap = bEnableSnap;
}

void ACommonGizmoTransform::OnSelectActorBeginTransform(FTransform Transform)
{
	
}

void ACommonGizmoTransform::OnSelectActorUpdateTransform(FTransform Transform)
{
	if (SelectedActor)
	{
		TEnumAsByte<EComponentMobility::Type> Mobility = SelectedActor->GetRootComponent()->Mobility;
		SelectedActor->GetRootComponent()->SetMobility(EComponentMobility::Movable);
		SelectedActor->SetActorTransform(Transform);
		SelectedActor->GetRootComponent()->SetMobility(Mobility);
		ApplyGizmoTransform(Transform);
	}
}


void ACommonGizmoTransform::OnSelectActorFinishTransform(FTransform Transform)
{
	if (SelectedActor)
	{
		if (SelectedActor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
		{
			Cast<IDeviceInterface>(SelectedActor)->GetOnOnActorReDrawDelegateChecked().Broadcast();
		}
	}
}
void ACommonGizmoTransform::OnUISectionChange(const FEditItem& Item, bool bSelected)
{
	if (bSelected)
	{
		DeactivateGizmo();
	}
	else
	{
		
	}
}

// Called when the game starts or when spawned
void ACommonGizmoTransform::BeginPlay()
{
	Super::BeginPlay();

	DeactivateGizmo();

	InteractiveSubsystem = GetWorld()->GetSubsystem<UInteractiveSubsystem>();
	if (InteractiveSubsystem)
	{
		InteractiveSubsystem->OnActivateGizmo.AddDynamic(this, &ThisClass::OnActivateGizmo);
		InteractiveSubsystem->OnTransformModeChange.AddDynamic(this, &ThisClass::SetTransformMode);
		InteractiveSubsystem->OnAlignSpaceChange.AddDynamic(this, &ThisClass::SetAlignSpace);
		InteractiveSubsystem->OnUISectionChange.AddDynamic(this, &ThisClass::OnUISectionChange);
	}
	StateSubsystem = GetGameInstance()->GetSubsystem<UStateSubsystem>();
	
	if (StateSubsystem)
	{
		StateSubsystem->UserStateOnChange.AddDynamic(this, &ThisClass::OnUserStateOnChange);
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);
	}
}

// Called every frame
void ACommonGizmoTransform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDragging)
	{
	}
	else
	{
		UpdateGizmoScale();
	}
}
