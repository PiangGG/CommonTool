// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneTreeNode.h"

USceneTreeNode::USceneTreeNode()
{
	
}

FString USceneTreeNode::ToJsonString() const
{
	TSharedPtr<FJsonObject> JsonObject = ConvertToJsonObject();
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	return OutputString;
}

TSharedPtr<FJsonObject> USceneTreeNode::ConvertToJsonObject() const
{
	TSharedPtr<FJsonObject> JsonNode = MakeShareable(new FJsonObject);
    
	JsonNode->SetStringField("name", Name);
    
	if (!Id.IsEmpty())
	{
		JsonNode->SetStringField("id", Id);
	}
    
	if (Children.Num() > 0)
	{
		TArray<TSharedPtr<FJsonValue>> ChildrenArray;
		for (const USceneTreeNode* Child : Children)
		{
			ChildrenArray.Add(MakeShareable(new FJsonValueObject(Child->ConvertToJsonObject())));
		}
		JsonNode->SetArrayField("children", ChildrenArray);
	}
    
	return JsonNode;
}
