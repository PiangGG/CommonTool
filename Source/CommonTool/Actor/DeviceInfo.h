// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeviceInfo.generated.h"

class UWidgetComponent;
class UUIManagerSubsystem;

UCLASS()
class COMMONTOOL_API ADeviceInfo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeviceInfo();

	UFUNCTION()
	void SetDeviceKey(const FString& id,AActor *Device);

	UFUNCTION(BlueprintNativeEvent)
	void OnSetDeviceKey(const FString& id);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Info", meta=(AllowPrivateAccess=true))
	USceneComponent* Root;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Info", meta=(AllowPrivateAccess=true))
	// UWidgetComponent* InfoWidget;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Info", meta=(AllowPrivateAccess=true))
	// TSubclassOf<UUserWidget> InfoWidgetClass;

	UPROPERTY()
	UUIManagerSubsystem* UIManagerSubsystem;
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Info", meta=(AllowPrivateAccess=true))
	FString ID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Info", meta=(AllowPrivateAccess=true))
	UUserWidget* UserWidget;
};
