// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDevice.h"
#include "DeviceSurface.generated.h"

class UProceduralMeshComponent;
class ADeviceEditPoint;
/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class COMMONTOOLCORE_API ADeviceSurface : public ABaseDevice
{
	GENERATED_BODY()
public:
	ADeviceSurface();

protected:
	
	virtual void BeginPlay() override;

	virtual void AddHitData_Implementation(FHitResult HitResult) override;
	
	UFUNCTION()
	void OnActorInitializationCompleted(UObject* Object);

	UFUNCTION()
	void AddPointLocation(FVector Location);

	virtual TSharedRef<FJsonObject> GetDeviceJsonObjectData(const FString &GUID) override;

	virtual void SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject) override;

	virtual void ReDraw() override;
	
	virtual void SetEditing(bool edit) override;

	UFUNCTION(BlueprintNativeEvent)
	void ReSetLocations(const TArray<FVector>& locations);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SM")
	UProceduralMeshComponent* ProceduralMeshComponent;
private:
	UPROPERTY()
	TArray<FVector> Locations;
	UPROPERTY()
	TArray<ADeviceEditPoint*>DeviceEditPoint;
};
