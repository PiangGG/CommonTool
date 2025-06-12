// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionObjectInterface.h"

#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

// Add default functionality here for any IFunctionObjectInterface functions that are not pure virtual.
void IFunctionObjectInterface::SetFunctionName(FText Name)
{
	// UTitleWidget* TempTitleWidget = Cast<UTitleWidget>(TitleWidget);
	// if (TempTitleWidget) {
	// 	TempTitleWidget->SetItem(WorldResourceItem);
	// 	TempTitleWidget->SetTitleNameText(Name);
	// }
}

int32 IFunctionObjectInterface::GetFontSize() const
{
	return PropertyInfo.FontSize;
}

void IFunctionObjectInterface::SetFontSize(int32 Size)
{
	if (TitleWidget)
	{
		if (UTextBlock* TitleName = Cast<UTextBlock>(TitleWidget->GetWidgetFromName(TEXT("TheTitleName"))))
		{
			FSlateFontInfo Font = TitleName->GetFont();
			Font.Size = Size;
			TitleName->SetFont(Font);
			PropertyInfo.FontSize = Size;
		}
	}
}

void IFunctionObjectInterface::SetBackgroundColor(FLinearColor& Color)
{
	if (TitleWidget)
	{
		if (UBorder* BackgroundColor = Cast<UBorder>(TitleWidget->GetWidgetFromName(TEXT("BackgroundColor"))))
		{
			BackgroundColor->SetBrushColor(Color);
			PropertyInfo.BackgroundColor = Color;
		}
	}
}

void IFunctionObjectInterface::SetIconSize_XY(int32 X, int32 Y)
{
	if (TitleWidget)
	{
		if (UImage* DisplayTag = Cast<UImage>(TitleWidget->GetWidgetFromName(TEXT("DisplayTag"))))
		{
			FVector2D Size(X, Y);
			DisplayTag->SetDesiredSizeOverride(Size);
			PropertyInfo.IconSize_X = X;
			PropertyInfo.IconSize_Y = Y;
		}
	}
}

void IFunctionObjectInterface::SetIconStyle(UTexture2D* Icon)
{
	if (TitleWidget)
	{
		if (UImage* DisplayTag = Cast<UImage>(TitleWidget->GetWidgetFromName(TEXT("DisplayTag"))))
		{
			DisplayTag->SetBrushFromTexture(Icon);
			PropertyInfo.IconStyle = Icon;
		}
	}
}

void IFunctionObjectInterface::SetIconName(FString Name)
{
	PropertyInfo.IconName = Name;
}

void IFunctionObjectInterface::SetShowIconStatus(bool TheStatus)
{
	if (TheStatus)
	{
		TitleWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		TitleWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

FLinearColor IFunctionObjectInterface::GetBackgroundColor() const
{
	return PropertyInfo.BackgroundColor;
}

FVector2D IFunctionObjectInterface::GetIconSize() const
{
	return FVector2D(PropertyInfo.IconSize_X, PropertyInfo.IconSize_Y);
}

UTexture2D* IFunctionObjectInterface::GetIconStyle() const
{
	return PropertyInfo.IconStyle;
}

float IFunctionObjectInterface::GetExtraInfo() const
{
	return 0.0f;
}
