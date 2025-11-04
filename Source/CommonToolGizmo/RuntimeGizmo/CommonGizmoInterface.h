// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CommonGizmoInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCommonGizmoInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMMONTOOLGIZMO_API ICommonGizmoInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPointerEnter(UPrimitiveComponent* Component);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPointerMove(UPrimitiveComponent* Component);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPointerLeave(UPrimitiveComponent* Component);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnPointerDown(UPrimitiveComponent* Component,const FVector&RayStart,const FVector&RayEnd);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPointerDrag(UPrimitiveComponent* Component,const FVector&RayStart,const FVector&RayEnd);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnPointerUp(UPrimitiveComponent* Component);
};
