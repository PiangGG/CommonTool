// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTool/RuntimeGizemo/CommonGizmoHandle.h"
#include "Engine/DeveloperSettings.h"
#include "CommDeveloperSettings.generated.h"

//UCLASS(config = CommonToolLibrary,Category = CommonTool,DefaultConfig, meta = (DisplayName = "CommonToolDeveloperSettings"))
/**
 * 
 */
UCLASS(config=Game, Category = CommonTool, defaultconfig, meta = (DisplayName = "CommonToolSettings"))
class COMMONTOOL_API UCommDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Log")
	int32 LogLevel = 1;

	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|UI")
	TSubclassOf<UUserWidget> ActorInfoWidget;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|UI")
	TSubclassOf<UUserWidget> TrainStationMarkingWidget;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|UI")
	TSubclassOf<UUserWidget> DeviceInfoSelectWidget;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Device")
	TSoftClassPtr<AActor> DeviceInfoActor;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Device")
	bool bInitializeDeviceInfoPool = true;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Device")
	int32 InitializeDeviceInfoPoolSize = 1000;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Device")
	FTransform DeviceDisassembleTransform;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Test")
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Test")
	float InteractionScanRate = 0.1;

	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Test")
	FInteractionSettings InteractionSettings;

	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Test")
	float Forcesize = 5.0f;
	
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Test")
	TSoftObjectPtr<UMaterialInterface> OverlayMaterial;

	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonLibrary|Test")
	TSoftObjectPtr<UMaterialInterface> TransparentMaterial;
};
