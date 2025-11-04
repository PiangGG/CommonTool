// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCanvas.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class COMMONTOOLCORE_API SRadialWheelMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRadialWheelMenu)
	{}
		SLATE_ATTRIBUTE(FOptionalSize, WidthOverride)
      	SLATE_ATTRIBUTE(FOptionalSize, HeightOverride)
      	SLATE_ATTRIBUTE( const FSlateBrush*, BackgroundImage )
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	// 当参数修改时能够刷新
	TAttribute<FOptionalSize> WidthOverrideAttr;
	TAttribute<FOptionalSize> HeightOverrideAttr;
	TAttribute<const FSlateBrush*> BackgroundImageAttr;
	TSharedPtr<SCanvas> MyCanvas;
	TArray<SCanvas::FSlot*> CanvasSlots;
public:
	void SetWidthOverrideAttr(float WidthOverride)
	{
		WidthOverrideAttr = WidthOverride;
	}

	void SetHeightOverrideAttr(float HeightOverride)
	{
		HeightOverrideAttr = HeightOverride;
	}

	void SetBackgroundImageAttr(const FSlateBrush* SlateBrush)
	{
		BackgroundImageAttr = SlateBrush;
	}

	void InitializeCircularSlots(int32 NumSectors, float Radius, const FVector2D& Center);

	void InsertWidget(const TSharedRef<SWidget>& InWidget, int32 Index);

	void RequestLayoutRefresh();
};
