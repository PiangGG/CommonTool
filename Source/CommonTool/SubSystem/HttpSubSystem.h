// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HttpSubSystem.generated.h"

/**
 * 
 */

// 动态多播代理 支持序列化，可以作为类变量（但需要加上 BlueprintAssignable 关键字），因此可以在蓝图中使用
UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHttpCallBack, const FString&, JsonStr);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHttpNetError, const FString&, Url);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FHttpSingleCallBack, const FString&, JsonStr);

UCLASS()
class COMMONTOOL_API UHttpSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UHttpSubSystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	UFUNCTION()
	void HttpPost(const FString& Url, const FString& Data, const FHttpSingleCallBack& SingleCallBack, int Retry = 5,
	              float TimeInterval = 5);

	//提供给蓝图调用的简单方式， 如果要处理复杂请求，eg 失败重试，根据返回参数做处理等， 使用其他方式
	//Post请求数据
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Http")
	void HttpPostCallBack(const FString& Url, const FString& Data, const FHttpSingleCallBack& SingleCallBack);
	//Get请求数据
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Http")
	void HttpGetCallBack(const FString& Url, const FString& Data, const FHttpSingleCallBack& SingleCallBack);

	//http请求回调完成
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Http")
	void HttpCallBackComplete(const FString& string);

	//调用deepSeek测试
	UFUNCTION(BlueprintCallable, Category="CommonLibrary|Http")
	void HttpPostDeepSeek(const FString& Url, const FString& Token, const FString& Data,
	                      const FHttpSingleCallBack& SingleCallBack);

	//游戏消息回调
	UFUNCTION(BlueprintNativeEvent, Category="CommonLibrary|Http")
	void OnGameHttpError(const FString& Url, const FString& Content);

	//网络错误
	UFUNCTION(BlueprintNativeEvent, Category="CommonLibrary|Http")
	void OnHttpNetWorkError(const FString& Url);

	UFUNCTION(BlueprintNativeEvent, Category="CommonLibrary|Http")
	void OnHttpError(const FString& Url, const FString& Content);

	//http 回调
	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Http")
	FHttpCallBack HttpCallBack;

	//http网络错误回调
	UPROPERTY(BlueprintAssignable, Category="CommonLibrary|Http")
	FHttpNetError HttpNetErrorHandle;
};
