// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonFunctionLibrary.h"

#include "GameplayTagContainer.h"
#include "ToolFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

bool UJsonFunctionLibrary::GetJsonString(const FString& FileName, FString& JsonString, const bool& bPrintJsonStr,FString RelativePath)
{
	FString JsonFilePath;
	if (RelativePath.IsEmpty())
	{
		JsonFilePath = UKismetSystemLibrary::GetProjectContentDirectory() + FileName + ".json";
	}
	else
	{
		JsonFilePath = UKismetSystemLibrary::GetProjectContentDirectory() + RelativePath+"/"+FileName + ".json";
	}

	if (FPaths::FileExists(JsonFilePath))
	{
		FString FileContent;
		if (FFileHelper::LoadFileToString(FileContent, *JsonFilePath))
		{
			JsonString = FileContent;
			if (bPrintJsonStr)
			{
				UToolFunctionLibrary::Debug(FString::Printf(TEXT("Json解析成功:%s"), *FileName));
			}
			return true;
		}
		else
		{
			UToolFunctionLibrary::Error(FString::Printf(TEXT("Json解析失败:%s"), *FileName));
		}
	}
	else
	{
		UToolFunctionLibrary::Error(FString::Printf(TEXT("找不到配置文件:%s"), *FileName));
	}
	return false;
}

void UJsonFunctionLibrary::SaveJsonToFile(const FString& FileName,const FString& JsonString,const FString& Path)
{
	// 将FString转换为TCHAR*，因为FFileHelper::SaveStringToFile需要TCHAR*
	const TCHAR* TCharJsonString = *JsonString;
	FString FilePath = UKismetSystemLibrary::GetProjectContentDirectory()+Path+"/"+ FileName + ".json";
	// 使用FFileHelper保存字符串到文件
	bool bSuccess = FFileHelper::SaveStringToFile(
		TCharJsonString, // 要保存的字符串
		*FilePath,       // 文件路径
		FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM, // 编码选项，这里选择UTF8无BOM
		&IFileManager::Get()                                // 文件管理器实例
	);
 
	if (bSuccess)
	{
		UToolFunctionLibrary::Debug(FString::Printf(TEXT("JSON文件已成功保存: %s"), *FileName));
	}
	else
	{
		UToolFunctionLibrary::Error(FString::Printf(TEXT("保存JSON文件失败: %s"), *FileName));
	}
}

TSharedPtr<FJsonObject> UJsonFunctionLibrary::GetJsonObjectFromFile(const FString& FileName)
{
	FString JsonString;
	GetJsonString(FileName, JsonString, false);

	// 创建 Json 阅读器
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	// 创建 Json 对象
	TSharedPtr<FJsonObject> JsonObject;
	// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
	FJsonSerializer::Deserialize(JsonReader, JsonObject);

	return JsonObject;
}

TSharedPtr<FJsonObject> UJsonFunctionLibrary::GetJsonObjectFromJsonString(const FString& JsonString)
{
	// 创建 Json 阅读器
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	// 创建 Json 对象
	TSharedPtr<FJsonObject> JsonObject;
	// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
	FJsonSerializer::Deserialize(JsonReader, JsonObject);

	return JsonObject;
}

void UJsonFunctionLibrary::GetJsonStringFromJsonObject(const TSharedPtr<FJsonObject>& JsonObject, FString& JsonString)
{
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
}

void UJsonFunctionLibrary::GetStringFromJosn(const FString& FileName, const FString& Key, FString& Value)
{
	FString Result;
	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromFile(FileName))
	{
		Result = JsonObject->GetStringField(Key);
	}
	Value = Result;
}

void UJsonFunctionLibrary::GetStringFromJsonString(const FString& JsonString, const FString& Key, FString& Value)
{
	FString Result;

	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		Result = JsonObject->GetStringField(Key);
	}
	Value = Result;
}

void UJsonFunctionLibrary::SetStringFromJsonString(const FString& JsonString, const FString& Key, const FString& Value,
                                                   FString& RsultJsonString)
{
	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		JsonObject->SetStringField(Key, Value);
		GetJsonStringFromJsonObject(JsonObject, RsultJsonString);
	}
}

void UJsonFunctionLibrary::GetInteger32FromJsonString(const FString& JsonString, const FString& Key, int32& Value)
{
	int32 Result = -1;

	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		Result = JsonObject->GetIntegerField(Key);
	}
	Value = Result;
}

void UJsonFunctionLibrary::SetInteger32FromJsonString(const FString& JsonString, const FString& Key, const int32& Value,
                                                      FString& RsultJsonString)
{
	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		JsonObject->SetNumberField(Key, Value);
		GetJsonStringFromJsonObject(JsonObject, RsultJsonString);
	}
}

void UJsonFunctionLibrary::GetInteger64FromJsonString(const FString& JsonString, const FString& Key, int64& Value)
{
	int64 Result = -1;

	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		if (JsonObject->TryGetNumberField(Key, Result))
		{
		}
		else
		{
		}
	}
	Value = Result;
}

void UJsonFunctionLibrary::GetFloatFromJsonString(const FString& JsonString, const FString& Key, float& Value)
{
	int32 Result = -1;

	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		Result = JsonObject->GetIntegerField(Key);
	}
	Value = Result;
}

void UJsonFunctionLibrary::SetFloatFromJsonString(const FString& JsonString, const FString& Key, const float& Value,
                                                  FString& RsultJsonString)
{
	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		JsonObject->SetNumberField(Key, Value);
		GetJsonStringFromJsonObject(JsonObject, RsultJsonString);
	}
}

void UJsonFunctionLibrary::GetBoolFromJsonString(const FString& JsonString, const FString& Key, bool& Value)
{
	bool Result = false;

	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		Result = JsonObject->GetBoolField(Key);
	}
	Value = Result;
}

void UJsonFunctionLibrary::SetBoolFromJsonString(const FString& JsonString, const FString& Key, const bool& Value,
                                                 FString& RsultJsonString)
{
	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		JsonObject->SetBoolField(Key, Value);
		GetJsonStringFromJsonObject(JsonObject, RsultJsonString);
	}
}

void UJsonFunctionLibrary::GetJsonStringArrayFromJsonString(const FString& JsonString, const FString& Key,
                                                            TArray<FString>& JsonStringArray)
{
	TArray<FString> Result;

	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray = JsonObject->GetArrayField(Key);
	
		for (auto JsonValue : JsonArray)
		{
			FString LocalJsonString = "";
			TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&LocalJsonString);
			FJsonSerializer::Serialize(JsonValue->AsObject().ToSharedRef(), JsonWriter);
			Result.Add(LocalJsonString);
		}
	}
	JsonStringArray = Result;
}

void UJsonFunctionLibrary::GetJsonStringAllKeyFromJsonString(const FString& JsonString, TArray<FString>& JsonStringKeys)
{
	if (TSharedPtr<FJsonObject> JsonObject = GetJsonObjectFromJsonString(JsonString))
	{
		JsonObject->Values.GetKeys(JsonStringKeys);
	}
}

void UJsonFunctionLibrary::ConvertMapToJsonString(const TMap<FString, FString>& Map, FString& JsonString)
{
	// 创建一个Json对象
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	// 遍历Map，将键值对添加到Json对象中
	for (const auto& Pair : Map)
	{
		JsonObject->SetStringField(Pair.Key, Pair.Value);
	}

	// 将Json对象转换为字符串
	FString OutputString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	JsonString = OutputString;
}

int32 UJsonFunctionLibrary::GetArrayKeyIndex(TArray<TSharedPtr<FJsonValue>> JsonValueArray, const FString& Key)
{
	FString LocalKey = "";
	for (int i = 0;i<JsonValueArray.Num();i++)
	{
		JsonValueArray[i]->AsObject()->TryGetStringField(TEXT("Key"),LocalKey);
		if (LocalKey.Equals(Key))
		{
			return i;
		}
	}
	return -1;
}

void UJsonFunctionLibrary::ConvertStringToGameplayTag(FString TagString,FGameplayTag& GameplayTag)
{
	GameplayTag = FGameplayTag::RequestGameplayTag(FName(*TagString), false);
}

void UJsonFunctionLibrary::ConvertGameplayTaTogString(const FGameplayTag& GameplayTag, FString& TagString)
{
	TagString = GameplayTag.ToString();
}

