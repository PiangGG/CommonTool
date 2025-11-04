// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeviceInfo.generated.h"

class UWidgetComponent;
class UDeviceManager;

UCLASS()
class COMMONTOOLCORE_API ADeviceInfo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeviceInfo();

	UFUNCTION()
	void Pop(const FString& id);
	UFUNCTION()
	void Push(const FString& id);
	UFUNCTION(BlueprintNativeEvent)
	void OnSetDeviceID(const FString& id);

	UFUNCTION(BlueprintPure,Category="CommonToolCore|DeviceManager")
	UUserWidget* GetDevicePOI();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonToolCore|Info", meta=(AllowPrivateAccess=true))
	USceneComponent* Root;
private:
	UPROPERTY()
	UDeviceManager* DeviceManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonToolCore|Info", meta=(AllowPrivateAccess=true))
	UUserWidget* UserWidget;
};
