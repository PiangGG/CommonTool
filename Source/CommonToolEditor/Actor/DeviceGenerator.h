// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeviceGenerator.generated.h"

class UInspectionManager;

UCLASS()
class COMMONTOOLEDITOR_API ADeviceGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeviceGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable)
	bool GetGenerateDeviceLocation(FVector& Vector);
	UFUNCTION(BlueprintCallable,CallInEditor,Category="DeviceGenerator")
	void GenerateDevice();
	
	void SpawnActorToExistingLevel(const FString& LevelName, const FVector& SpawnLocation);

	void CreateNewLevelAndSpawnActor(const FString& LevelName, const FVector& SpawnLocation);

	void SpawnActors(const FString& LevelName, const FVector& SpawnLocation,UWorld* SpawnWorld);
	FVector TransformLocalOffsetToWorld(
	const FVector& WorldBaseLocation, // 基准世界位置
	const FVector& WorldDirection,    // 基准世界方向（需归一化）
	const FVector& LocalOffset        // 本地空间中的偏移
	);

	UFUNCTION(BlueprintCallable,CallInEditor,Category="DeviceGenerator")
	void SetDeviceData();
#endif

public:
	UPROPERTY()
	UInspectionManager* InspectionManager;
	UPROPERTY(EditAnywhere)
	FString SaveLevelName;
	UPROPERTY(EditAnywhere)
	FString SavePath;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> DeviceActorClass;
	//起始点偏移位置
	UPROPERTY(EditAnywhere, meta=(MakeEditWidget))
	FVector RoundOffSizeLocation;
	UPROPERTY(EditAnywhere)
	FVector PitchOffSizeLocation;
	UPROPERTY(EditAnywhere,meta=(ClampMin=1,ClampMax=25))
	int32 Paragraphs = 1;
	UPROPERTY(EditAnywhere)
	float Spacing = 6000;

private:
	UPROPERTY()
	TArray<AActor*> Actors;
};
