// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CommonCameraComponent.generated.h"

/**
 * 
 */
class UCameraModeStack;
class UCameraMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FZoomChangeSignature,const float&,ZoomVal);

DECLARE_DELEGATE_RetVal(TSubclassOf<UCameraMode>, FCameraModeDelegate);

UCLASS()
class COMMONTOOLCORE_API UCommonCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UCommonCameraComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the camera component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Common|Camera")
	static UCommonCameraComponent* FindCameraComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UCommonCameraComponent>() : nullptr);
	}

	// Returns the target actor that the camera is looking at.
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// Delegate used to query for the best camera mode.
	FCameraModeDelegate DetermineCameraModeDelegate;

	// Add an offset to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

	virtual void DrawDebug(UCanvas* Canvas) const;

	UFUNCTION(BlueprintCallable, Category = "Common|Camera")
	void PushCameraMode(TSubclassOf<UCameraMode> CameraMode);

	UFUNCTION(BlueprintCallable, Category = "Common|Camera")
	void PopCameraMode();

	UFUNCTION()
	TSubclassOf<UCameraMode> GetActiveCameraMode();

	UFUNCTION(BlueprintCallable)
	virtual void SetIsUseOld(bool UseOld);
	UFUNCTION(BlueprintCallable)
	virtual void ZoomIn();
	UFUNCTION(BlueprintCallable)
	virtual void ZoomAway();
	UFUNCTION(BlueprintCallable)
	virtual void SetZoom(float NewZoom);
	UFUNCTION(BlueprintCallable)
	virtual float GetZoomSize();

	UFUNCTION(BlueprintCallable)
	virtual float GetPreviousZoomSize();

	UPROPERTY(BlueprintAssignable)
	FZoomChangeSignature ZoomChangeSignature;
protected:
	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void UpdateCameraModes();

	// Stack used to blend the camera modes.
	UPROPERTY()
	TObjectPtr<UCameraModeStack> CameraModeStack;

	UPROPERTY()
	TSubclassOf<UCameraMode> ActiveCameraMode;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	float FieldOfViewOffset;

	float PreviousZoomSize = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	float ZoomSize = 100.0f;
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category = "View")
	float ZoomSizeMin = 100.0;
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category = "View")
	float ZoomSizeMax = 100000.0;
	
	UPROPERTY()
	bool bUseOld;
};
