// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTool/Library/StructLibrary.h"
#include "UObject/Interface.h"
#include "FunctionObjectInterface.generated.h"

class UFileMeshData;
class UFileData;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFunctionObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMMONTOOL_API IFunctionObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void GetViewportPosition(FVector& LongitudeLatitude, double& TheYaw, double& Pitch)const = 0;
	virtual void SetupViewportWithYawAndPitch(FVector LongitudeLatitude, double TheYaw, double Pitch) = 0;
	virtual void SetFunctionName(FText Name);
	virtual void DestroyTag() = 0;
	virtual FVector GetLongitudeLatitude()const = 0;

	virtual float GetViewDistance()const { return ViewDistance; }
	virtual void SetViewDistance(float TheDistance) { ViewDistance = TheDistance; }

	virtual int32 GetFontSize()const;
	virtual FLinearColor GetBackgroundColor()const;
	virtual FVector2D GetIconSize()const;
	virtual UTexture2D* GetIconStyle()const;
	virtual float GetExtraInfo()const;

	void SetFontSize(int32 Size);
	void SetBackgroundColor(FLinearColor& Color);
	void SetIconSize_XY(int32 X, int32 Y);
	void SetIconStyle(UTexture2D* Icon);
	void SetIconName(FString Name);

	virtual void SetShowIconStatus(bool TheStatus);

	virtual  void SetWorldResourceItem(UFileData* TheItem) { WorldResourceItem = TheItem; }
	virtual UFileData* GetResourceItem()const { return WorldResourceItem; }

	virtual TSubclassOf<UUserWidget> GetDetailSetupWidgetClass() { return DetailSetupWidgetClass; }

	FORCEINLINE FPropertyInfo& GetPropertyInfo() { return PropertyInfo; }

	FORCEINLINE void SetQuickMoveStatus(bool bStatus) { bAllowQuickMovement = bStatus; }
	FORCEINLINE bool IsQuickMoveStatus()const { return bAllowQuickMovement; }

	virtual void SetInGameHide(bool bHide) = 0;
	
	FORCEINLINE void SetShowTitleStatus(bool bStatus) {bShowTitle = bStatus;}
	FORCEINLINE bool IsShowTitleStatus()const { return bShowTitle; }

	FORCEINLINE FVector GetWorldOriginLongitudeLatitude()const { return WorldOriginLongitudeLatitude; }
	FORCEINLINE void SetWorldOriginLongitudeLatitude(FVector Location) { WorldOriginLongitudeLatitude = Location; }

	FORCEINLINE void SetShowFunctionObjectStatus(bool bStatus) { bShowFunctionObject = bStatus; }

	FORCEINLINE void SetIconResourcePath(FString Path) { PropertyInfo.IconResourcePath = Path; }

	FORCEINLINE void SetTitleIconFileMeshData(UFileMeshData* TheData) { TitleIconFilePath = TheData; }

	FORCEINLINE UFileMeshData* GetTitleIconFilePath() { return TitleIconFilePath; }
	
	virtual void SetTitleBasePropertyInfo(FPropertyInfo ThePropertyInfo, bool bStatus = false) = 0;

	void SetResourceReferenceIcon(UTexture2D* Icon) { ResourceReferenceIcon = Icon; }
	UTexture2D* GetResourceReferenceIcon() { return ResourceReferenceIcon; }

	void SetResourceReferencePath(UFileMeshData* TheResourceReferencePath) { ResourceReferencePathWidget = TheResourceReferencePath; }
	UFileMeshData* GetResourceReferencePath() { return ResourceReferencePathWidget; }

	virtual FVector GetObjectScale() = 0;
	virtual void SetObjectScale(const FVector& Scale) = 0;

	virtual FVector GetObjectLongitudeLatitude() = 0;
protected:
	

	UUserWidget* TitleWidget = nullptr;
	
	UFileData* WorldResourceItem = nullptr;

	TSubclassOf<UUserWidget> DetailSetupWidgetClass;
	
	float ViewDistance;

	FVector ViewportPosition;
	
	float YawAtDestination;
	
	float PitchAtDestination;

	APlayerController* PlayerController;

	FPropertyInfo PropertyInfo;

	bool bAllowQuickMovement;
	
	bool bShowTitle;
	
	FVector WorldOriginLongitudeLatitude;
	
	bool bShowFunctionObject;

	UFileMeshData* TitleIconFilePath;

	UFileMeshData* ResourceReferencePathWidget;

	UTexture2D* ResourceReferenceIcon;
};
