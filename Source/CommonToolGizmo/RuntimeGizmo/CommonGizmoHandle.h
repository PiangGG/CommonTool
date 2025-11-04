#pragma once

#include "CoreMinimal.h"
#include "CommonToolCoreTags.h"
#include "GameplayTagContainer.h"
#include "CommonGizmoHandle.generated.h"

USTRUCT(BlueprintType)
struct COMMONTOOLGIZMO_API FCommonGizmoHandle
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
