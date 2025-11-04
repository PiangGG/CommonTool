// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeToolLibrary.h"

#include "JsonToolLibrary.h"

bool UAttributeToolLibrary::GetStringAttribute(const TSharedPtr<FJsonObject>& JsonObject,
                                                   const FString& AttributeName, FString& AttributeValue)
{
	const TArray<TSharedPtr<FJsonValue>>* Attributes;
	if (JsonObject->TryGetArrayField(TEXT("Attribute"),Attributes))
	{
		for (const TSharedPtr<FJsonValue>& JsonValue : *Attributes)
		{
			if (TSharedPtr<FJsonObject> JsonObjectLocal = JsonValue->AsObject())
			{
				FString GetAttributeName;
				if (JsonObjectLocal->TryGetStringField(TEXT("AttributeName"),GetAttributeName))
				{
					if (AttributeName.Equals(GetAttributeName))
					{
						if (JsonObjectLocal->TryGetStringField(TEXT("AttributeValue"),AttributeValue))
						{
							return true;
						}
						return false;
					}
				}
			}
		}
	}
	return false;
}

bool UAttributeToolLibrary::GetBoolAttribute(const TSharedPtr<FJsonObject>& JsonObject,
	const FString& AttributeName, bool& AttributeValue)
{
	const TArray<TSharedPtr<FJsonValue>>* Attributes;
	if (JsonObject->TryGetArrayField(TEXT("Attribute"),Attributes))
	{
		for (const TSharedPtr<FJsonValue>& JsonValue : *Attributes)
		{
			if (TSharedPtr<FJsonObject> JsonObjectLocal = JsonValue->AsObject())
			{
				FString GetAttributeName;
				if (JsonObjectLocal->TryGetStringField(TEXT("AttributeName"),GetAttributeName))
				{
					if (AttributeName.Equals(GetAttributeName))
					{
						bool Value = false;
						if (JsonObjectLocal->TryGetBoolField(TEXT("AttributeValue"),Value))
						{
							AttributeValue = Value;
							return true;
						}
						return false;
					}
				}
			}
		}
	}
	return false;
}

bool UAttributeToolLibrary::GetAttribute(const FString& SceneName, const FString& GUID, const FString& AttributeName,
	FString& Attribute)
{
	if (SceneName.IsEmpty())
	{
		return false;
	}
	TSharedPtr<FJsonObject> RootJsonObject;
	FString TargetSceneData;
	//判断是否已经有场景数据
	if (UJsonToolLibrary::GetJsonString(SceneName,TargetSceneData,false,"SceneData"))
	{
		//序列化已保存的actor
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TargetSceneData);
		// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
		if (FJsonSerializer::Deserialize(JsonReader, RootJsonObject))
		{
			const TArray<TSharedPtr<FJsonValue>>* ArrayValues;

			if (RootJsonObject->TryGetArrayField(TEXT("Actors"),ArrayValues))
			{
				for (auto  JsonValue : *ArrayValues)
				{
					FString LocalGUID;
					JsonValue->AsObject()->TryGetStringField(TEXT("GUID"),LocalGUID);
					if (LocalGUID.Equals(GUID))
					{
						const TArray<TSharedPtr<FJsonValue>>* Attributes;
						if (JsonValue->AsObject()->TryGetArrayField(TEXT("Attribute"),Attributes))
						{
							for (const TSharedPtr<FJsonValue>& JsonValue2 : *Attributes)
							{
								if (TSharedPtr<FJsonObject> JsonObjectLocal = JsonValue2->AsObject())
								{
									FString GetAttributeName;
									if (JsonObjectLocal->TryGetStringField(TEXT("AttributeName"),GetAttributeName))
									{
										if (AttributeName.Equals(GetAttributeName))
										{
											UJsonToolLibrary::GetJsonStringFromJsonObject(JsonValue2->AsObject(),Attribute);
											return true;
										}
									}
								}
							}
						}
					}
					//拥有guid才递归在子类中寻找
					TArray<TSharedPtr<FJsonValue>> ChildrenArray = JsonValue->AsObject()->GetArrayField(TEXT("Children"));
					if (ChildrenArray.Num()>0)
					{
						for (auto Children : ChildrenArray)
						{
							TSharedPtr<FJsonObject> JsonObjectLocal = Children->AsObject();

							if (GetAttribute(SceneName,GUID,AttributeName,Attribute))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool UAttributeToolLibrary::GetAttributes(const FString& SceneName, const FString& GUID,
                                          TArray<FString>& Attributes)
{
	if (SceneName.IsEmpty())
	{
		return false;
	}
	TSharedPtr<FJsonObject> RootJsonObject;
	FString TargetSceneData;
	//判断是否已经有场景数据
	if (UJsonToolLibrary::GetJsonString(SceneName,TargetSceneData,false,"SceneData"))
	{
		//序列化已保存的actor
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TargetSceneData);
		// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
		if (FJsonSerializer::Deserialize(JsonReader, RootJsonObject))
		{
			const TArray<TSharedPtr<FJsonValue>>* ArrayValues;

			if (RootJsonObject->TryGetArrayField(TEXT("Actors"),ArrayValues))
			{
				for (auto  JsonValue : *ArrayValues)
				{
					FString LocalGUID;
					JsonValue->AsObject()->TryGetStringField(TEXT("GUID"),LocalGUID);
					if (LocalGUID.Equals(GUID))
					{
						const TArray<TSharedPtr<FJsonValue>>* AttributeObjects;
						if (JsonValue->AsObject()->TryGetArrayField(TEXT("Attribute"),AttributeObjects))
						{
							for (auto AttributeObject : *AttributeObjects)
							{
								FString Attribute;
								UJsonToolLibrary::GetJsonStringFromJsonObject(AttributeObject->AsObject(),Attribute);
								Attributes.AddUnique(Attribute);
							}
						}
						return true;
					}
					//拥有guid才递归在子类中寻找
					TArray<TSharedPtr<FJsonValue>> ChildrenArray = JsonValue->AsObject()->GetArrayField(TEXT("Children"));
					if (ChildrenArray.Num()>0)
					{
						for (auto Children : ChildrenArray)
						{
							TSharedPtr<FJsonObject> JsonObjectLocal = Children->AsObject();

							if (GetAttributes(SceneName,GUID,Attributes))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool UAttributeToolLibrary::GetActorStatic(FString SceneName, const FString& GUID)
{
	if (SceneName.IsEmpty())
	{
		return false;
	}
	TSharedPtr<FJsonObject> RootJsonObject;
	FString TargetSceneData;
	//判断是否已经有场景数据
	if (UJsonToolLibrary::GetJsonString(SceneName,TargetSceneData,false,"SceneData"))
	{
		//序列化已保存的actor
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TargetSceneData);
		// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
		if (FJsonSerializer::Deserialize(JsonReader, RootJsonObject))
		{
			const TArray<TSharedPtr<FJsonValue>>* ArrayValues;

			if (RootJsonObject->TryGetArrayField(TEXT("Actors"),ArrayValues))
			{
				for (auto  JsonValue : *ArrayValues)
				{
					FString LocalGUID;
					JsonValue->AsObject()->TryGetStringField(TEXT("GUID"),LocalGUID);
					if (LocalGUID.Equals(GUID))
					{
						bool bStatic = false;
						if (JsonValue->AsObject()->TryGetBoolField(TEXT("bStatic"),bStatic))
						{
							return true;
						}
						return false;
					}
					//拥有guid才递归在子类中寻找
					TArray<TSharedPtr<FJsonValue>> ChildrenArray = JsonValue->AsObject()->GetArrayField(TEXT("Children"));
					if (ChildrenArray.Num()>0)
					{
						for (auto Children : ChildrenArray)
						{
							TSharedPtr<FJsonObject> JsonObjectLocal = Children->AsObject();

							if (GetActorStatic(SceneName,GUID))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool UAttributeToolLibrary::MakeAttribute(const FString& AttributeName, const FString& AttributeValue,
	FString& ResultAttribute)
{
	TSharedPtr<FJsonObject> NewJsonObject = MakeShared<FJsonObject>();
	NewJsonObject->SetStringField(TEXT("AttributeName"), AttributeName);
	NewJsonObject->SetStringField(TEXT("AttributeValue"), AttributeValue);
	NewJsonObject->SetStringField(TEXT("AttributeType"), "EditType.Normal.Dynamic");
	NewJsonObject->SetBoolField(TEXT("bDisplay"), true);
	NewJsonObject->SetBoolField(TEXT("bCanEditor"), true);
	UJsonToolLibrary::GetJsonStringFromJsonObject(NewJsonObject,ResultAttribute);
	return true;
}
