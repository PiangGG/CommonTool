// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceStateComponent.h"

#include "StateSubsystem.h"
#include "Engine/AssetManager.h"

// Sets default values for this component's properties
UDeviceStateComponent::UDeviceStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDeviceStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	StateSubsystem =  UStateSubsystem::Get(this);
	if (StateSubsystem)
	{
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateChange);
		StateSubsystem->OperationStateOnChange.AddDynamic(this, &ThisClass::OnOperationStateChange);
		OnOperationStateChange(CommonToolTags::OperationState_1);
	}
	//初始化所有静态网格体的材质
	if (GetOwner())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(TargetMaterial.ToSoftObjectPath(),FStreamableDelegate::CreateLambda(
[this]()
		{
			CurrentMaterial = TargetMaterial.Get();
			if (CurrentMaterial)
			{
				TArray<UActorComponent*> MeshComponents =  GetOwner()->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
				MeshComponentMaterials.Empty();
				for (auto MeshComponent : MeshComponents)
				{
					if (TObjectPtr<UStaticMeshComponent> StaticMeshComponent =  Cast<UStaticMeshComponent>(MeshComponent))
					{
						TArray<TObjectPtr<UMaterialInterface>> Materials;
						for (auto MaterialInterface : StaticMeshComponent->GetMaterials())
						{
							Materials.Add(MaterialInterface);
						}
						MeshComponentMaterials.Add(StaticMeshComponent,Materials);
					}
				}
				//OnOperationStateChange(CommonToolTags::OperationState_1);
			}
		}));
		
	}
}

void UDeviceStateComponent::OnSystemStateChange(FGameplayTag newState)
{
	// if (bChangeVisibleState && GetOwner())
	// {
	// 	if (newState == CommonToolTags::State_SkylightOperation)
	// 	{
	// 		GetOwner()->SetActorHiddenInGame(bChangeVisibleState);
	// 	}
	// 	else
	// 	{
	// 		if (LastState== CommonToolTags::State_SkylightOperation)
	// 		{
	// 			GetOwner()->SetActorHiddenInGame(bChangeVisibleState);
	// 		}
	// 	}
	// }
	LastState = newState;
}

void UDeviceStateComponent::OnOperationStateChange(FGameplayTag newState)
{
	
	if (newState == CommonToolTags::OperationState_2)
	{
		if (CurrentMaterial)
		{
			for (auto Element : MeshComponentMaterials)
			{
				if (Element.Key)
				{
					for (int i = 0;i<Element.Value.Num();i++)
					{
						Element.Key->SetMaterial(i, CurrentMaterial);
					}
				}
			}
		}
		
		if (bChangeVisibleState)
		{
			GetOwner()->SetActorHiddenInGame(false);
		}
	}
	else if (newState== CommonToolTags::OperationState_1)
	{
		for (auto Element : MeshComponentMaterials)
		{
			if (Element.Key)
			{
				for (int i = 0;i<Element.Value.Num();i++)
				{
					Element.Key->SetMaterial(i, Element.Value[i]);
				}
			}
		}
		if (bChangeVisibleState)
		{
			GetOwner()->SetActorHiddenInGame(true);
		}
	}
}