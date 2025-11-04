// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneTreeBuilder.h"

#include "SceneTreeNode.h"

USceneTreeNode* USceneTreeBuilder::BuildSceneTree(const FString& JsonString)
{
	// 创建根节点
	USceneTreeNode* Root = NewObject<USceneTreeNode>();
	Root->Initialize("Root",TSharedPtr<FJsonObject>());

	// 使用哈希表快速查找节点
	TMap<FString, USceneTreeNode*> NodeMap;
	NodeMap.Empty();
	NodeMap.Add("Root", Root);
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const FString& Key = "SceneList";
		TArray<TSharedPtr<FJsonValue>> SceneList = JsonObject->GetArrayField(Key);
		for (auto Scene : SceneList)
		{
			const TSharedPtr<FJsonObject> SceneJsonObject =  Scene->AsObject();
			
			if (SceneJsonObject.IsValid())
			{
				FString Id = "";
				SceneJsonObject->TryGetStringField(TEXT("Id"), Id);
				FString Path = "";
				SceneJsonObject->TryGetStringField(TEXT("Path"), Path);
				TArray<FString> PathParts;
				Path.ParseIntoArray(PathParts, TEXT("/"));
        
				FString CurrentPath = "Root";
				USceneTreeNode* CurrentNode = Root;
        
				for (const FString& Part : PathParts)
				{
					FString NewPath = CurrentPath + "/" + Part;
            
					if (!NodeMap.Contains(NewPath))
					{
						USceneTreeNode* NewNode = NewObject<USceneTreeNode>();
						NewNode->Initialize(Part,SceneJsonObject);
						CurrentNode->AddChild(NewNode);
						NodeMap.Add(NewPath, NewNode);
					}
            
					CurrentPath = NewPath;
					CurrentNode = NodeMap[CurrentPath];
				}
        
				// 设置叶节点ID
				if (CurrentNode && !Id.IsEmpty())
				{
					CurrentNode->Id = Id;
				}
			}
		}
	}
	return Root;
}
