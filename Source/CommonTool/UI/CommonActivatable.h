// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonActivatable.generated.h"

/**
 * 
 */
struct FUIInputConfig;

UENUM(BlueprintType)
enum class ECommonWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

UCLASS(Abstract, Blueprintable)
class COMMONTOOL_API UCommonActivatable : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UCommonActivatable(const FObjectInitializer& ObjectInitializer);

	//~UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~End of UCommonActivatableWidget interface

protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	ECommonWidgetInputMode InputConfig = ECommonWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
