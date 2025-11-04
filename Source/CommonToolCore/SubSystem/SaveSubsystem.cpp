// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSubsystem.h"

#include "SceneManagerSubsystem.h"
#include "CommonToolCore/Library/JsonToolLibrary.h"
#include "CommonToolCore/Library/PrintToolLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

USaveSubsystem* USaveSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		USaveSubsystem* Router = UGameInstance::GetSubsystem<USaveSubsystem>(GWorld->GetGameInstance());
		check(Router);
		return Router;
	}
	return nullptr;
}

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("SaveSubsystem::Initialize"));
}

void USaveSubsystem::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("SaveSubsystem::Deinitialize"));
	Super::Deinitialize();
}

void USaveSubsystem::SaveCurrentScene()
{
	UPrintToolLibrary::Debug(FString("SaveSubsystem::SaveCurrentScene"));
	OnSceneSave.Broadcast();
}

void USaveSubsystem::SaveImportScenePath(const FString& ScenePath)
{
	int32 StartIndex = -1,EndIndex = -1;
	FString Key = "";
	USaveSubsystem::GetSceneName(ScenePath,Key,StartIndex,EndIndex);
	UPrintToolLibrary::Debug(FString::Printf(TEXT("SaveImportScenePath(key:%s,Value:%s)"),*Key,*ScenePath));

	FString JsonString;
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> ArrayValues;
	
	TSharedRef<FJsonObject> JsonObjectLocal = MakeShared<FJsonObject>();
	//
	if (UJsonToolLibrary::GetJsonString("ImportScenePathList",JsonString))
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			TArray<TSharedPtr<FJsonValue>> JsonValueArray = JsonObject->GetArrayField(TEXT("ScenePathList"));
			ArrayValues = JsonValueArray;
			int32 index = UJsonToolLibrary::GetArrayKeyIndex(ArrayValues,Key);
			if (index!=-1&&ArrayValues.IsValidIndex(index))
			{
				ArrayValues[index]->AsObject()->SetStringField("Key",Key);
				ArrayValues[index]->AsObject()->SetStringField("MapPath",ScenePath);
				JsonObject->SetArrayField("ScenePathList", ArrayValues);
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
				FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
				UJsonToolLibrary::SaveJsonToFile("ImportScenePathList",JsonString);
			}
			else
			{
				JsonObjectLocal->SetStringField("Key",Key);
				JsonObjectLocal->SetStringField("MapPath",ScenePath);
				ArrayValues.Add(MakeShared<FJsonValueObject>(JsonObjectLocal));
				JsonObject->SetArrayField("ScenePathList", ArrayValues);
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
				FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
				UJsonToolLibrary::SaveJsonToFile("ImportScenePathList",JsonString);
			}
		}
	}
	else
	{
		JsonObjectLocal->SetStringField("Key",Key);
		JsonObjectLocal->SetStringField("MapPath",ScenePath);
		ArrayValues.Add(MakeShared<FJsonValueObject>(JsonObjectLocal));
		JsonObject->SetArrayField("ScenePathList", ArrayValues);
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
		UJsonToolLibrary::SaveJsonToFile("ImportScenePathList",JsonString);
	}
}

void USaveSubsystem::GetSceneName(FString ScenePath,FString& Key,int32& start, int32& end)
{
	start = UKismetStringLibrary::FindSubstring(ScenePath,"/",false,true)+1;
	end = UKismetStringLibrary::FindSubstring(ScenePath,".udatasmith",false,true);
	Key = UKismetStringLibrary::GetSubstring(ScenePath,start,end - start);
}
