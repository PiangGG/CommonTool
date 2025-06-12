// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "Components/SplineComponent.h"
#include "DeviceLine.generated.h"

class USplineMeshComponent;
class ADeviceEditPoint;
/**
 * 
 */
UCLASS()
class COMMONTOOL_API ADeviceLine : public ABaseDevice
{
	GENERATED_BODY()

public:
	ADeviceLine();

protected:
	
	virtual void BeginPlay() override;

	virtual void AddHitData_Implementation(FHitResult HitResult) override;
	
	UFUNCTION()
	void OnActorInitializationCompleted(UObject* Object);

	UFUNCTION()
	void AddPointLocation(FVector Location);

	virtual TSharedRef<FJsonObject> GetDeviceJsonObjectData(const FString &GUID) override;

	virtual void SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject) override;

	virtual void SetEditing(bool edit) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void ReSetLocations(const TArray<FVector>& locations);

	virtual void ReDraw() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	UPROPERTY(BlueprintReadOnly)
	USplineComponent* SplineComponent;
private:
	UPROPERTY()
	TArray<FVector> Locations;
	UPROPERTY()
	TArray<ADeviceEditPoint*>DeviceEditPoint;
	
};