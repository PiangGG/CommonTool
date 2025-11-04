// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DatasmithRuntime.h"
#include "ComDataSmithActor.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class COMMONTOOLCORE_API AComDatasmithActor : public ADatasmithRuntimeActor
{
	GENERATED_BODY()

public:
	AComDatasmithActor();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;
	
	virtual void OnOpenDelta() override;
	virtual void OnImportEnd() override;

public:
};
