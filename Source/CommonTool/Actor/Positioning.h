// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Positioning.generated.h"

UCLASS(Blueprintable,BlueprintType)
class COMMONTOOL_API APositioning : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APositioning();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString LocationName;
};
