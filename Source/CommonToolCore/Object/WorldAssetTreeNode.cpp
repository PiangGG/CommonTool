// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAssetTreeNode.h"

#include "CommonToolCore/Library/JsonToolLibrary.h"

UWorldAssetTreeNode::UWorldAssetTreeNode():JsonObject(nullptr)
{
	
}

bool UWorldAssetTreeNode::GetAttribute(const FString& AttributeName, FString& AttributeValue)
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
						 UJsonToolLibrary::GetJsonStringFromJsonObject(JsonValue->AsObject(),AttributeValue);
						return true;
					}
				}
			}
		}
	}
	return false;
}
