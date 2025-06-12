#pragma once

#include "CoreMinimal.h"
#include "CommonToolTags.h"
#include "GameplayTagContainer.h"
#include "CommonGizmoHandle.generated.h"

USTRUCT(BlueprintType)
struct COMMONTOOL_API FCommonGizmoHandle
{
	GENERATED_BODY()

	FCommonGizmoHandle();

	FCommonGizmoHandle(FGameplayTag Mode, FGameplayTag Axis, int32 Priority)
	{
		TransformMode = Mode;
		ConstraintAxis = Axis;
		HitPriority = Priority;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FGameplayTag TransformMode = CommonToolTags::TransformMode_Translation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	FGameplayTag ConstraintAxis = CommonToolTags::ConstraintAxis_X;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gizmo|Set", meta = (AllowPrivateAccess = "true"))
	int32 HitPriority = 0;
};

USTRUCT(BlueprintType)
struct COMMONTOOL_API FInteractionSettings
{
	GENERATED_BODY()

	FInteractionSettings()
	{
	}

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
