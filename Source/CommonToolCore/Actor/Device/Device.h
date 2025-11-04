// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructLibrary.h"
#include "GameFramework/Actor.h"
#include "Device.generated.h"

class UDeviceManager;
class UStateSubsystem;

UCLASS()
class COMMONTOOLCORE_API ADevice : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADevice();

	UFUNCTION(BlueprintCallable)
	void SetDeviceNodes(const FDeviceConstructStruct &DeviceConstructStruct);
	UFUNCTION(BlueprintCallable)
	void FindDeviceComponentData(UStaticMeshComponent* MeshComponent,FDeviceTreeNodeStruct & DeviceComponentData);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDeviceStateOnChange(FGameplayTag State,const FString& ID);

	//Disassemble
	UFUNCTION()
	void DisassembleDevice();
	UFUNCTION()
	void UpdateDisassembleDevice(UStaticMeshComponent* MeshComponent, const FDeviceTreeNodeStruct &DeviceTreeNodeStruct);
	UFUNCTION()
	void OnDeviceDisassemble(ADevice* Device,const FString& Type);
	UFUNCTION()
	void StartQuery();
	UFUNCTION()
	void Query();
	UFUNCTION()
	void EndQuery();
	UFUNCTION(BlueprintCallable)
	FHitResult LineTraceProfileActor(const FName& name);
private:
	friend class UStateSubsystem;
	
	friend class UDeviceManager;
	
	friend class UPOICanvas;
	
	UPROPERTY()
	FDeviceConstructStruct DeviceStruct;
	
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	UPROPERTY()
	UDeviceManager* DeviceManager;
	UPROPERTY()
	UDataTable* DeviceDataTable;
	UPROPERTY()
	TMap<UStaticMeshComponent*,FDeviceTreeNodeStruct> DeviceComponentMap;
	// UPROPERTY()
	// float DisassembleBlendAlpha;
	// UPROPERTY()
	// FTimerHandle DisassembleDeviceTimeHandle;

	UPROPERTY()
	TArray<FString> DisassembleTypes;
	
	UPROPERTY()
	FTimerHandle QueryTimerHandle;
	UPROPERTY()
	UMeshComponent*QueryComponent;
	UPROPERTY()
	UMaterialInterface* OverlayMaterial;
	UPROPERTY()
	TMap<UStaticMeshComponent*,FTimerHandle> TimerHandleMap;
	UPROPERTY()
	TMap<UStaticMeshComponent*,int32> BlendAlphaMap;
};
