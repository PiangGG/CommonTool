// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonCameraManager.h"

#include "CameraModeStack.h"
#include "CommonCameraComponent.h"
#include "Engine/Canvas.h"

ACommonCameraManager::ACommonCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = CAMERA_DEFAULT_FOV;
	ViewPitchMin = CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = CAMERA_DEFAULT_PITCH_MAX;
}

void ACommonCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void ACommonCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("PlayerCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	if (const UCommonCameraComponent* CameraComponent = UCommonCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}
