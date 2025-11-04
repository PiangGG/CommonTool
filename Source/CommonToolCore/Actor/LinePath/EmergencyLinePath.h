// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmergencyLinePath.generated.h"

struct FGameplayTag;
class USplineComponent;
class USplineMeshComponent;

UCLASS(Blueprintable,BlueprintType)
class COMMONTOOLCORE_API AEmergencyLinePath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEmergencyLinePath();
	
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag State);
public:

	UFUNCTION(BlueprintCallable)
	void PathSpawn_SplineMesh();

	UFUNCTION(BlueprintCallable)
	TArray<USplineMeshComponent*> GetSplineMesh();
	UFUNCTION(BlueprintCallable)
	TArray<UMaterialInstanceDynamic*> GetSplineMeshDynamicMaterial();

public:
	UPROPERTY(BlueprintReadOnly, Category = "LinePath")
	USceneComponent* Root;

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "LinePath")
	USplineComponent* SplineComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "LinePath")
	UStaticMesh* StaticMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LinePath")
	UMaterialInterface* Material;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "LinePath")
	float Wide = 1.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "LinePath")
	FVector StartLocationOffsize;
private:
	UPROPERTY()
	TArray<USplineMeshComponent*>SplineMeshComponents;
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> MaterialInstanceDynamics;

	UPROPERTY()
	FVector StartLocation;
	
};
