// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonToolCoreTags.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "CommonCharacter.generated.h"

class UInspectionManager;
class UDeviceManager;
class USceneManagerSubsystem;
class UCommonCameraComponent;
class USpringArmComponent;
class UCameraMode;
class UInputMappingContext;
class UInputAction;
class UStateSubsystem;
class UInteractiveSubsystem;
class ULoadingSubsystem;

struct FGameplayTag;
struct FInputActionValue;

UCLASS()
class COMMONTOOLCORE_API ACommonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACommonCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//input
	void PanoramaMove(const FInputActionValue& Value);
	void PanoramaLook(const FInputActionValue& Value);
	void PanoramaZoom(const FInputActionValue& Value);

	void FirstPersonMove(const FInputActionValue& Value);
	void FirstPersonLook(const FInputActionValue& Value);

	void ThirdPersonMove(const FInputActionValue& Value);
	void ThirdPersonLook(const FInputActionValue& Value);

	void FocusMove(const FInputActionValue& Value);
	void FocusLook(const FInputActionValue& Value);
	void FocusZoom(const FInputActionValue& Value);

	void FreeMove(const FInputActionValue& Value);
	void FreeMoveUp(const FInputActionValue& Value);
	void FreeLook(const FInputActionValue& Value);

	bool LeftButtonHold = false;
	void LeftButtonHoldStart(const FInputActionValue& Value);
	void LeftButtonHoldEnd(const FInputActionValue& Value);

	bool RightButtonHold = false;
	void RightButtonHoldStart(const FInputActionValue& Value);
	void RightButtonHoldEnd(const FInputActionValue& Value);

	void LeftButtonStarted(const FInputActionValue& Value);
	void LeftButtonCompleted(const FInputActionValue& Value);

	void RightButtonStarted(const FInputActionValue& Value);
	void RightButtonCompleted(const FInputActionValue& Value);

	UFUNCTION()
	bool IsPressMouseLeft();
	
	virtual void NotifyControllerChanged() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	void ZoomIn();
	
	void ZoomAway();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	TSubclassOf<UCameraMode> DetermineCameraMode() const;

	UFUNCTION()
	void OnUserStateOnChange(FGameplayTag newState, const FTransform& Transform, float Zoom,bool bblend);
	UFUNCTION()
	void OnUserStateChangeCompletedSignature(const FTransform& Transform, float Zoom);
	UFUNCTION()
	void OnSystemStateOnChange(FGameplayTag newState);
	UFUNCTION()
	void OnDeviceStateOnChange(FGameplayTag newState,const FString& ID);
	UFUNCTION()
	void OnLoadScreenShowChange(bool bShow);

	UFUNCTION()
	void InspectionCheck(bool bInspection,const FTransform& Transform);
	UFUNCTION()
	void InspectionTransform(const FTransform& InTransform);
public:
	UFUNCTION()
	void OnSceneChangeComplete();
	//当前状态
	UPROPERTY()
	FGameplayTag CurrentUserState;

	UPROPERTY()
	FGameplayTag LastTimeUserState;
	
	UPROPERTY()
	FTransform LastStateTransform;
	UPROPERTY()
	FGameplayTag LastTimeDeviceState = CommonToolTags::DeviceState_Null;
	//
	UPROPERTY()
	float UpdateViewBlendAlpha = 0.0;
	//状态切换位置与控制器朝向插值切换
	UPROPERTY()
	FTimerHandle ViewChangeTimerHandle;

	UFUNCTION()
	void UpdateViewChanging(const FTransform &CurrentTransform, float PreviousZoomSize,const FTransform &TargetTransform,
	                         float TargetZoom);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Character")
	TObjectPtr<UCommonCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Character")
	USpringArmComponent* SpringArmComponent;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Character")
	// UPawnMovementComponent* FlyMovementComponent;
	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CommonToolCore|Character")

	TSubclassOf<UCameraMode> DefaultCameraMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CommonToolCore|Character")
	TSubclassOf<UCameraMode> CameraMode_Panorama;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CommonToolCore|Character")
	TSubclassOf<UCameraMode> CameraMode_FirstPerson;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CommonToolCore|Character")
	TSubclassOf<UCameraMode> CameraMode_ThirdPerson;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CommonToolCore|Character")
	TSubclassOf<UCameraMode> CameraMode_Focus;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CommonToolCore|Character")
	TSubclassOf<UCameraMode> CameraMode_Free;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CommonToolCore|Character")
	UCurveFloat* CurveFloat;
private:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* PanoramaMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* FirstPersonMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* ThirdPersonMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* FocusMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* FreeMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveUpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ZoomAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftButtonAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* RightButtonAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftButtonOnClickAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* RightButtonOnClickAction;
	UPROPERTY()
	AActor* LastInteractiveActor = nullptr;
	//聚焦状态下是否允许移动
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Input", meta = (AllowPrivateAccess = "true"))
	bool bEnableForceMove = false;
	//是否改用原始视角切换
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CommonToolCore|Character", meta = (AllowPrivateAccess = "true"))
	bool bUseOldView = true;
	UPROPERTY()
	UStateSubsystem* StateSubsystem;
	UPROPERTY()
	UInteractiveSubsystem* InteractiveSubsystem;
	UPROPERTY()
	USceneManagerSubsystem* SceneManagerSubsystem;
	UPROPERTY()
	UDeviceManager * DeviceManager;
	UPROPERTY()
	UInspectionManager * InspectionManager;
	UPROPERTY()
	ULoadingSubsystem* LoadingSubsystem;
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Default", meta = (AllowPrivateAccess = "true"))
	float PanoramaSpeed = 3000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Default", meta = (AllowPrivateAccess = "true"))
	float ForceSpeed = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Default", meta = (AllowPrivateAccess = "true"))
	float FirstPersonSpeed = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Default", meta = (AllowPrivateAccess = "true"))
	float ThirdPersonSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Default", meta = (AllowPrivateAccess = "true"))
	float FreeSpeed = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonToolCore|Default", meta = (AllowPrivateAccess = "true"))
	float LagInterpSpeed = 8.0f;
};
