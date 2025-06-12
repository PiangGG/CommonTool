// Fill out your copyright notice in the Description page of Project Settings.


#include "SRadialWheelMenu.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SRadialWheelMenu::Construct(const FArguments& InArgs)
{
	WidthOverrideAttr = InArgs._WidthOverride;
	HeightOverrideAttr = InArgs._HeightOverride;
	BackgroundImageAttr = InArgs._BackgroundImage;
	MyCanvas = SNew(SCanvas);
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(WidthOverrideAttr)// 绑定动态属性改变时保证会重新渲染 
		.HeightOverride(HeightOverrideAttr)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.Image(BackgroundImageAttr)
				.ColorAndOpacity(FSlateColor(FColor::White))
			]
			// 在这里添加插槽，添加我们的Canvas
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				MyCanvas.ToSharedRef()
			]
		]
	];
}

void SRadialWheelMenu::InitializeCircularSlots(int32 NumSectors, float Radius, const FVector2D& Center)
{
	MyCanvas->ClearChildren();
	CanvasSlots.Reset();

	constexpr float HeightRatio = 0.3f;
	const float AngleStep = 360.0f / NumSectors;
	const float SectorAngle = FMath::DegreesToRadians(AngleStep);
	const float MaxWidth = 2 * (Radius * (1.f - HeightRatio)) * FMath::Sin(SectorAngle / 2); // 根据弦长公式计算最大宽度
	const FVector2D WidgetSize(
		FMath::Min(MaxWidth - 5.0f, 100.0f), // 留5px边距，设置最大100px防止过大
		Radius * HeightRatio // 高度设为半径的30%
	);

	for (int32 i = 0; i < NumSectors; ++i)
	{
		// 计算角度（转换为弧度）
		const float Angle = FMath::DegreesToRadians(AngleStep * (i + 0.5) - 90.f);

		// 计算坐标位置（中心点对齐）
		const FVector2D Position(
			Center.X + Radius * FMath::Cos(Angle) - WidgetSize.X / 2,
			Center.Y + Radius * FMath::Sin(Angle) - WidgetSize.Y / 2
		);

		// 创建占位插槽
		auto NewSlot = MyCanvas->AddSlot()
								.Position(Position)
								.Size(WidgetSize)
								.VAlign(VAlign_Fill)
								.HAlign(HAlign_Fill)
		[
			SNew(SBorder)
			.VAlign(VAlign_Center) 
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::FromInt(i)))
				.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 24))
			]
		].GetSlot();
		
		CanvasSlots.Add(NewSlot);
	}
}

void SRadialWheelMenu::InsertWidget(const TSharedRef<SWidget>& InWidget, int32 Index)
{
	if (CanvasSlots.IsValidIndex(Index) && CanvasSlots[Index] != nullptr)
	{
		CanvasSlots[Index]->AttachWidget(InWidget);
	}
}

void SRadialWheelMenu::RequestLayoutRefresh()
{
	
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
