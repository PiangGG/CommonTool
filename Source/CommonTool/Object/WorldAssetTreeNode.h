// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CommonTool/Library/JsonToolLibrary.h"
#include "WorldAssetTreeNode.generated.h"

class UTreeView;
/**
 * 
 */
UCLASS(BlueprintType)
class COMMONTOOL_API UWorldAssetTreeNode : public UObject
{
	GENERATED_BODY()
public:
	UWorldAssetTreeNode();

	UPROPERTY(BlueprintReadOnly, Category = "Tree")
	FString Data;
	UPROPERTY(BlueprintReadOnly, Category = "Tree")
	AActor* ActorPtr = nullptr;
	
	void Init(const TSharedPtr<FJsonObject>& InJsonObject)
	{
		JsonObject = InJsonObject;
		UJsonToolLibrary::GetJsonStringFromJsonObject(JsonObject,Data);
	}

	UFUNCTION(BlueprintCallable, Category = "Tree")
	bool GetAttribute(const FString& AttributeName, FString &AttributeValue);
	
	//树的Json数据
	TSharedPtr<FJsonObject> JsonObject;
	
	UPROPERTY(BlueprintReadWrite, Category = "Tree")
	TArray<UWorldAssetTreeNode*> Children;
    
	UPROPERTY(BlueprintReadWrite, Category = "Tree")
	UWorldAssetTreeNode* Parent = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Tree")
	int32 Layer = 0;

	UFUNCTION(BlueprintCallable, Category = "Tree")
	void RemoveFromParent()
	{
		if (Parent != nullptr)
		{
			Parent->Children.Remove(this);
		}
	}
	
	UFUNCTION(BlueprintCallable, Category = "Tree")
	void AddChild(UWorldAssetTreeNode* ChildNode)
	{
		if(ChildNode)
		{
			Children.Add(ChildNode);
			ChildNode->Parent = this;
		}
	}
    
	UFUNCTION(BlueprintCallable, Category = "Tree")
	bool IsLeaf() const
	{
		return Children.Num() == 0;
	}
};
