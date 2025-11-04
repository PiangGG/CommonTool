// Fill out your copyright notice in the Description page of Project Settings.

/*
 * 配置文件操作相关的子系统
 */
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ConfigSubSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfigChanged,const FString&, ConfigString);
/**
 * 
 */
UCLASS()
class COMMONTOOLCORE_API UConfigSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UConfigSubSystem()
	{
	}

	static UConfigSubSystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 释放
	virtual void Deinitialize() override;

	void InitConfig();

	UFUNCTION(BlueprintCallable, Category=Cfg)
	FString GetCfgData();

	UFUNCTION(BlueprintCallable, Category=Cfg)
	void SetCfgData(const FString& Key,const FString& Value);

	UPROPERTY(BlueprintAssignable, Category=Cfg)
	FOnConfigChanged OnConfigChanged;
	// template<typename OutStructType>
	// bool ReadConfig(FString fileName, OutStructType* OutStruct, bool CheckEncrypt=true)
	// {
	// 	FString configFile = UKismetSystemLibrary::GetProjectDirectory() + fileName;
	//
	// 	if(FPaths::FileExists(configFile))
	// 	{
	// 		FString FileContent;
	// 		FFileHelper::LoadFileToString(FileContent, *configFile);
	// 		//解密
	// 		if(CheckEncrypt && GetIsNeedEncrypt())
	// 		{
	// 			//使用base64方式
	// 			FString outStr;
	// 			if(FBase64::Decode(FileContent, outStr))
	// 			{
	// 				FileContent = outStr;
	// 			}
	// 		}
	// 		
	// 		if(FJsonObjectConverter::JsonObjectStringToUStruct(FileContent,OutStruct))
	// 		{
	// 			UToolLibrary::Debug(FString::Printf(TEXT("Json解析成功:%s"), *FileContent));
	// 			return true;
	// 		}
	// 		else
	// 		{
	// 			UToolLibrary::Error(FString::Printf(TEXT("Json解析失败:%s"), *FileContent));
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UToolLibrary::Error(FString::Printf(TEXT("找不到配置文件:%s"), *configFile));
	// 	}
	// 	return false;
	// }
private:
	FString JsonStringCfg;
};
