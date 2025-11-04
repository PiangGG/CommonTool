// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraMode.generated.h"

class UCommonCameraComponent;
/**
 * ECameraModeBlendFunction
 *
 *	Blend function used for transitioning between camera modes.
 */
UENUM(BlueprintType)
enum class ECameraModeBlendFunction : uint8
{
	// 进行简单的线性插值
	Linear,

	// 立即加速，但平稳减速到目标。缓量由指数控制。
	EaseIn,

	// 平稳加速，但不会减速到目标。缓量由指数控制。
	EaseOut,

	// 平稳加速和减速。缓量由指数控制。
	EaseInOut,

	COUNT UMETA(Hidden)
};

/**
 *	查看由用于混合相机模式的相机模式生成的数据。
 */
struct FCameraModeView
{
	FCameraModeView();

	void Blend(const FCameraModeView& Other, float OtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class COMMONTOOLCORE_API UCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UCameraMode();

	UCommonCameraComponent* GetCommonCameraComponent() const;

	virtual UWorld* GetWorld() const override;

	AActor* GetTargetActor() const;

	const FCameraModeView& GetCameraModeView() const { return View; }

	// 在相机模式堆栈上激活此相机模式时调用。
	virtual void OnActivation()
	{
	};

	// 当相机模式堆栈上的此相机模式被禁用时调用。
	virtual void OnDeactivation()
	{
	};

	void UpdateCameraMode(float DeltaTime);

	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float Weight);

	virtual void DrawDebug(UCanvas* Canvas) const;

	UFUNCTION(BlueprintCallable)
	virtual void ZoomIn();
	UFUNCTION(BlueprintCallable)
	virtual void ZoomAway();
	UFUNCTION(BlueprintCallable)
	virtual void SetZoom(float NewZoom);
	UFUNCTION(BlueprintCallable)
	virtual float GetZoom();

protected:
	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;

	virtual FVector GetPivotOffsetLocation() const;
	virtual FRotator GetPivotOffsetRotation() const;

	virtual void UpdateView(float DeltaTime);
	virtual void UpdateBlending(float DeltaTime);
	UPROPERTY(EditDefaultsOnly, Category = "View")
	float ZoomSize = 100.0f;

	// View output produced by the camera mode.
	FCameraModeView View;

	// The horizontal field of view (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View",
		Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	// Minimum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View",
		Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	// Maximum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View",
		Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	// How long it takes to blend in this mode.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	// Function used for blending.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	ECameraModeBlendFunction BlendFunction = ECameraModeBlendFunction::EaseInOut;

	// Exponent used by blend functions to control the shape of the curve.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	// Linear blend alpha used to determine the blend weight.
	float BlendAlpha;

	// Blend weight calculated using the blend alpha and function.
	float BlendWeight;
	UPROPERTY(EditDefaultsOnly, Category = "Camera|Offset")
	FVector PivotOffsetLocation;
	UPROPERTY(EditDefaultsOnly, Category = "Camera|Offset")
	FRotator PivotOffsetRotation;
	/** If true, skips all interpolation and puts camera in ideal location.  Automatically set to false next frame. */
	UPROPERTY(transient)
	uint32 bResetInterpolation : 1;
};
