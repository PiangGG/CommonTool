// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraModeStack.generated.h"

/**
 * 
 */
class UCameraMode;
struct FCameraModeView;

UCLASS()
class COMMONTOOLCORE_API UCameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	UCameraModeStack();

	void ActivateStack();
	void DeactivateStack();

	bool IsStackActivate() const { return bIsActive; }

	void PushCameraMode(TSubclassOf<UCameraMode> CameraModeClass);

	bool EvaluateStack(float DeltaTime, FCameraModeView& OutCameraModeView);

	void DrawDebug(UCanvas* Canvas) const;

	UCameraMode* GetActiveCameraMode() const;

protected:
	UCameraMode* GetCameraModeInstance(TSubclassOf<UCameraMode> CameraModeClass);

	void UpdateStack(float DeltaTime);
	void BlendStack(FCameraModeView& OutCameraModeView) const;

	bool bIsActive;

	UPROPERTY()
	TArray<TObjectPtr<UCameraMode>> CameraModeInstances;

	// Camera Mode 堆栈
	UPROPERTY()
	TArray<TObjectPtr<UCameraMode>> CameraModeStack;
};
