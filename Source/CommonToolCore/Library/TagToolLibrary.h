// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TagToolLibrary.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOLCORE_API UTagToolLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Tag")
	static void ConvertStringToGameplayTag(FString TagString,FGameplayTag& GameplayTag);

	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Tag")
	static void ConvertGameplayTaTogString(const FGameplayTag& GameplayTag,FString& TagString);
};
