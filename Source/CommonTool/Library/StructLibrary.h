// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructLibrary.generated.h"
/**
 * 
 */
class COMMONTOOL_API StructLibrary
{
public:
	StructLibrary();
	~StructLibrary();
};

USTRUCT(BlueprintType)
struct FIconItem:public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag Tag;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FEditItem:public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString Name;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSoftClassPtr<AActor>Actor;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag Tag;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString ItemIcon;
};

USTRUCT(BlueprintType)
struct FVisitPath {
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag VisitStatus;

	UPROPERTY()
	FString Path;
};

USTRUCT(BlueprintType)
struct FPropertyInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FontSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor BackgroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IconSize_X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IconSize_Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString IconName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString IconResourcePath;
};