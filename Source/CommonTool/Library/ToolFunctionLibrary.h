// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ToolFunctionLibrary.generated.h"
/**
 * 
 */

UCLASS()
class COMMONTOOL_API UToolFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	 *Debug
	 * @param Msg 输出日志
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Print")
	static void Debug(const FString& Msg);

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Print")
	static void Waring(const FString& Msg);

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Print")
	static void Error(const FString& Msg);
};
