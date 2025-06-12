// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "WorldAssetTree.generated.h"

class UUIManagerSubsystem;
class UTreeView;
/**
 * 
 */
UCLASS()
class COMMONTOOL_API UWorldAssetTree : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true",BindWidget))
	UTreeView* TreeView;

	UPROPERTY()
	UUIManagerSubsystem* UIManagerSubsystem ;
};
