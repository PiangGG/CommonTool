// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonToolCoreTags.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "RegionMarkComponent.generated.h"


struct FGameplayTag;
class UStateSubsystem;
class USceneManagerSubsystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMMONTOOLCORE_API URegionMarkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URegionMarkComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnSystemStateChange(FGameplayTag newState);
public:
	UFUNCTION(BlueprintCallable)
	FString GetRegionID();
	UFUNCTION(BlueprintPure)
	int32 GetPOI0ffsize();
#if WITH_EDITOR
	UFUNCTION(BlueprintCallable,CallInEditor, Category = "Developer")
	void GenerateDevice();
#endif
private:
	UPROPERTY(EditAnywhere,Category="CommonToolCore|Region")
	FString RegionID;
	UPROPERTY(EditAnywhere,Category="CommonToolCore|Region")
	bool bRegisterScene = false;
	UPROPERTY(EditAnywhere,Category="CommonToolCore|Region")
	bool bRegisterPOI = true;
	UPROPERTY(EditAnywhere, Category = "CommonToolCore|LinePath")
	FVector LocationOffsize;
	UPROPERTY(EditAnywhere, Category = "CommonToolCore|LinePath")
	int32 POI0ffsize = 1;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|POI")
	TSubclassOf<UUserWidget> DefaultPOI;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|POI")
	FGameplayTag ShowStata = CommonToolTags::State_Null;
private:
	UPROPERTY()
	USceneManagerSubsystem *SceneManagerSubsystem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonToolCore|Info", meta=(AllowPrivateAccess=true))
	UUserWidget* UserWidget;
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
};
