// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructLibrary.h"
#include "Engine/DeveloperSettings.h"
#include "CommonSettings.generated.h"

//UCLASS(config = CommonToolCore,Category = CommonTool,DefaultConfig, meta = (DisplayName = "CommonToolDeveloperSettings"))
/**
 * 
 */
UCLASS(config=Game, Category = CommonTool, defaultconfig, meta = (DisplayName = "CommonToolSettings"))
class COMMONTOOLCORE_API UCommonSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Log")
	int32 LogLevel = 1;

	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|POI")
	TSubclassOf<UUserWidget> DefaultPOI;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|POI")
	TSubclassOf<UUserWidget> RegionPOI;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|POI")
	TSubclassOf<UUserWidget> DevicePOI;
	
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Device")
	TSoftClassPtr<AActor> DeviceInfoActor;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Device")
	bool bInitializeDeviceInfoPool = true;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Device")
	int32 InitializeDeviceInfoPoolSize = 1000;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Device")
	TSoftObjectPtr<UDataTable> DeviceDataTable;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Device")
	FTransform DeviceDisassembleTransform;
	
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Test")
	bool bPrintDebug = false;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Test")
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Test")
	float InteractionScanRate = 0.1;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Test")
	FTransform InitTransform;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Test")
	FInteractionSettings InteractionSettings;

	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Test")
	float Forcesize = 5.0f;
	
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Test")
	TSoftObjectPtr<UMaterialInterface> OverlayMaterial;

	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|Test")
	TSoftObjectPtr<UMaterialInterface> TransparentMaterial;
};
