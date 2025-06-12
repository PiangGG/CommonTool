// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CommonInfo.generated.h"

class UWidgetComponent;

struct FGameplayTag;
struct FEditItem;

UCLASS()
class COMMONTOOL_API ACommonInfo : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACommonInfo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void InteractiveOnClicked(AActor* Actor);

	UFUNCTION()
	void OnUserStateOnChange(FGameplayTag newState, const FTransform& Transform, float Zoom,bool bblend);

	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag newState);

	UFUNCTION()
	void SelectedChange(const FEditItem& Item, bool bSelected);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Info", meta=(AllowPrivateAccess=true))
	USceneComponent* Root;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Info", meta=(AllowPrivateAccess=true))
	UWidgetComponent* InfoWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CommonLibrary|Info", meta=(AllowPrivateAccess=true))
	TSubclassOf<UUserWidget> InfoWidgetClass;
	UPROPERTY()
	AActor* CurrentInteractiveActor;
	UPROPERTY()
	UMaterialInterface* OverlayMaterial;
};
