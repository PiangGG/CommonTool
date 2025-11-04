// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonToolCoreTags.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DocumentMarkComponent.generated.h"


class USceneManagerSubsystem;
class UStateSubsystem;
class UDocumentManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMMONTOOLCORE_API UDocumentMarkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDocumentMarkComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void OnSystemStateChange(FGameplayTag newState);

	UFUNCTION()
	void OnSceneChange(const FString& name);
public:	
	
private:
	UPROPERTY(EditAnywhere,Category="CommonToolCore|Region")
	FString RegionID;
	UPROPERTY(EditAnywhere,Category="CommonToolCore|Region")
	bool bRegisterPOI = true;
	UPROPERTY(EditAnywhere, Category = "CommonToolCore|LinePath")
	FVector LocationOffsize;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|POI")
	TSubclassOf<UUserWidget> DefaultPOI;
	UPROPERTY(EditAnywhere, config = CommonTool, Category="CommonToolCore|POI")
	FGameplayTag ShowStata = CommonToolTags::State_Null;
private:
	UPROPERTY()
	UDocumentManager *DocumentManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonToolCore|Info", meta=(AllowPrivateAccess=true))
	UUserWidget* UserWidget;
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	UPROPERTY()
	USceneManagerSubsystem *SceneManagerSubsystem;
};
