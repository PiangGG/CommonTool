// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ImportExportSubsystem.generated.h"

/**
 * 
 */
class USaveSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnImportScene,const FString&, Path,bool ,bDeserialize,const FString&,SceneType);

UCLASS()
class COMMONTOOL_API UImportExportSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UImportExportSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void ImportScene();

	UFUNCTION(BlueprintCallable)
	void ImportScenePointCloudScene();

	UFUNCTION(BlueprintCallable)
	FString SelectedPath() const;
public:
	FOnImportScene OnImportScene;
};
