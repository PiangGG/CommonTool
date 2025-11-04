// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "DeviceInterface.generated.h"

template <typename InterfaceType> class TScriptInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorInitializationCompleted, UObject*, Object);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActorReDraw);

// This class does not need to be modified.
UINTERFACE(MinimalAPI,Blueprintable,BlueprintType)
class UDeviceInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class COMMONTOOLCORE_API IDeviceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Device)
	AActor * GetActor();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Device)
	FString GetDeviceIndex();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Device)
	FString GetDeviceData();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Device)
	void AddHitData(FHitResult HitResult);
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category=Device)
	void GetAttributes(TArray<FString>& Attributes);
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category=Device)
	void GetAttributeValue(const FString& AttributeName,FString & AttributeValue);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Device)
	void InteractiveDevice();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Device)
	void DeInteractiveDevice();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Device)
	void SetDeviceLoad(const FString& bload);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Device)
	bool GetbIsCanReView();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Device)
	FString GetItemIcon();
	virtual TSharedRef<FJsonObject> GetDeviceJsonObjectData(const FString &GUID) = 0;
	virtual void SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject) = 0;
	
	virtual FOnActorInitializationCompleted* GetOnActorInitializationCompletedDelegate() { return nullptr; }
	virtual FOnActorReDraw* GetOnActorReDrawDelegate() { return nullptr; }
	
	static void ActorInitializationCompleted(TScriptInterface<IDeviceInterface> This);
	static void ActorReDraw(TScriptInterface<IDeviceInterface> This);
	
	FOnActorInitializationCompleted& GetOnActorInitializationCompletedDelegateChecked()
	{
		FOnActorInitializationCompleted* Result = GetOnActorInitializationCompletedDelegate();
		check(Result);
		return *Result;
	}

	FOnActorReDraw& GetOnOnActorReDrawDelegateChecked()
	{
		FOnActorReDraw* Result = GetOnActorReDrawDelegate();
		check(Result);
		return *Result;
	}
};
