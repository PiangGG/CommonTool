// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "WheelMenu.generated.h"

class SRadialWheelMenu;
/**
 * 
 */
UCLASS()
class COMMONTOOLCORE_API UWheelMenu : public UWidget
{
	GENERATED_BODY()

public:
	// SRadialWheelMenu使用
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// 同步属性到SRadialWheelMenu
	virtual void SynchronizeProperties() override;
	// 当编译器编译时或需要销毁时会调用这个方法来销毁Slate的资源
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RadialWheelMenu")
	int32 Count;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RadialWheelMenu")
	int32 Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RadialWheelMenu")
	float WidthOverride;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RadialWheelMenu")
	float HeightOverride;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RadialWheelMenu")
	TObjectPtr<UMaterialInterface> BackgroundMaterial;
	// 用于创建动态的Material，并且我们在之前写Material时留下了参数
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynMaterial;

	FReply OnMouseMove(const FGeometry& Geometry,const FPointerEvent& PointerEvent);

	FReply OnMouseButtonDown(const FGeometry& Geometry,const FPointerEvent& PointerEvent);
	
	// 声明InnerRadius内圆大小也就是空心的大小如若没有填0即可
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RadialWheelMenu")
	int32 InnerRadius;

	static int32 GetCurrentSectorIndex(int32 SectorCount, FVector2D MousePosition, float OuterRadius, float InnerRadius);

	void AddItem(UUserWidget* InWidget);
private:
	// 用于存储创建Slate的对象
	TSharedPtr<SRadialWheelMenu> MyRadialWheelMenu;
	// 存储Image的画笔对象
	FSlateBrush	SlateBrush;

	UPROPERTY()
	TArray<UUserWidget*> ListItems;
};
