// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "POICanvas.generated.h"

class ADevice;
class UDocumentManager;
class USceneManagerSubsystem;
class UStateSubsystem;
class UCanvasPanel;
struct FGameplayTag;
class UDeviceManager;
/**
 * 
 */
UCLASS()
class COMMONTOOLCORE_API UPOICanvas : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION()
	void OnDevicePOIInit(const FString&ID,const FVector&HitLocation,const bool bShow);
	UFUNCTION()
	void OnDeviceDisassemble(ADevice* Device,const FString& Type);
	UFUNCTION()
	void OnDeviceStateOnChange(FGameplayTag newState,const FString& ID);

	UFUNCTION()
	void OnScenePOIInit(const FString&ID,const FVector&HitLocation,const bool bShow,UUserWidget* UserWidget);

	UFUNCTION()
	void OnDocumentPOIInit(const FString&ID,const FVector&HitLocation,const bool bShow,UUserWidget* UserWidget);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (BindWidget))
	UCanvasPanel* CanvasPanel;
private:
	UPROPERTY()
	UDeviceManager* DeviceManager;
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	UPROPERTY()
	USceneManagerSubsystem* SceneManagerSubsystem;
	UPROPERTY()
	UDocumentManager* DocumentManager;
	UPROPERTY()
	TMap<FString,UUserWidget*> POIMap;
	UPROPERTY()
	TMap<FString,FVector> POILocationMap;
	UPROPERTY()
	TMap<UStaticMeshComponent*,UUserWidget*> DeviceComponentTagMap;
	UPROPERTY()
	ADevice* DisassembleDevice;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> DisassembleDevicePOIClass;
};
