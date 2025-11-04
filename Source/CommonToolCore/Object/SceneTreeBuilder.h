// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneTreeBuilder.generated.h"

class USceneTreeNode;
/**
 * 
 */

UCLASS()
class COMMONTOOLCORE_API USceneTreeBuilder : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Tree")
	static USceneTreeNode* BuildSceneTree(const FString& JsonString);
	
};
