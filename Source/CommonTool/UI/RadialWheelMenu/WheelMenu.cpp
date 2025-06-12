// Fill out your copyright notice in the Description page of Project Settings.


#include "WheelMenu.h"

#include "SRadialWheelMenu.h"
#include "Blueprint/UserWidget.h"

TSharedRef<SWidget> UWheelMenu::RebuildWidget()
{
	Super::RebuildWidget();
	// 初始化我们的材质并且赋值默认值
	if (BackgroundMaterial)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(BackgroundMaterial, this);
		DynMaterial->SetScalarParameterValue("Count", Count);
		DynMaterial->SetScalarParameterValue("Index", Index);
		SlateBrush.SetResourceObject(DynMaterial);
	}
	MyRadialWheelMenu = SNew(SRadialWheelMenu)
		.HeightOverride(HeightOverride)
		.WidthOverride(WidthOverride);
	MyRadialWheelMenu->SetBackgroundImageAttr(&SlateBrush);
	MyRadialWheelMenu->SetOnMouseMove(FPointerEventHandler::CreateUObject(this, &UWheelMenu::OnMouseMove));
	MyRadialWheelMenu->SetOnMouseButtonDown(FPointerEventHandler::CreateUObject(this, &UWheelMenu::OnMouseButtonDown));
	return MyRadialWheelMenu.ToSharedRef();
}

void UWheelMenu::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyRadialWheelMenu.IsValid())
	{
		MyRadialWheelMenu->SetHeightOverrideAttr(HeightOverride);
		MyRadialWheelMenu->SetWidthOverrideAttr(WidthOverride);
		if (ensure(DynMaterial))
		{
			DynMaterial->SetScalarParameterValue("Count", Count);
			DynMaterial->SetScalarParameterValue("Index", Index);
		}
	}
}

void UWheelMenu::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyRadialWheelMenu.Reset();
}

FReply UWheelMenu::OnMouseMove(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	// 直接捕获鼠标在屏幕上的位置的话返回的是在整个显示视口的位置
	// 所以需要转换成当前控件的位置也就SizeBob的位置
	FDeprecateSlateVector2D MousePostion = Geometry.AbsoluteToLocal(PointerEvent.GetScreenSpacePosition());
	// 创建一个静态方法来获取Index
	// 需要参数OuterRadius，InnerRadius来计算是否在圆内
	Index = GetCurrentSectorIndex(Count, MousePostion, WidthOverride / 2, InnerRadius);
	if (ensure(DynMaterial))
	{
		DynMaterial->SetScalarParameterValue("Index", Index);
	}
	return FReply::Handled();
}

FReply UWheelMenu::OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	FDeprecateSlateVector2D MousePostion = Geometry.AbsoluteToLocal(PointerEvent.GetScreenSpacePosition());
	Index = GetCurrentSectorIndex(Count, MousePostion, WidthOverride / 2, InnerRadius);
	return FReply::Handled();
}

int32 UWheelMenu::GetCurrentSectorIndex(int32 SectorCount, FVector2D MousePosition, float OuterRadius,
                                        float InnerRadius)
{
	// 1.转移（0，0）坐标到圆心坐标
	FVector2D RelativePos = MousePosition - FVector2D(OuterRadius, OuterRadius);
	
	float DistanceSquared = RelativePos.SizeSquared();
	// 2. 判断是否在圆范围内
	if (DistanceSquared > OuterRadius * OuterRadius || DistanceSquared < InnerRadius * InnerRadius)
	{
		return -1;
	}

	// 3. 转换为极坐标角度（弧度）
	float Radians = FMath::Atan2(RelativePos.Y, RelativePos.X);
	// 4.把弧度转换成角度
	float Degrees = FMath::Fmod(FMath::RadiansToDegrees(Radians) + 90.0f, 360.0f);
	if (Degrees < 0.0f)
	{
		Degrees += 360.0f; // 确保角度为正
	}

	// 4. 计算扇形索引
	float SectorAngle = 360.0f / SectorCount;
	int32 Index = FMath::Floor(Degrees / SectorAngle);
                
	UE_LOG(LogTemp, Warning, TEXT("Degrees: %.2f, Sector Index: %d"), Degrees, Index);
	return Index;
}

void UWheelMenu::AddItem(UUserWidget* InWidget)
{
	if (InWidget == nullptr)
	{
		return;
	}
	if (ListItems.Contains(InWidget))
	{
		return;
	}
	
	int32 TempIndex = ListItems.Add(InWidget);
	if (MyRadialWheelMenu.IsValid())
	{
		MyRadialWheelMenu->InsertWidget(InWidget->TakeWidget(), TempIndex);
		MyRadialWheelMenu->RequestLayoutRefresh();
	}
}
