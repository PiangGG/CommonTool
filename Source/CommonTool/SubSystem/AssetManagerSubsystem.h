// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AssetManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOL_API UAssetManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UAssetManagerSubsystem()
	{
	}

	static UAssetManagerSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;
public:


private:
};
