// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonToolTags.h"
#include "GameplayTagContainer.h"
#include "CommonGizmoHandle.h"
#include "CommonGizmoInterface.h"
#include "CommonTool/SubSystem/InteractiveSubsystem.h"
#include "GameFramework/Actor.h"
#include "CommonGizmoTransform.generated.h"

class UInteractiveSubsystem;

struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCommonGizmoTransform, FTransform, Transform);

UCLASS()
class COMMONTOOL_API ACommonGizmoTransform : public AActor, public ICommonGizmoInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACommonGizmoTransform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnUserStateOnChange(FGameplayTag newState, const FTransform& Transform, float Zoom,bool bblend);

	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag newState);
	
	/*
	 * ICommonGizmoInterface
	 */
	virtual void OnPointerEnter_Implementation(UPrimitiveComponent* Component) override;
	virtual void OnPointerLeave_Implementation(UPrimitiveComponent* Component) override;
	virtual void OnPointerDrag_Implementation(UPrimitiveComponent* Component) override;
	virtual bool OnPointerDown_Implementation(UPrimitiveComponent* Component) override;
	virtual bool OnPointerUp_Implementation(UPrimitiveComponent* Component) override;
	virtual int32 GetHitPriority_Implementation(UPrimitiveComponent* Component) override;
	//handler
	UFUNCTION(Category=handler)
	void HandlePointerEnter(UPrimitiveComponent* Component);
	UFUNCTION(Category=handler)
	void HandlePointerLeave(UPrimitiveComponent* Component);
	UFUNCTION(Category=handler)
	void HandlePointerDown(UPrimitiveComponent* Component, FVector PointerRayStart, FVector PointerRayEnd);
	UFUNCTION(Category=handler)
	void HandlePointerDrag(UPrimitiveComponent* Component, FVector PointerRayStart, FVector PointerRayEnd);
	UFUNCTION(Category=handler)
	void HandlePointerUp(UPrimitiveComponent* Component);

	//Helpers
	UFUNCTION(Category=Helpers)
	FVector GetViewLocation() const;
	UFUNCTION(Category=Helpers)
	FRotator GetViewRotation() const;
	UFUNCTION(Category=Helpers)
	FVector GetViewDirection();
	UFUNCTION(Category=Helpers)
	FVector GetDirectionToView();
	UFUNCTION(Category=Helpers)
	bool IsOrthographicView() const;
	UFUNCTION(Category=Helpers)
	float GetOrthoWidth() const;
	UFUNCTION(Category=Helpers)
	float GetFieldOfView() const;
	UFUNCTION(Category=Helpers)
	FVector GetSnappedLocation(FVector Location) const;
	UFUNCTION(Category=Helpers)
	float GetSnappedAngle(float Angle) const;
	UFUNCTION(Category=Helpers)
	float GetSnappedScale(float Scale) const;

	UFUNCTION()
	void OnActivateGizmo(AActor* TransformActor);
	
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void ActivateGizmo(const FTransform& InitialTransform);
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void DeactivateGizmo();
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void SetAlignSpace(FGameplayTag InAlignSpace);
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void SetTransformMode(FGameplayTag InTransformMode, bool CombineTranslateRotate);
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void SetTransform(FTransform InTransform);
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void SetActiveGizmoGroup(bool bTranslation, bool bRotation, bool bScale) const;
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void UpdateGizmoScale();
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void InitializeGizmoHandleMap();
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void ApplyGizmoTransform(FTransform InTransform);
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Gizmo")
	void EnableSnap(bool bEnableSnap);
	UFUNCTION()
	void OnSelectActorBeginTransform(FTransform Transform);
	UFUNCTION()
	void OnSelectActorUpdateTransform(FTransform Transform);
	UFUNCTION()
	void OnSelectActorFinishTransform(FTransform Transform);
	FOnCommonGizmoTransform OnBeginTransform;
	FOnCommonGizmoTransform OnUpdateTransform;
	FOnCommonGizmoTransform OnFinishTransform;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* TransformGizmoGroup;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* TranslationGizmoGroup;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TranslationHandle_X;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TranslationHandle_Y;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TranslationHandle_Z;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlaneTranslationHandle_YZ;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* TranslationHelperGroup;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TranslationHelper_X;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TranslationHelper_Y;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TranslationHelper_Z;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScreenTranslationHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlaneTranslationHandle_XY;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlaneTranslationHandle_XZ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RotationGizmoGroup;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* RotationHandle_X;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* RotationHandle_Y;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* RotationHandle_Z;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScreenRotationHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TrackballRotationHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RotationHelperGroup;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AngleHelperMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ScaleGizmoGroup;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHandle_X;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHandle_Y;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHandle_Z;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlaneScaleHandle_XY;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlaneScaleHandle_XZ;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlaneScaleHandle_YZ;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* UniformScaleHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ScaleHelperGroup;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHelper_UnitLine_X;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHelper_Cube_X;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHelper_UnitLine_Y;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHelper_Cube_Y;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHelper_UnitLine_Z;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHelper_Cube_Z;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ScaleHelperPlaneGroup;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHelper_Plane_XY;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHelper_Plane_XZ;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonLibrary|Gizmo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScaleHelper_Plane_YZ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	int32 BaseInteractionPriority = 100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	float GizmoScreenSize = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	bool bScaledByDPI = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	bool bDrawHelperGizmo = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	bool bEnableLocationSnap = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	bool bEnableRotationSnap = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	bool bEnableScaleSnap = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	float LocationGridSize = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	float RotationGridSize = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	float ScaleGridSize = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FGameplayTag AlignSpace = CommonToolTags::AlignSpace_Local;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FGameplayTag ActiveTransformMode = CommonToolTags::TransformMode_Translation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	bool bCombineTranslateRotate = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	float RotateFactor = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	float ScaleFactor = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	TMap<class UPrimitiveComponent*, struct FCommonGizmoHandle> GizmoHandleMap;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FPlane DragConstraintPlane;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FVector StartDragLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FVector LastDragLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FVector WorldLocationOnDragStart;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FRotator WorldRotationOnDragStart;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FVector WorldScaleOnDragStart;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FRotator AlignRotator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FVector WorldLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FRotator WorldRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FVector WorldScale;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	float GizmoWorldScale = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	bool bIsActivated = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	bool bIsDragging = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	float RotationAngle = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	float DraggedAngle = 0.0f;

	UFUNCTION()
	void OnUISectionChange(const FEditItem& Item, bool bSelected);
private:
	UPROPERTY()
	UStaticMesh* SM_AngleHelper;
	UPROPERTY()
	UStaticMesh* SM_LineHelper;
	UPROPERTY()
	UStaticMesh* SM_PlaneHandle;
	UPROPERTY()
	UStaticMesh* SM_RotationHandle;
	UPROPERTY()
	UStaticMesh* SM_RotationHandleCollision;
	UPROPERTY()
	UStaticMesh* SM_ScaleHandle;
	UPROPERTY()
	UStaticMesh* SM_ScaleHelper_Cube;
	UPROPERTY()
	UStaticMesh* SM_ScaleHelper_Plane;
	UPROPERTY()
	UStaticMesh* SM_ScaleHelper_UnitLine;
	UPROPERTY()
	UStaticMesh* SM_ScreenRotationHandle;
	UPROPERTY()
	UStaticMesh* SM_ScreenTranslationHandle;
	UPROPERTY()
	UStaticMesh* SM_TrackballRotationHandle;
	UPROPERTY()
	UStaticMesh* SM_TranslationHandle;
	UPROPERTY()
	UStaticMesh* SM_UniformScaleHandle;

	UPROPERTY()
	UMaterialInterface* M_AngleHelper;
	UPROPERTY()
	UMaterialInterface* M_GizmoAxis;
	UPROPERTY()
	UMaterialInterface* M_Hidden;
	UPROPERTY()
	UMaterialInterface* M_ScreenRotation;
	UPROPERTY()
	UMaterialInterface* MI_Ghost;
	UPROPERTY()
	UMaterialInterface* MI_Hovered;
	UPROPERTY()
	UMaterialInterface* MI_Rotation_Hovered;
	UPROPERTY()
	UMaterialInterface* MI_Rotation_XAxis;
	UPROPERTY()
	UMaterialInterface* MI_Rotation_YAxis;
	UPROPERTY()
	UMaterialInterface* MI_Rotation_ZAxis;
	UPROPERTY()
	UMaterialInterface* MI_ScreenRotation_Hovered;
	UPROPERTY()
	UMaterialInterface* MI_XAxis;
	UPROPERTY()
	UMaterialInterface* MI_XAxis_Ghost;
	UPROPERTY()
	UMaterialInterface* MI_XYZAxis;
	UPROPERTY()
	UMaterialInterface* MI_YAxis;
	UPROPERTY()
	UMaterialInterface* MI_YAxis_Ghost;
	UPROPERTY()
	UMaterialInterface* MI_ZAxis;
	UPROPERTY()
	UMaterialInterface* MI_ZAxis_Ghost;

	UPROPERTY()
	UInteractiveSubsystem *InteractiveSubsystem;
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	
	UPROPERTY()
	AActor* SelectedActor;
	UPROPERTY()
	FVector SelectedActorOrigin;
};