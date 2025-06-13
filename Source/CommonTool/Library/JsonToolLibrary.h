// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonToolLibrary.generated.h"

struct FGameplayTag;

/**
 * 
 */
UCLASS()
class COMMONTOOL_API UJsonToolLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	 * @param FileName 文件名称
	 * @param bPrintJsonStr 是否打印Json字符串
	 * return JsonString 返回Json字符串
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static bool GetJsonString(const FString& FileName, FString& JsonString, const bool& bPrintJsonStr = false,FString RelativePath = "");

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void SaveJsonToFile(const FString& FileName,const FString& JsonString,const FString& Path = "");
	/*
	 * @param FileName 文件名称
	 * return JsonString 返回JsonObject
	 */
	static TSharedPtr<FJsonObject> GetJsonObjectFromFile(const FString& FileName);

	/*
	 * @param FileName 文件名称
	 * return JsonString 返回JsonObject
	 */
	static TSharedPtr<FJsonObject> GetJsonObjectFromJsonString(const FString& JsonString);

	/*
	 * return JsonString 返回JsonObject
	 * return JsonString 返回JsonObject
	 */
	static void GetJsonStringFromJsonObject(const TSharedPtr<FJsonObject>& JsonObject, FString& JsonString);

	/*
	 * @param FileName 文件名称
	 * return Value 返回FString
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void GetStringFromJosn(const FString& FileName, const FString& Key, FString& Value);

	/*
	 * @param JsonString Json字符串
	 * return Value 返回FString
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void GetStringFromJsonString(const FString& JsonString, const FString& Key, FString& Value);

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void SetStringFromJsonString(const FString& JsonString, const FString& Key, const FString& Value,
	                                    FString& RsultJsonString);

	/*
	 * @param JsonString Json字符串
	 * return Value 返回int32
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void GetInteger32FromJsonString(const FString& JsonString, const FString& Key, int32& Value);

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void SetInteger32FromJsonString(const FString& JsonString, const FString& Key, const int32& Value,
	                                       FString& RsultJsonString);

	/*
	 * @param JsonString Json字符串
	 * return Value 返回int64
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void GetInteger64FromJsonString(const FString& JsonString, const FString& Key, int64& Value);

	/*
	 * @param JsonString Json字符串
	 * return Value 返回float
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void GetFloatFromJsonString(const FString& JsonString, const FString& Key, float& Value);

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void SetFloatFromJsonString(const FString& JsonString, const FString& Key, const float& Value,
	                                   FString& RsultJsonString);

	/*
	 * @param JsonString Json字符串
	 * return Value 返回bool
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void GetBoolFromJsonString(const FString& JsonString, const FString& Key, bool& Value);

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void SetBoolFromJsonString(const FString& JsonString, const FString& Key, const bool& Value,
	                                  FString& RsultJsonString);

	/*
	 * @param JsonString Json字符串
	 * return JsonStringArray 返回TArray<FString>
	 */
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void GetJsonStringArrayFromJsonString(const FString& JsonString, const FString& Key,
	                                             TArray<FString>& JsonStringArray);

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void GetJsonStringAllKeyFromJsonString(const FString& JsonString,
												 TArray<FString>& JsonStringKeys);
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void ConvertMapToJsonString(const TMap<FString, FString>& Map, FString& JsonString);
	
	static int32 GetArrayKeyIndex(TArray<TSharedPtr<FJsonValue>> JsonValueArray , const FString& Key);

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void ConvertStringToGameplayTag(FString TagString,FGameplayTag& GameplayTag);

	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Json")
	static void ConvertGameplayTaTogString(const FGameplayTag& GameplayTag,FString& TagString);
};
