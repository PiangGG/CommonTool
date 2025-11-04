// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CommonCameraManager.generated.h"

/**
 * 
 */
#define CAMERA_DEFAULT_FOV			(80.0f)
#define CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define CAMERA_DEFAULT_PITCH_MAX	(89.0f)

class UCameraModeStack;

UCLASS()
class COMMONTOOLCORE_API ACommonCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ACommonCameraManager(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
};
