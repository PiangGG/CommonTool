// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "CommonToolCoreTags.h"
#include "DeviceManager.h"
#include "InspectionManager.h"
#include "JsonToolLibrary.h"
#include "SceneManagerSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "CommonToolCore/Camera/CameraMode.h"
#include "CommonToolCore/Camera/CommonCameraComponent.h"
#include "CommonSettings.h"
#include "LoadingSubsystem.h"
#include "CommonToolCore/Library/PrintToolLibrary.h"
#include "CommonToolCore/SubSystem/InteractiveSubsystem.h"
#include "CommonToolCore/SubSystem/StateSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACommonCharacter::ACommonCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Create a camera boom (pulls in towards the player if there is a collision)
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300; // The camera follows at this distance behind the character	
	SpringArmComponent->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	CameraComponent = CreateDefaultSubobject<UCommonCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(0.0f));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//FlyMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FlyMovementComponent"));
}

// Called when the game starts or when spawned
void ACommonCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CameraComponent)
	{
		CameraComponent->SetIsUseOld(bUseOldView);
		if (!bUseOldView)
		{
			CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}
	}

	StateSubsystem = GetGameInstance()->GetSubsystem<UStateSubsystem>();
	if (StateSubsystem)
	{
		StateSubsystem->UserStateOnChange.AddDynamic(this, &ThisClass::OnUserStateOnChange);
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);
		StateSubsystem->OnUserStateChangeCompletedSignature.AddDynamic(this, &ThisClass::OnUserStateChangeCompletedSignature);
		StateSubsystem->DeviceStateOnChange.AddDynamic(this, &ThisClass::OnDeviceStateOnChange);
	}

	SceneManagerSubsystem = UWorld::GetSubsystem<USceneManagerSubsystem>(GWorld);
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->OnSceneChangeCompleteSignature.AddDynamic(this,&ThisClass::OnSceneChangeComplete);
	}
	InteractiveSubsystem = UWorld::GetSubsystem<UInteractiveSubsystem>(GWorld);
	
	DeviceManager = UWorld::GetSubsystem<UDeviceManager>(GWorld);
	
	if (StateSubsystem)
	{
		StateSubsystem->ChangeSystemState(CommonToolTags::State_Null);
	}
	InspectionManager = UInspectionManager::Get(this);
	if (InspectionManager)
	{
		InspectionManager->InspectionCheckSignature.AddDynamic(this,&ThisClass::InspectionCheck);
		InspectionManager->InspectionTransformSignature.AddDynamic(this,&ThisClass::InspectionTransform);
	}
}

void ACommonCharacter::PanoramaMove(const FInputActionValue& Value)
{
	if (LeftButtonHold)
	{
		if (CurrentUserState == CommonToolTags::UserState_Panorama)
		{
			// input is a Vector2D
			FVector2D MovementVector = Value.Get<FVector2D>();

			if (Controller != nullptr)
			{
				// find out which way is forward
				const FRotator Rotation = GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get forward vector
				const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

				// get right vector 
				const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

				// add movement 
				AddMovementInput(ForwardDirection, MovementVector.Y);
				AddMovementInput(RightDirection, MovementVector.X);
			}
		}
	}
}

void ACommonCharacter::PanoramaLook(const FInputActionValue& Value)
{
	// if (InteractiveSubsystem->GetIsDragging())
	// {
	// 	return;
	// }
	//
	// if (RightButtonHold)
	// {
	// 	if (CurrentUserState == CommonToolTags::UserState_Panorama)
	// 	{
	// 		// input is a Vector2D
	// 		FVector2D LookAxisVector = Value.Get<FVector2D>();
	//
	// 		if (Controller != nullptr)
	// 		{
	// 			// add yaw and pitch input to controller
	// 			AddControllerYawInput(LookAxisVector.X);
	// 			AddControllerPitchInput(LookAxisVector.Y);
	// 		}
	// 	}
	// }
}

void ACommonCharacter::PanoramaZoom(const FInputActionValue& Value)
{
	if (CurrentUserState == CommonToolTags::UserState_Panorama)
	{
		// input is a Vector2D
		float movesize = Value.Get<float>();

		if (Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = GetControlRotation();

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);

			// add movement 
			AddMovementInput(ForwardDirection, movesize);
			GetCharacterMovement()->MaxFlySpeed = PanoramaSpeed *  GetActorTransform().GetLocation().Z*2;
			GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->MaxFlySpeed*10;
			GetCharacterMovement()->BrakingDecelerationFlying = GetCharacterMovement()->MaxAcceleration/2;
			CameraComponent->ZoomChangeSignature.Broadcast(SpringArmComponent->TargetArmLength);
		}
	}
}

void ACommonCharacter::FirstPersonMove(const FInputActionValue& Value)
{
	if (CurrentUserState == CommonToolTags::UserState_FirstPerson)
	{
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			const FRotator ControlSpaceRot = Controller->GetControlRotation();
			if (MovementVector.Y != 0.f)
			{
				// transform to world space and add it
				AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), MovementVector.Y);
			}
			if (MovementVector.X != 0.f)
			{
				// transform to world space and add it
				AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), MovementVector.X);
			}
		}
	}
}

void ACommonCharacter::FirstPersonLook(const FInputActionValue& Value)
{
	if (CurrentUserState == CommonToolTags::UserState_FirstPerson)
	{
		// input is a Vector2D
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
	}
}

void ACommonCharacter::ThirdPersonMove(const FInputActionValue& Value)
{
	if (CurrentUserState == CommonToolTags::UserState_ThirdPerson)
	{
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void ACommonCharacter::ThirdPersonLook(const FInputActionValue& Value)
{
	if (InteractiveSubsystem->GetIsDragging())
	{
		return;
	}
	if (CurrentUserState == CommonToolTags::UserState_ThirdPerson)
	{
		// input is a Vector2D
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
	}
}

void ACommonCharacter::FocusMove(const FInputActionValue& Value)
{
	if (CurrentUserState == CommonToolTags::UserState_Focus && !InteractiveSubsystem->GetIsDragging())
	{
		if (LeftButtonHold&&!RightButtonHold)
		{
			// input is a Vector2D
			FVector2D MovementVector = Value.Get<FVector2D>();

			if (Controller != nullptr)
			{
				// find out which way is forward
				const FRotator Rotation = GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get forward vector
				const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

				// get right vector 
				const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

				// add movement 
				AddMovementInput(ForwardDirection, MovementVector.Y);
				AddMovementInput(RightDirection, MovementVector.X);
			}
		}
	}
}

void ACommonCharacter::FocusLook(const FInputActionValue& Value)
{
	if (InteractiveSubsystem->GetIsDragging())
	{
		return;
	}
	if (CurrentUserState == CommonToolTags::UserState_Focus)
	{
		if (!LeftButtonHold&&RightButtonHold)
		{
			// input is a Vector2D
			FVector2D LookAxisVector = Value.Get<FVector2D>();

			if (Controller != nullptr)
			{
				// add yaw and pitch input to controller
				AddControllerYawInput(LookAxisVector.X);
				FRotator CurrentRotation = Controller->GetControlRotation();
				float NewPitch = CurrentRotation.Pitch + LookAxisVector.Y;
				CurrentRotation.Pitch = FMath::ClampAngle (NewPitch, 275.0f, 355.0f);
				Controller->SetControlRotation(CurrentRotation);
			}
		}
	}
}

void ACommonCharacter::FocusZoom(const FInputActionValue& Value)
{
	if (CurrentUserState == CommonToolTags::UserState_Focus)
	{
		float ZoomFactor = Value.Get<float>();
		if (ZoomFactor > 0.0f)
		{
			ZoomIn();
		}
		if (ZoomFactor < 0.0f)
		{
			ZoomAway();
		}
		GetCharacterMovement()->MaxFlySpeed = ForceSpeed*CameraComponent->GetZoomSize();
		GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->MaxFlySpeed*20;
		GetCharacterMovement()->BrakingDecelerationFlying = GetCharacterMovement()->MaxAcceleration/2;
	}
}

void ACommonCharacter::FreeMove(const FInputActionValue& Value)
{
	if (CurrentUserState == CommonToolTags::UserState_Free)
	{
		// input is a Vector2D
		FVector MovementVector = Value.Get<FVector>();

		if (Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);

			// get right vector 
			const FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
			
			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
			AddMovementInput(FVector::UpVector, MovementVector.Z);
		}
	}
}

void ACommonCharacter::FreeMoveUp(const FInputActionValue& Value)
{
	if (CurrentUserState == CommonToolTags::UserState_Free)
	{
		float Driction = Value.Get<float>();

		AddMovementInput(FVector::UpVector, Driction);
	}
}

void ACommonCharacter::FreeLook(const FInputActionValue& Value)
{
	if (CurrentUserState == CommonToolTags::UserState_Free)
	{
		// input is a Vector2D
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
	}
}

void ACommonCharacter::LeftButtonHoldStart(const FInputActionValue& Value)
{
	LeftButtonHold = true;
}

void ACommonCharacter::LeftButtonHoldEnd(const FInputActionValue& Value)
{
	LeftButtonHold = false;
}

void ACommonCharacter::RightButtonHoldStart(const FInputActionValue& Value)
{
	RightButtonHold = true;
}

void ACommonCharacter::RightButtonHoldEnd(const FInputActionValue& Value)
{
	RightButtonHold = false;
}

void ACommonCharacter::LeftButtonStarted(const FInputActionValue& Value)
{
	if (InteractiveSubsystem)
	{
		InteractiveSubsystem->OnPressLeftPointer();
	}
}

void ACommonCharacter::LeftButtonCompleted(const FInputActionValue& Value)
{
	if (InteractiveSubsystem)
	{
		InteractiveSubsystem->OnReleaseLeftPointer();
	}
}

void ACommonCharacter::RightButtonStarted(const FInputActionValue& Value)
{
	if (InteractiveSubsystem)
	{
		InteractiveSubsystem->OnPressRightPointer();
	}
}

void ACommonCharacter::RightButtonCompleted(const FInputActionValue& Value)
{
	if (InteractiveSubsystem)
	{
		InteractiveSubsystem->OnReleaseRightPointer();
	}
}

bool ACommonCharacter::IsPressMouseLeft()
{
	return LeftButtonHold;
}

void ACommonCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

void ACommonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	PlayerController = Cast<APlayerController>(NewController);
}

void ACommonCharacter::UnPossessed()
{
	Super::UnPossessed();
	PlayerController = nullptr;
}

void ACommonCharacter::ZoomIn()
{
	CameraComponent->ZoomIn();
}

void ACommonCharacter::ZoomAway()
{
	CameraComponent->ZoomAway();
}


// Called to bind functionality to input
void ACommonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		// Looking
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACommonCharacter::FocusMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACommonCharacter::FocusLook);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ACommonCharacter::FocusZoom);
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACommonCharacter::PanoramaMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACommonCharacter::PanoramaLook);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ACommonCharacter::PanoramaZoom);
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
		                                   &ACommonCharacter::FirstPersonMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
		                                   &ACommonCharacter::FirstPersonLook);
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
		                                   &ACommonCharacter::ThirdPersonMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
		                                   &ACommonCharacter::ThirdPersonLook);
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACommonCharacter::FreeMove);
		EnhancedInputComponent->BindAction(MoveUpAction, ETriggerEvent::Triggered, this, &ACommonCharacter::FreeMoveUp);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACommonCharacter::FreeLook);

		EnhancedInputComponent->BindAction(LeftButtonAction, ETriggerEvent::Started, this,
		                                   &ACommonCharacter::LeftButtonHoldStart);
		EnhancedInputComponent->BindAction(LeftButtonAction, ETriggerEvent::Completed, this,
		                                   &ACommonCharacter::LeftButtonHoldEnd);

		EnhancedInputComponent->BindAction(RightButtonAction, ETriggerEvent::Started, this,
		                                   &ACommonCharacter::RightButtonHoldStart);
		EnhancedInputComponent->BindAction(RightButtonAction, ETriggerEvent::Completed, this,
		                                   &ACommonCharacter::RightButtonHoldEnd);

		EnhancedInputComponent->BindAction(LeftButtonOnClickAction, ETriggerEvent::Started, this,
		                                   &ACommonCharacter::LeftButtonStarted);
		EnhancedInputComponent->BindAction(LeftButtonOnClickAction, ETriggerEvent::Completed, this,
		                                   &ACommonCharacter::LeftButtonCompleted);
		EnhancedInputComponent->BindAction(RightButtonOnClickAction, ETriggerEvent::Started, this,
										   &ACommonCharacter::RightButtonStarted);
		EnhancedInputComponent->BindAction(RightButtonOnClickAction, ETriggerEvent::Completed, this,
										   &ACommonCharacter::RightButtonCompleted);
	}
	else
	{
		UPrintToolLibrary::Error(TEXT(
			"Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."));
	}
}


TSubclassOf<UCameraMode> ACommonCharacter::DetermineCameraMode() const
{
	if (CameraComponent->GetActiveCameraMode())
	{
		return CameraComponent->GetActiveCameraMode();
	}
	return DefaultCameraMode;
}

void ACommonCharacter::OnUserStateOnChange(FGameplayTag newState, const FTransform& Transform, float Zoom,bool bblend)
{
	ULoadingSubsystem::Get(this)->ShowLoadScreen(true);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<

		UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			float PreviousZoomSize = CameraComponent->GetZoomSize();

			if (newState != CurrentUserState)
			{
				Subsystem->ClearAllMappings();
				LastTimeUserState = CurrentUserState;
				CurrentUserState = newState;
				SpringArmComponent->SocketOffset = FVector::ZeroVector;
				if (newState == CommonToolTags::UserState_Focus)
				{
					Subsystem->AddMappingContext(FocusMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_Focus);
					CameraComponent->bUsePawnControlRotation = true;
					SpringArmComponent->bUsePawnControlRotation = true;
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					GetCapsuleComponent()->SetEnableGravity(false);

					GetCharacterMovement()->SetMovementMode(MOVE_Flying);
					GetCharacterMovement()->MaxFlySpeed = ForceSpeed*Zoom;
					GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->MaxFlySpeed*20;
					GetCharacterMovement()->BrakingDecelerationFlying = GetCharacterMovement()->MaxAcceleration/2;
				}

				if (newState == CommonToolTags::UserState_Panorama)
				{
					Subsystem->AddMappingContext(PanoramaMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_Panorama);
					CameraComponent->bUsePawnControlRotation = true;
					SpringArmComponent->bUsePawnControlRotation = true;
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					GetCapsuleComponent()->SetEnableGravity(false);

					GetCharacterMovement()->SetMovementMode(MOVE_Flying);
					GetCharacterMovement()->MaxFlySpeed = PanoramaSpeed * Transform.GetLocation().Z*2;
					GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->MaxFlySpeed*10;
					GetCharacterMovement()->BrakingDecelerationFlying = GetCharacterMovement()->MaxAcceleration/2;
				}

				if (newState == CommonToolTags::UserState_FirstPerson)
				{
					Subsystem->AddMappingContext(FirstPersonMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_FirstPerson);
					CameraComponent->bUsePawnControlRotation = true;
					SpringArmComponent->bUsePawnControlRotation = true;
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					GetCapsuleComponent()->SetEnableGravity(true);
					GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					GetCharacterMovement()->MaxWalkSpeed = FirstPersonSpeed;
					GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->MaxWalkSpeed*20;
					GetCharacterMovement()->BrakingDecelerationFlying = GetCharacterMovement()->MaxAcceleration/2;
				}

				if (newState == CommonToolTags::UserState_ThirdPerson)
				{
					Subsystem->AddMappingContext(ThirdPersonMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_ThirdPerson);
					CameraComponent->bUsePawnControlRotation = true;
					SpringArmComponent->bUsePawnControlRotation = true;
					SpringArmComponent->SocketOffset = FVector(200.000000,30.000000,75.000000);
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					GetCapsuleComponent()->SetEnableGravity(true);
					GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					GetCharacterMovement()->MaxWalkSpeed = ThirdPersonSpeed;
					GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->MaxWalkSpeed*20;
					GetCharacterMovement()->BrakingDecelerationFlying = GetCharacterMovement()->MaxAcceleration/2;
					
				}

				if (newState == CommonToolTags::UserState_Free)
				{
					Subsystem->AddMappingContext(FreeMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_Free);
					CameraComponent->bUsePawnControlRotation = true;
					SpringArmComponent->bUsePawnControlRotation = true;
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					GetCapsuleComponent()->SetEnableGravity(true);
					GetCharacterMovement()->SetMovementMode(MOVE_Flying);
					GetCharacterMovement()->MaxFlySpeed = FreeSpeed;
					GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->MaxFlySpeed*20;
					GetCharacterMovement()->BrakingDecelerationFlying = GetCharacterMovement()->MaxAcceleration/2;
				}

				if (newState == CommonToolTags::UserState_AutoRoam)
				{
					Subsystem->AddMappingContext(FirstPersonMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_ThirdPerson);
					CameraComponent->bUsePawnControlRotation = false;
					SpringArmComponent->bUsePawnControlRotation = false;
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					GetCapsuleComponent()->SetEnableGravity(false);
					GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					GetCharacterMovement()->MaxWalkSpeed = FirstPersonSpeed;
					GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->MaxWalkSpeed*20;
					GetCharacterMovement()->BrakingDecelerationFlying = GetCharacterMovement()->MaxAcceleration/2;
				}
				
				if (CurrentUserState == CommonToolTags::UserState_Null)
				{
					return;
				}
			}else if (newState == CurrentUserState && newState == CommonToolTags::UserState_Focus)
			{
				Subsystem->AddMappingContext(FocusMappingContext, 0);
				CameraComponent->PushCameraMode(CameraMode_Focus);
				CameraComponent->bUsePawnControlRotation = true;
				SpringArmComponent->bUsePawnControlRotation = true;
				GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				GetCapsuleComponent()->SetEnableGravity(false);

				GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				GetCharacterMovement()->MaxFlySpeed = ForceSpeed*Zoom;
				GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->MaxFlySpeed*20;
				GetCharacterMovement()->BrakingDecelerationFlying = GetCharacterMovement()->MaxAcceleration/2;
			}

			if (LastTimeUserState == CommonToolTags::UserState_Null)
			{
				LastTimeUserState = newState;
				return;
			}

			if (bblend)
			{
				GetWorld()->GetTimerManager().ClearTimer(ViewChangeTimerHandle);
				FTransform CurrentTransform;
				CurrentTransform.SetRotation(GetControlRotation().Quaternion());
				CurrentTransform.SetLocation(GetActorLocation());
				UpdateViewBlendAlpha = 0.0f;
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, FName("UpdateViewChanging"), CurrentTransform, PreviousZoomSize,
											Transform, Zoom);
				GetWorld()->GetTimerManager().SetTimer(ViewChangeTimerHandle, TimerDelegate, 0.001, true);
			}
			else
			{
				SpringArmComponent->bEnableCameraLag = false;
				SpringArmComponent->bEnableCameraRotationLag = false;
				
				GetWorld()->GetTimerManager().ClearTimer(ViewChangeTimerHandle);
				SetActorLocation(Transform.GetLocation(), false, nullptr, ETeleportType::TeleportPhysics);
				GetController()->SetControlRotation(Transform.Rotator());
				CameraComponent->SetZoom(Zoom);
				StateSubsystem->OnUserStateChangeCompletedSignature.Broadcast(Transform, Zoom);
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]()
				{
					SpringArmComponent->bEnableCameraLag = true;
					SpringArmComponent->bEnableCameraRotationLag = true;
				}), 0.1, false);
			}
		}
	}
}

void ACommonCharacter::OnUserStateChangeCompletedSignature(const FTransform& Transform, float Zoom)
{
	ULoadingSubsystem::Get(this)->ShowLoadScreen(false);
}

void ACommonCharacter::OnSystemStateOnChange(FGameplayTag newState)
{
	if (newState == CommonToolTags::State_Null)
	{
		FTransform Transform;
		if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
		{
			Transform = CommonSettings->InitTransform;
		}
		else
		{
			Transform.SetLocation(FVector(-177302.2, -29375.4, 456969.3));
			Transform.SetRotation(FRotator(-76.1, 736.6, 701.2).Quaternion());
		}
		
		if (StateSubsystem)
		{
			StateSubsystem->ChangeUserState(CommonToolTags::UserState_Panorama,Transform,0);
		}
	}
	else if (newState == CommonToolTags::State_MainMenu)
	{
		if (SceneManagerSubsystem)
		{
			if (SceneManagerSubsystem->GetCurrentSceneName().IsEmpty())
			{
				return;
			}
			FString JsonSceneData;
		
			FTransform Transform;
			if (SceneManagerSubsystem->	GetCurrentSceneTransform(Transform) && SceneManagerSubsystem->GetCurrentJsonSceneData(JsonSceneData))
			{
				FString TransformString;
				UJsonToolLibrary::GetStringFromJsonString(JsonSceneData,"Transform",TransformString);
				FTransform TransformOffsize;
				TransformOffsize.InitFromString(TransformString);
				FTransform TargetTransform;
				TargetTransform.SetLocation(Transform.GetLocation()+TransformOffsize.GetLocation());
				TargetTransform.SetRotation(TransformOffsize.GetRotation());
				StateSubsystem->ChangeUserState(CommonToolTags::UserState_Focus,TargetTransform,10000.0);
			}
		}
	}else if (newState == CommonToolTags::State_Roam)
	{
		StateSubsystem->ChangeUserState(CommonToolTags::UserState_FirstPerson, GetActorTransform(), 0,true);
	}
}

void ACommonCharacter::OnDeviceStateOnChange(FGameplayTag newState,const FString& ID)
{
	if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
	{
		if (LastTimeDeviceState == CommonToolTags::DeviceState_Disassemble)
		{
			//上一次在拆解状态
			if (newState == CommonToolTags::DeviceState_Disassemble)
			{
				//新状态在拆解不做处理
			}
			else
			{
				//新状态不在拆解移动回场景不插值
				LastStateTransform.SetRotation(GetControlRotation().Quaternion());
				OnUserStateOnChange(CommonToolTags::UserState_Focus, LastStateTransform, CameraComponent->GetZoomSize(),false);
			}
		}
		else
		{
			//上次状态不在拆解
			if (newState == CommonToolTags::DeviceState_Disassemble)
			{
				FVector Origin;
				FVector BoxExtent;
				DeviceManager->GetFocusDevice()->GetActorBounds(false, Origin, BoxExtent);
				
				//新状态在拆解
				//记录一下拆解状态前的位置,用于取消拆解
				LastStateTransform = GetActorTransform();
				LastStateTransform.SetRotation(GetControlRotation().Quaternion());
				LastStateTransform.SetLocation(GetActorLocation());
				FTransform TargetTransform;
				TargetTransform = CommonSettings->DeviceDisassembleTransform;
				TargetTransform.SetRotation(GetControlRotation().Quaternion());
				FVector TargetLocation = TargetTransform.GetLocation()+FVector(0,0,BoxExtent.Z);
				TargetTransform.SetRotation(GetControlRotation().Quaternion());
				TargetTransform.SetLocation(TargetLocation);
				OnUserStateOnChange(CommonToolTags::UserState_Focus, TargetTransform, CameraComponent->GetZoomSize(),false);
			}
			else if (newState == CommonToolTags::DeviceState_Ledger)
			{
				if (DeviceManager)
				{
					if (DeviceManager->GetFocusDevice())
					{
						FVector Origin;
						FVector BoxExtent;
			
						DeviceManager->GetFocusDevice()->GetActorBounds(false, Origin, BoxExtent);
			
						FTransform Transform;
						Transform.SetLocation(Origin);
						FRotator Rotator = GetActorRotation();
						Rotator.Pitch = -45.0f;
						Transform.SetRotation(Rotator.Quaternion());
						
						if (StateSubsystem)
						{
							StateSubsystem->ChangeUserState(CommonToolTags::UserState_Focus, Transform, BoxExtent.Length() * CommonSettings->Forcesize);
						}
			
						if (UPrintToolLibrary::DrawDebug())
						{
							DrawDebugBox(GetWorld(), Origin, BoxExtent, FColor::Green, false, 5);
						}
					}	
				}
				else
				{
					UPrintToolLibrary::Waring(FString::Printf(TEXT("ForceActor is null")));
				}
			
			}
			else
			{
				//新状态不在拆解不做处理
			}
		}
		LastTimeDeviceState = newState;	
	}
}

void ACommonCharacter::OnLoadScreenShowChange(bool bShow)
{
	if (bShow)
	{
		OnUserStateOnChange(CommonToolTags::UserState_Null, GetActorTransform(),
		                    CameraComponent->GetPreviousZoomSize(),true);
	}
	else
	{
		OnUserStateOnChange(LastTimeUserState, GetActorTransform(), CameraComponent->GetPreviousZoomSize(),true);
	}
}

void ACommonCharacter::InspectionCheck(bool bInspection,const FTransform& Transform)
{
	if (bInspection)
	{
		//OnUserStateOnChange(CommonToolTags::UserState_AutoRoam, Transform, 0,true);
	}
	else
	{
		//StateSubsystem->ChangeSystemState(CommonToolTags::State_MainMenu);
	}
}

void ACommonCharacter::InspectionTransform(const FTransform& InTransform)
{
	if (StateSubsystem)
	{
		FGameplayTag SystemState;
		StateSubsystem->GetSystemState(SystemState);

		if (SystemState==CommonToolTags::State_Inspection)
		{
			SetActorLocationAndRotation(InTransform.GetLocation(), InTransform.GetRotation());
		}
	}
}

void ACommonCharacter::OnSceneChangeComplete()
{
	if (StateSubsystem && SceneManagerSubsystem)
	{
		if (SceneManagerSubsystem->GetCurrentSceneName().IsEmpty())
		{
			
		}
		else
		{
			StateSubsystem->ChangeSystemState(CommonToolTags::State_MainMenu);
		}
	}
}

void ACommonCharacter::UpdateViewChanging(const FTransform &CurrentTransform, float PreviousZoomSize,
                                           const FTransform &TargetTransform, float TargetZoom)
{
	if (GetController())
	{
		UpdateViewBlendAlpha += 0.001;
		//自定义插值
		float EasedAlpha;
		FVector CurrentLocation;
		FRotator CurrentRotator;
		float CurrentZoom;
		if (bUseOldView)
		{
			if (CurveFloat)
			{
				EasedAlpha = CurveFloat->GetFloatValue(UpdateViewBlendAlpha);
			}
			else
			{
				EasedAlpha = UpdateViewBlendAlpha * UpdateViewBlendAlpha * (3.0f - 2.0f * UpdateViewBlendAlpha);
			}
			CurrentLocation = FMath::Lerp(CurrentTransform.GetLocation(), TargetTransform.GetLocation(),UpdateViewBlendAlpha);
			CurrentRotator = FMath::Lerp(CurrentTransform.Rotator(), TargetTransform.Rotator(), UpdateViewBlendAlpha);
			CurrentZoom = FMath::Lerp(PreviousZoomSize, TargetZoom, UpdateViewBlendAlpha);
		}
		else
		{
			//float EasedAlpha = UKismetMathLibrary::Ease(UpdateViewBlendAlpha, 0.0f, 1.0f, EEasingFunc::EaseInOut, 2.0f /*BlendExp*/);
			if (CurveFloat)
			{
				EasedAlpha = CurveFloat->GetFloatValue(UpdateViewBlendAlpha);
			}
			else
			{
				EasedAlpha = UpdateViewBlendAlpha * UpdateViewBlendAlpha * (3.0f - 2.0f * UpdateViewBlendAlpha);
			}
			CurrentLocation = FMath::Lerp(CurrentTransform.GetLocation(), TargetTransform.GetLocation(),EasedAlpha);
			CurrentRotator = FMath::Lerp(CurrentTransform.Rotator(), TargetTransform.Rotator(), EasedAlpha);
			CurrentZoom = FMath::Lerp(PreviousZoomSize, TargetZoom, EasedAlpha);
			//迟滞
			FVector LagLocation = FMath::VInterpTo(LagLocation, CurrentLocation, 0.16,  LagInterpSpeed);
			CurrentLocation = LagLocation;
		}
		
		CurrentRotator.Roll = 0.0f;
		SetActorLocation(CurrentLocation);
		GetController()->SetControlRotation(CurrentRotator);
		CameraComponent->SetZoom(CurrentZoom);
	
		if (UpdateViewBlendAlpha >= 1)
		{
			GetWorld()->GetTimerManager().ClearTimer(ViewChangeTimerHandle);
			UpdateViewBlendAlpha = 1.0f;
			SetActorLocation(TargetTransform.GetLocation());
			GetController()->SetControlRotation(CurrentRotator);
			CameraComponent->SetZoom(TargetZoom);
			StateSubsystem->OnUserStateChangeCompletedSignature.Broadcast(TargetTransform, TargetZoom);
		
		}
	}
}
