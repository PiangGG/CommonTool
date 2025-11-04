// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AttributeToolLibrary.generated.h"

/**
 * 
 */
UCLASS()
class COMMONTOOLCORE_API UAttributeToolLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//获取属性值
	/*
	 * @param AttributeType 0为字符串 1为bool
	 */
	static bool GetStringAttribute(const TSharedPtr<FJsonObject>& JsonObject,const FString& AttributeName, FString &AttributeValue);

	static bool GetBoolAttribute(const TSharedPtr<FJsonObject>& JsonObject,const FString& AttributeName, bool &AttributeValue);

	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Attribute")
	static bool GetAttribute(const FString& SceneName,const FString& GUID,const FString& AttributeName,FString& Attribute);
	/*
	 * @param SceneName为空表示当前地图
	 */
	
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Attribute")
	static bool GetAttributes(const FString& SceneName,const FString& GUID,TArray<FString>& Attributes);

	// UFUNCTION(BlueprintCallable, Category="CommonToolCore|Attribute")
	// static bool GetAttribute(const FString& SceneName,const FString& GUID,FString Attributes);
	
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Attribute")
	static bool GetActorStatic(FString SceneName,const FString& GUID);
	/*
	 * 通过键值对创建一个关于电站的动态属性(目前只支持动态创建字符串类型)
	 * @param AttributeName 属性名称
	 * @param AttributeValue 属性数值
	 */
	UFUNCTION(BlueprintCallable, Category="CommonToolCore|Attribute")
	static bool MakeAttribute(const FString& AttributeName, const FString &AttributeValue,FString& ResultAttribute);
};
