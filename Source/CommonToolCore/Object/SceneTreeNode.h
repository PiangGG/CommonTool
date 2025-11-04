// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneTreeNode.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class COMMONTOOLCORE_API USceneTreeNode : public UObject
{
	GENERATED_BODY()
public:
	USceneTreeNode();
	
	// 初始化节点
	void Initialize(const FString& InName,TSharedPtr<FJsonObject> InJsonObject,const FString& InId = "", int32 InLayer = 0)
	{
		Name = InName;
		JsonObject = InJsonObject;
		Id = InId;
		Layer = InLayer;
	}

	// 树操作接口
	UFUNCTION(BlueprintCallable, Category = "Tree")
	void AddChild(USceneTreeNode* ChildNode)
	{
		if(ChildNode && !Children.Contains(ChildNode))
		{
			Children.Add(ChildNode);
			ChildNode->Parent = this;
			ChildNode->Layer = Layer + 1;
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Tree")
	void RemoveFromParent()
	{
		if (Parent)
		{
			Parent->Children.Remove(this);
			Parent = nullptr;
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Tree")
	USceneTreeNode* FindNodeByName(const FString& InName)
	{
		// 检查当前节点是否匹配
		if (Name == InName)
		{
			return this;
		}

		// 递归检查所有子节点
		for (USceneTreeNode* Child : Children)
		{
			if (Child)
			{
				USceneTreeNode* FoundNode = Child->FindNodeByName(InName);
				if (FoundNode)
				{
					return FoundNode;
				}
			}
		}
		return nullptr;
	}
	
	UFUNCTION(BlueprintPure, Category = "Tree")
	bool IsLeaf() const { return Children.Num() == 0; }

	/**
	 * 获取该节点的所有叶子节点（递归DFS实现）
	 * @return 叶子节点数组
	 */
	UFUNCTION(BlueprintCallable, Category = "Tree")
	TArray<USceneTreeNode*> GetAllLeafNodes()
	{
		TArray<USceneTreeNode*> LeafNodes;
		GetLeafNodesRecursive(LeafNodes);
		return LeafNodes;
	}
	
	//树的Json数据
	TSharedPtr<FJsonObject> JsonObject;
	
	// 序列化为JSON
	UFUNCTION(BlueprintCallable, Category = "Tree")
	FString ToJsonString() const;
	
	// 属性
	UPROPERTY(BlueprintReadWrite, Category = "Tree")
	FString Name;
    
	UPROPERTY(BlueprintReadWrite, Category = "Tree")
	FString Id;
    
	UPROPERTY(BlueprintReadWrite, Category = "Tree")
	int32 Layer = 0;
    
	UPROPERTY(BlueprintReadOnly, Category = "Tree")
	USceneTreeNode* Parent = nullptr;
    
	UPROPERTY(BlueprintReadOnly, Category = "Tree")
	TArray<USceneTreeNode*> Children;
	// 私有递归辅助函数
private:
	void GetLeafNodesRecursive(TArray<USceneTreeNode*>& OutLeafNodes)
	{
		if (IsLeaf())
		{
			OutLeafNodes.Add(this);
			return;
		}

		for (USceneTreeNode* Child : Children)
		{
			if (Child)
			{
				Child->GetLeafNodesRecursive(OutLeafNodes);
			}
		}
	}
private:
	TSharedPtr<FJsonObject> ConvertToJsonObject() const;
};
