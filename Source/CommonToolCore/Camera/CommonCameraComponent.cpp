// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonCameraComponent.h"

#include "CameraMode.h"
#include "CameraModeStack.h"
#include "Engine/Canvas.h"
#include "GameFramework/SpringArmComponent.h"

UCommonCameraComponent::UCommonCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraModeStack = nullptr;
	FieldOfViewOffset = 0.0f;
}

void UCommonCameraComponent::DrawDebug(UCanvas* Canvas) const
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CommonCameraComponent: %s"), *GetNameSafe(GetTargetActor())));

	DisplayDebugManager.SetDrawColor(FColor::White);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("   Location: %s"), *GetComponentLocation().ToCompactString()));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("   Rotation: %s"), *GetComponentRotation().ToCompactString()));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("   FOV: %f"), FieldOfView));

	check(CameraModeStack);
	CameraModeStack->DrawDebug(Canvas);
}

void UCommonCameraComponent::PushCameraMode(TSubclassOf<UCameraMode> CameraMode)
{
	if (!bUseOld)
	{
		PreviousZoomSize = GetZoomSize();

		if (CameraMode->IsValidLowLevel())
		{
			ActiveCameraMode = CameraMode;
		}
	}
}

void UCommonCameraComponent::PopCameraMode()
{
	if (!bUseOld)
	{
		ActiveCameraMode = nullptr;
	}
}

TSubclassOf<UCameraMode> UCommonCameraComponent::GetActiveCameraMode()
{
	return ActiveCameraMode;
}

void UCommonCameraComponent::SetIsUseOld(bool UseOld)
{
	bUseOld = UseOld;
}

void UCommonCameraComponent::ZoomIn()
{
	if (bUseOld)
	{
		float ZoomSizeRate = ZoomSize/10.0f;
		ZoomSize += ZoomSizeRate;
		ZoomSize = FMath::Clamp(ZoomSize, ZoomSizeMin, ZoomSizeMax);
		PreviousZoomSize = ZoomSize;
		Cast<USpringArmComponent>(GetAttachParent())->TargetArmLength = PreviousZoomSize;
	}
	else
	{
		CameraModeStack->GetActiveCameraMode()->ZoomIn();
		PreviousZoomSize = GetZoomSize();
	}
	ZoomChangeSignature.Broadcast(PreviousZoomSize);
}

void UCommonCameraComponent::ZoomAway()
{
	if (bUseOld)
	{
		float ZoomSizeRate = ZoomSize/10.0f;
		ZoomSize -= ZoomSizeRate;
		ZoomSize = FMath::Clamp(ZoomSize, ZoomSizeMin, ZoomSizeMax);
		PreviousZoomSize = ZoomSize;
		Cast<USpringArmComponent>(GetAttachParent())->TargetArmLength = PreviousZoomSize;
	}else
	{
		CameraModeStack->GetActiveCameraMode()->ZoomAway();
		PreviousZoomSize = GetZoomSize();
	}
	ZoomChangeSignature.Broadcast(PreviousZoomSize);
}

void UCommonCameraComponent::SetZoom(float NewZoom)
{
	if (bUseOld)
	{
		ZoomSize = NewZoom;
	}
	else
	{
		if (CameraModeStack->GetActiveCameraMode())
		{
			CameraModeStack->GetActiveCameraMode()->SetZoom(NewZoom);
		}
	}
	PreviousZoomSize = NewZoom;
	Cast<USpringArmComponent>(GetAttachParent())->TargetArmLength = PreviousZoomSize;
}

float UCommonCameraComponent::GetZoomSize()
{
	if (bUseOld)
	{
		return PreviousZoomSize;
	}
	else
	{
		if (CameraModeStack->GetActiveCameraMode())
		{
			return CameraModeStack->GetActiveCameraMode()->GetZoom();
		}
	}
	return 0.0;
}

float UCommonCameraComponent::GetPreviousZoomSize()
{
	return PreviousZoomSize;
}

void UCommonCameraComponent::OnRegister()
{
	Super::OnRegister();

	if (!bUseOld)
	{
		if (!CameraModeStack)
		{
			CameraModeStack = NewObject<UCameraModeStack>(this);
			check(CameraModeStack);
		}
	}
}

void UCommonCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	check(CameraModeStack);

	if (bUseOld)
	{
		Super::GetCameraView(DeltaTime, DesiredView);
	}
	else
	{
		UpdateCameraModes();

		FCameraModeView CameraModeView;
		CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

		// Keep player controller in sync with the latest view.
		if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
		{
			if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
			{
				PC->SetControlRotation(CameraModeView.ControlRotation);
			}
		}

		// Apply any offset that was added to the field of view.
		CameraModeView.FieldOfView += FieldOfViewOffset;
		FieldOfViewOffset = 0.0f;

		// Keep camera component in sync with the latest view.
		SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);
		FieldOfView = CameraModeView.FieldOfView;

		// Fill in desired view.
		DesiredView.Location = CameraModeView.Location;
		DesiredView.Rotation = CameraModeView.Rotation;
		DesiredView.FOV = CameraModeView.FieldOfView;
		DesiredView.OrthoWidth = OrthoWidth;
		DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
		DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
		DesiredView.AspectRatio = AspectRatio;
		DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
		DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
		DesiredView.ProjectionMode = ProjectionMode;

		// See if the CameraActor wants to override the PostProcess settings used.
		DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
		if (PostProcessBlendWeight > 0.0f)
		{
			DesiredView.PostProcessSettings = PostProcessSettings;
		}


		if (IsXRHeadTrackedCamera())
		{
			// In XR much of the camera behavior above is irrellevant, but the post process settings are not.
			Super::GetCameraView(DeltaTime, DesiredView);
		}
	}
}

void UCommonCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	if (CameraModeStack->IsStackActivate())
	{
		if (DetermineCameraModeDelegate.IsBound())
		{
			if (const TSubclassOf<UCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
			{
				CameraModeStack->PushCameraMode(CameraMode);
			}
		}
	}
}
