// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "CommonToolTags.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "CommonTool/Camera/CameraMode.h"
#include "CommonTool/Camera/CommonCameraComponent.h"
#include "CommonTool/Library/CommDeveloperSettings.h"
#include "CommonTool/Library/PrintToolLibrary.h"
#include "CommonTool/SubSystem/InteractiveSubsystem.h"
#include "CommonTool/SubSystem/LoadingSubsystem.h"
#include "CommonTool/SubSystem/StateSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
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
	// SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// SpringArmComponent->SetupAttachment(RootComponent);
	// SpringArmComponent->TargetArmLength = 300; // The camera follows at this distance behind the character	
	// SpringArmComponent->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	CameraComponent = CreateDefaultSubobject<UCommonCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(0.0f));
	//CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//FlyMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FlyMovementComponent"));
}

// Called when the game starts or when spawned
void ACommonCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CameraComponent)
	{
		CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
	}

	StateSubsystem = GetGameInstance()->GetSubsystem<UStateSubsystem>();
	if (StateSubsystem)
	{
		StateSubsystem->UserStateOnChange.AddDynamic(this, &ThisClass::OnUserStateOnChange);
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);
		StateSubsystem->DeviceStateOnChange.AddDynamic(this, &ThisClass::OnDeviceStateOnChange);
	}

	LoadingSubsystem = GetGameInstance()->GetSubsystem<ULoadingSubsystem>();
	if (LoadingSubsystem)
	{
		//LoadingSubsystem->OnLoadScreenShowChange.AddDynamic(this, &ThisClass::OnLoadScreenShowChange);
	}

	InteractiveSubsystem = UWorld::GetSubsystem<UInteractiveSubsystem>(GWorld);
	
	InteractiveSubsystem->ReturnRootView();
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
	if (CurrentUserState == CommonToolTags::UserState_Focus)
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
			CameraComponent->ZoomIn();
		}
		if (ZoomFactor < 0.0f)
		{
			CameraComponent->ZoomAway();
		}
		GetCharacterMovement()->MaxFlySpeed = ForceSpeed*CameraComponent->GetZoomSize();
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
	if (!PlayerController->GetMousePosition(LeftMousePosition.X, LeftMousePosition.Y))
		return;
	if (!StateSubsystem || !InteractiveSubsystem)
	{
		return;
	}

	FGameplayTag SystemState;
	StateSubsystem->GetSystemState(SystemState);
	if (SystemState == CommonToolTags::State_Edit)
	{
		InteractiveSubsystem->PressPointer();
	}
	else if (SystemState == CommonToolTags::State_Roam)
	{
		InteractiveSubsystem->InteractiveRoam();
	}
	FGameplayTag DeviceState;
	StateSubsystem->GetDeviceState(SystemState);
	if (SystemState == CommonToolTags::DeviceState_Disassemble)
	{
		InteractiveSubsystem->InteractiveDisassembleDevice();
	}
}

void ACommonCharacter::LeftButtonCompleted(const FInputActionValue& Value)
{
	if (!StateSubsystem || !InteractiveSubsystem)
	{
		return;
	}
	
	FVector2D LastMousePosition = LeftMousePosition;
	if (!PlayerController->GetMousePosition(LeftMousePosition.X, LeftMousePosition.Y))
		return;
	
	FGameplayTag SystemState;
	StateSubsystem->GetSystemState(SystemState);
	if (SystemState == CommonToolTags::State_Inspection)
	{
		if (FMath::Abs(LastMousePosition-LeftMousePosition).Length()>1)
		{
			return;
		}
		InteractiveSubsystem->InteractiveInspection();
	}
	else if (SystemState == CommonToolTags::State_Edit)
	{
		if (InteractiveSubsystem->GetIsDragging())
		{
			InteractiveSubsystem->ReleasePointer();
		}
		else
		{
			if (FMath::Abs(LastMousePosition-LeftMousePosition).Length()>1)
			{
				return;
			}
			InteractiveSubsystem->Interactive();
		}
	}else if (SystemState == CommonToolTags::State_Simulate_Maintenance)
	{
		InteractiveSubsystem->InteractiveSimulate();
	}
}

void ACommonCharacter::RightButtonStarted(const FInputActionValue& Value)
{
	if (!PlayerController->GetMousePosition(RightMousePosition.X, RightMousePosition.Y))
		return;
}

void ACommonCharacter::RightButtonCompleted(const FInputActionValue& Value)
{
	if (!StateSubsystem || !InteractiveSubsystem)
	{
		return;
	}
	
	FVector2D LastMousePosition = RightMousePosition;
	if (!PlayerController->GetMousePosition(RightMousePosition.X, RightMousePosition.Y))
		return;
	if (FMath::Abs(LastMousePosition-RightMousePosition).Length()>1)
	{
		InteractiveSubsystem->DeInteractiveOption();
		InteractiveSubsystem->DeInteractive();
		return;
	}
	
	FGameplayTag SystemState;
	StateSubsystem->GetSystemState(SystemState);
	if (SystemState == CommonToolTags::State_Edit)
	{
		if (InteractiveSubsystem->GetUIIsSelected())
		{
			InteractiveSubsystem->DeInteractive();
		}
		else
		{
			InteractiveSubsystem->InteractiveOption();
		}
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

				if (newState == CommonToolTags::UserState_Focus)
				{
					Subsystem->AddMappingContext(FocusMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_Focus);
					CameraComponent->bUsePawnControlRotation = false;
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					GetCapsuleComponent()->SetEnableGravity(false);

					GetCharacterMovement()->SetMovementMode(MOVE_Flying);
					GetCharacterMovement()->MaxFlySpeed = ForceSpeed*Zoom;
				}

				if (newState == CommonToolTags::UserState_Panorama)
				{
					Subsystem->AddMappingContext(PanoramaMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_Panorama);
					CameraComponent->bUsePawnControlRotation = false;
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					GetCapsuleComponent()->SetEnableGravity(false);

					GetCharacterMovement()->SetMovementMode(MOVE_Flying);
					GetCharacterMovement()->MaxFlySpeed = PanoramaSpeed;
				}

				if (newState == CommonToolTags::UserState_FirstPerson)
				{
					Subsystem->AddMappingContext(FirstPersonMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_FirstPerson);
					CameraComponent->bUsePawnControlRotation = true;
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					GetCapsuleComponent()->SetEnableGravity(true);
					GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					GetCharacterMovement()->MaxWalkSpeed = FirstPersonSpeed;
				}

				if (newState == CommonToolTags::UserState_ThirdPerson)
				{
					Subsystem->AddMappingContext(ThirdPersonMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_ThirdPerson);
					CameraComponent->bUsePawnControlRotation = false;
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					GetCapsuleComponent()->SetEnableGravity(true);
					GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					GetCharacterMovement()->MaxWalkSpeed = ThirdPersonSpeed;
					
				}

				if (newState == CommonToolTags::UserState_Free)
				{
					Subsystem->AddMappingContext(FreeMappingContext, 0);
					CameraComponent->PushCameraMode(CameraMode_Free);
					CameraComponent->bUsePawnControlRotation = false;
					GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					GetCapsuleComponent()->SetEnableGravity(true);
					GetCharacterMovement()->SetMovementMode(MOVE_Flying);
					GetCharacterMovement()->MaxFlySpeed = FreeSpeed;
				}

				if (CurrentUserState == CommonToolTags::UserState_Null)
				{
					return;
				}
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
				GetWorld()->GetTimerManager().ClearTimer(ViewChangeTimerHandle);
				SetActorLocation(Transform.GetLocation());
				GetController()->SetControlRotation(Transform.Rotator());
				CameraComponent->SetZoom(Zoom);
			}
		}
	}
}

void ACommonCharacter::OnSystemStateOnChange(FGameplayTag newState)
{
	if (newState == CommonToolTags::State_Roam)
	{
		OnUserStateOnChange(CommonToolTags::UserState_FirstPerson, GetActorTransform(), 0,true);
	}
}

void ACommonCharacter::OnDeviceStateOnChange(FGameplayTag newState)
{
	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
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
				//新状态在拆解
				//记录一下拆解状态前的位置,用于取消拆解
				LastStateTransform = GetActorTransform();
				LastStateTransform.SetRotation(GetControlRotation().Quaternion());
				LastStateTransform.SetLocation(GetActorLocation());
				FTransform TargetTransform;
				TargetTransform = Settings->DeviceDisassembleTransform;
				TargetTransform.SetRotation(GetControlRotation().Quaternion());
				OnUserStateOnChange(CommonToolTags::UserState_Focus, TargetTransform, CameraComponent->GetZoomSize(),false);
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

void ACommonCharacter::UpdateViewChanging(const FTransform &CurrentTransform, float PreviousZoomSize,
                                           const FTransform &TargetTransform, float TargetZoom)
{
	UpdateViewBlendAlpha += 0.001;
	if (UpdateViewBlendAlpha >= 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(ViewChangeTimerHandle);
		UpdateViewBlendAlpha = 1.0f;
	}
	//自定义插值
	float EasedAlpha = UpdateViewBlendAlpha * UpdateViewBlendAlpha * (3.0f - 2.0f * UpdateViewBlendAlpha);
	//float EasedAlpha = UKismetMathLibrary::Ease(UpdateViewBlendAlpha, 0.0f, 1.0f, EEasingFunc::EaseInOut, 2.0f /*BlendExp*/);
	const FVector CurrentLocation = FMath::Lerp(CurrentTransform.GetLocation(), TargetTransform.GetLocation(),
	                                      EasedAlpha);
	const FRotator CurrentRotator = FMath::Lerp(CurrentTransform.Rotator(), TargetTransform.Rotator(), EasedAlpha);
	const float LocalCurrentZoom = FMath::Lerp(PreviousZoomSize, TargetZoom, EasedAlpha);
	SetActorLocation(CurrentLocation);
	GetController()->SetControlRotation(CurrentRotator);
	CameraComponent->SetZoom(LocalCurrentZoom);
}
