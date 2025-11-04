// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "DeviceVideoDecal.generated.h"

class ADecalActor;
class UMediaPlayer;
class UMediaTexture;
class UMediaSoundComponent;
class UMaterialInterface;
/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class COMMONTOOLCORE_API ADeviceVideoDecal : public ABaseDevice
{
	GENERATED_BODY()
	
	ADeviceVideoDecal();
	
public:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditAnywhere, Category=Device)
	FString VideoPath;

	virtual AActor* GetActor_Implementation() override;

	virtual TSharedRef<FJsonObject> GetDeviceJsonObjectData(const FString &GUID) override;
	
	virtual void SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject) override;

	virtual void SetDeviceLoad_Implementation(const FString& bload) override;
	
	UFUNCTION()
	void OnActorInitializationCompleted(UObject* Object);

	virtual void ReDraw() override;
	
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|MediaPlayer")
	UMediaPlayer* GetMediaPlayer()const;
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|MediaPlayer")
	UMediaTexture* GetMediaTexture()const;
	//实例中创建MediaPlayer
	UMediaPlayer* CreateAndSetupMediaPlayer();

	void PlayLocalVideo(const FString& FilePath);

	void PlayStream(const FString& StreamUrl);

	void SetupMediaOutputs(UMediaTexture*& OutMediaTexture, UMediaSoundComponent*& OutMediaSound);

	FTransform GetDecalTransform()const;
private:
	
	UPROPERTY()
	UMediaPlayer* MediaPlayer;

	UPROPERTY()
	UMediaTexture* MediaTexture;
    
	UPROPERTY()
	UMediaSoundComponent* MediaSound;

	UPROPERTY()
	FString Option = "LocalVideo";

	UPROPERTY()
	ADecalActor* DecalActor;

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstanceDynamic;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CommonToolCore|MediaPlayer")
	UMaterialInterface* MaterialInterface;
};
