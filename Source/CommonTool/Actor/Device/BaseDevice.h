// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTool/Interface/DeviceInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BaseDevice.generated.h"

class UDeviceMarkComponent;

struct FGameplayTag;
UCLASS(BlueprintType, Blueprintable)
class COMMONTOOL_API ABaseDevice : public AActor, public IDeviceInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseDevice();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	
	//Begin IDeviceInterface
	virtual AActor* GetActor_Implementation() override;
	virtual FString GetDeviceData_Implementation() override;
	virtual FString GetDeviceIndex_Implementation() override;
	virtual void InteractiveDevice_Implementation() override;
	virtual void DeInteractiveDevice_Implementation() override;
	virtual TSharedRef<FJsonObject> GetDeviceJsonObjectData(const FString &GUID) override;
	virtual void SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject) override;
	virtual FOnActorInitializationCompleted* GetOnActorInitializationCompletedDelegate() override;
	virtual FOnActorReDraw* GetOnActorReDrawDelegate() override;
	virtual void SetDeviceLoad_Implementation(const FString& bload) override;
	virtual bool GetbIsCanReView_Implementation() override;
	virtual FString GetItemIcon_Implementation() override;
	virtual void GetAttributeValue_Implementation(const FString& AttributeName, FString& AttributeValue) override;
	virtual void GetAttributes_Implementation(TArray<FString>& Attributes) override;
	//End IDeviceInterface

protected:
	UFUNCTION()
	virtual void ReDraw();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Device", meta=(AllowPrivateAccess=true))
	USceneComponent* Root;
	//meshcomp 设备静态模型
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Device", meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* StaticMesh;
	//boxcomp 设备交互模型
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Device", meta=(AllowPrivateAccess=true))
	UBoxComponent* BoxCollision;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Device", meta=(AllowPrivateAccess=true))
	// UDeviceMarkComponent* DeviceMarkComponent;

	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag newState);
	
	virtual void SetEditing(bool edit);

private:
	UPROPERTY(EditAnywhere, Category=Device)
	FTransform Transform;
	
	UPROPERTY(EditAnywhere, Category=Device)
	FString DeviceIndex;
	
	UPROPERTY(BlueprintAssignable, Category=Device)
	FOnActorInitializationCompleted OnActorInitializationCompletedDelege;
	UPROPERTY()
	FOnActorReDraw OnActorReDrawDelege;
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Device)
	FString ItemIcon = "BaseDevice";
protected:
	//Json字符串
	UPROPERTY(EditAnywhere, Category=Device)
	FString Data;
	//是否加载与显示，根据actor不同改变
	UPROPERTY(EditAnywhere, Category=Device)
	bool BIsLoad = true;
	UPROPERTY(EditAnywhere, Category=Device)
	bool bReView = false;
	UPROPERTY(EditAnywhere, Category=Device)
	bool bCanEdit = true;
};