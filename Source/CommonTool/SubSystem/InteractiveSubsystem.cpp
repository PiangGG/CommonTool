// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveSubsystem.h"

#include "CommonToolTags.h"
#include "ConfigSubSystem.h"
#include "HttpSubSystem.h"
#include "CommonTool/Interface/DeviceInterface.h"
#include "CommonTool/Library/PrintToolLibrary.h"
#include "InputCoreTypes.h"
#include "LoadingSubsystem.h"
#include "MessageSubsystem.h"
#include "SaveSubsystem.h"
#include "SceneManagerSubsystem.h"
#include "StateSubsystem.h"
#include "RegionManager.h"
#include "CommonTool/Components/DeviceMarkComponent.h"
#include "CommonTool/Library/CommDeveloperSettings.h"
#include "CommonTool/Library/JsonToolLibrary.h"
#include "CommonTool/RuntimeGizemo/CommonGizmoInterface.h"
#include "CommonTool/Task/GameplayTask_Interactive.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UInteractiveSubsystem* UInteractiveSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject && GWorld)
	{
		return UWorld::GetSubsystem<UInteractiveSubsystem>(GWorld);
	}
	return nullptr;
}

bool UInteractiveSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UInteractiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("InteractiveSubsystem::Initialize"));
	
}

void UInteractiveSubsystem::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("InteractiveSubsystem::Deinitialize"));
	Super::Deinitialize();
	
}

void UInteractiveSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	StateSubsystem = InWorld.GetGameInstance()->GetSubsystem<UStateSubsystem>();
	if (StateSubsystem)
	{
		StateSubsystem->UserStateOnChange.AddDynamic(this, &ThisClass::OnUserStateOnChange);
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);
		StateSubsystem->DeviceStateOnChange.AddDynamic(this, &ThisClass::OnDeviceStateOnChange);
	}
	SceneManagerSubsystem = InWorld.GetSubsystem<USceneManagerSubsystem>();
	
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->OnSelectionActor.AddDynamic(this, &ThisClass::OnSelectionActor);
	}
	InteractiveOnClickedSignature.AddDynamic(this, &ThisClass::InteractiveOnClicked);
	InteractiveRoamSignature.AddDynamic(this, &ThisClass::InteractiveRoamOnClicked);
	InteractiveOptionSignature.AddDynamic(this, &ThisClass::InteractiveOnClickedOption);
	InteractiveEditSignature.AddDynamic(this, &ThisClass::InteractiveOnClickedEdit);
	OnUISectionChange.AddDynamic(this, &ThisClass::UISectionChange);
	TrainStationManager =  UWorld::GetSubsystem<UTrainStationManager>(GWorld);
	SaveSubsystem = InWorld.GetGameInstance()->GetSubsystem<USaveSubsystem>();

	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
	{
		InteractionSettings = Settings->InteractionSettings;
	}
}

void UInteractiveSubsystem::BeginDestroy()
{
	Super::BeginDestroy();
}

void UInteractiveSubsystem::OnUserStateOnChange(FGameplayTag newState, const FTransform& Transform, float Zoom,bool bblend)
{
	
}

void UInteractiveSubsystem::OnSystemStateOnChange(FGameplayTag newState)
{
	if (newState == CommonToolTags::State_Edit)
	{
		StartQueryInteractables();
	}
	else
	{
		EndQueryInteractables();
		if (SceneManagerSubsystem)
		{
			SceneManagerSubsystem->DeInteractive();
		}
	}
	CleanCurrentInteractiveActor();
}

void UInteractiveSubsystem::OnDeviceStateOnChange(FGameplayTag newState)
{
	if (newState != CommonToolTags::DeviceState_Disassemble)
	{
		GetDeviceDatabyID(CurrentForceDeviceID,newState);
	}
}

void UInteractiveSubsystem::CleanCurrentInteractiveActor()
{
	ForceDeviceSetMaterial(CurrentInteractiveActor,false);
	CurrentInteractiveActor = nullptr;
	CurrentForceDeviceID = "";
	ForceDeviceSignature.Broadcast(CurrentForceDeviceID);
	InteractiveRoamDevice.Broadcast(CurrentInteractiveActor);
}

void UInteractiveSubsystem::DeviceOnClick(AActor* Device, FKey ButtonPressed)
{
	UPrintToolLibrary::Debug(FString::Printf(TEXT("InteractiveSubsystem::DeviceOnClick:%s"), *Device->GetName()));
	InteractiveOnClickedSignature.Broadcast(Device);
}

AActor* UInteractiveSubsystem::LineTraceActor()
{
	// 获取鼠标位置
	float MouseX, MouseY;
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController == nullptr)
	{
		return nullptr;
	}
	if (PlayerController->GetMousePosition(MouseX, MouseY))
	{
		// 将屏幕位置转换为世界空间中的射线
		FVector WorldLocation, WorldDirection;
		if (PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
		{
			// 计算射线起点和终点
			FVector RayStart = WorldLocation;
			FVector RayEnd = RayStart + (WorldDirection * 1000000.0f);

			// 定义碰撞参数
			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;

			// 执行射线检测
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, RayStart, RayEnd, ECC_Visibility,
															 CollisionParams);
			if (bHit)
			{
				// 处理碰撞结果
				AActor* LocalHitActor = HitResult.GetActor();
				if (LocalHitActor)
				{
					return LocalHitActor;
				}

				// 绘制调试射线
				if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
				{
					if ( Settings->bDrawDebug)
					{
						DrawDebugLine(GetWorld(), RayStart, HitResult.Location, FColor::Red, false, 2.0f, 0, 1.0f);
					}
				}
			}
			else
			{
				// 绘制调试射线
				if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
				{
					if ( Settings->bDrawDebug)
					{
						DrawDebugLine(GetWorld(), RayStart, RayEnd, FColor::Green, false, 2.0f, 0, 1.0f);
					}
				}
				
			}
		}
	}
	return nullptr;
}

void UInteractiveSubsystem::InteractiveOnClicked(AActor* Actor)
{
	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
	{
		if (CurrentInteractiveActor == Actor)
		{
			CurrentInteractiveActor = nullptr;
			return;
		}
		else
		{
			CurrentInteractiveActor = Actor;
		}
		
		FVector Origin;
		FVector BoxExtent;
		CurrentInteractiveActor->GetActorBounds(false, Origin, BoxExtent);

		FTransform Transform;
		Transform.SetLocation(Origin);
		FRotator Rotator = UGameplayStatics::GetPlayerCharacter(CurrentInteractiveActor, 0)->GetActorRotation();
		Rotator.Pitch = -45.0f;
		Transform.SetRotation(Rotator.Quaternion());

		float Forcesize = 5.0f;
	
		if (Settings->bDrawDebug)
		{
			DrawDebugBox(GetWorld(), Origin, BoxExtent, FColor::Green, false, 5);
		}
		
		Forcesize = Settings->Forcesize;
		
		if (StateSubsystem)
		{
			StateSubsystem->ChangeUserState(CommonToolTags::UserState_Focus, Transform, BoxExtent.Length() * Forcesize);
		}
		//交互信息广播
		GetWorld()->GetTimerManager().ClearTimer(SetInfoTimer);
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("BroadCastInfo"), CurrentInteractiveActor);
		GetWorld()->GetTimerManager().SetTimer(SetInfoTimer, TimerDelegate, 0.01, false);
	}
}

void UInteractiveSubsystem::InteractiveRoamOnClicked(AActor* Actor)
{
	if (CurrentInteractiveActor == Actor)
	{
		CleanCurrentInteractiveActor();
	}
	else
	{
		ForceDeviceSetMaterial(CurrentInteractiveActor,false);
		
		CurrentInteractiveActor = Actor;
		ForceDeviceSetMaterial(CurrentInteractiveActor,true);

		if (UDeviceMarkComponent* DeviceMarkComponent= CurrentInteractiveActor->FindComponentByClass<UDeviceMarkComponent>())
		{
			CurrentForceDeviceID = DeviceMarkComponent->DeviceID;
			GetDeviceDatabyID(CurrentForceDeviceID,CommonToolTags::DeviceState_Ledger);
			InteractiveRoamDevice.Broadcast(CurrentInteractiveActor);
		}
	}
}

void UInteractiveSubsystem::InteractiveSimulate()
{
	if (AActor* ResultActor = LineTraceActor())
	{
		if (ResultActor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
		{
			IDeviceInterface::Execute_InteractiveDevice(ResultActor);
		}
	}
}

void UInteractiveSubsystem::InteractiveOnClickedOption(AActor* Actor)
{
	EditActor = Actor;
	
}

void UInteractiveSubsystem::InteractiveOnClickedEdit(AActor* Actor)
{
	
}

void UInteractiveSubsystem::BroadCastInfo(AActor* actor)
{
	InteractiveInfo.Broadcast(actor->GetName());
}

void UInteractiveSubsystem::GetPointerRay(FVector& RayStart, FVector& RayEnd)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		FVector WorldLocation = FVector::ZeroVector;
		FVector WorldDirection = FVector::ZeroVector;
		if (InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_World)
		{
			RayStart = UGameplayStatics::GetPlayerCharacter(GWorld, 0)->GetActorLocation();
			RayEnd = RayStart + UGameplayStatics::GetPlayerCharacter(GWorld, 0)->
				GetActorForwardVector() * InteractionSettings.InteractionDistance;
		}
		else if (InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_MouseCursor)
		{
			PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
			RayStart = WorldLocation;
			RayEnd = WorldLocation + (WorldDirection * InteractionSettings.InteractionDistance);
		}
		else if (InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_ScreenCenter)
		{
			int32 X = 0;
			int32 Y = 0;
			PlayerController->GetViewportSize(X, Y);
			PlayerController->DeprojectScreenPositionToWorld(X, Y, WorldLocation, WorldDirection);
			RayStart = WorldLocation;
			RayEnd = WorldLocation + (WorldDirection * InteractionSettings.InteractionDistance);
		}
		else
		{
		}
	}
}

void UInteractiveSubsystem::Interactive()
{
	FVector RayStart = FVector::ZeroVector;
	FVector	RayEnd= FVector::ZeroVector;
	GetPointerRay(RayStart,RayEnd);

	// 定义碰撞参数
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	
	// 执行射线检测
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, RayStart, RayEnd, ECC_Visibility,CollisionParams);
	
	if (bHit)
	{
		if (bUIIsSelected)
		{
			SceneManagerSubsystem->InteractiveHit(HitResult);
		}
		else
		{
			if (SelectedActor == HitResult.GetActor())
			{
				SelectedActor = nullptr;
				OnActivateGizmo.Broadcast(nullptr);
			}
			else
			{
				SetInteractiveActor(HitResult.GetActor());
			}
		}
	}
}

void UInteractiveSubsystem::DeInteractive()
{
	if (bUIIsSelected)
	{
		if (SceneManagerSubsystem)
		{
			SceneManagerSubsystem->DeInteractive();
		}
	}
	if (SelectedActor)
	{
		SelectedActor = nullptr;
		OnActivateGizmo.Broadcast(nullptr);
	}
}

void UInteractiveSubsystem::InteractiveRoam()
{
	if (AActor* ResultActor = LineTraceActor())
	{
		if (UDeviceMarkComponent* DeviceMarkComponent= ResultActor->FindComponentByClass<UDeviceMarkComponent>())
		{
			InteractiveRoamSignature.Broadcast(ResultActor);
		}
	}
}

void UInteractiveSubsystem::InteractiveDisassembleDevice()
{
	// 获取鼠标位置
	float MouseX, MouseY;
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	
	if (PlayerController->GetMousePosition(MouseX, MouseY))
	{
		// 将屏幕位置转换为世界空间中的射线
		FVector WorldLocation, WorldDirection;
		if (PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
		{
			// 计算射线起点和终点
			FVector RayStart = WorldLocation;
			FVector RayEnd = RayStart + (WorldDirection * 1000000.0f);

			// 定义碰撞参数
			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;

			// 执行射线检测
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, RayStart, RayEnd, ECC_Visibility,
															 CollisionParams);
			if (bHit)
			{
				InteractiveDisassembleDeviceSignature.Broadcast(HitResult);

				// 绘制调试射线
				if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
				{
					if ( Settings->bDrawDebug)
					{
						DrawDebugLine(GetWorld(), RayStart, HitResult.Location, FColor::Red, false, 2.0f, 0, 1.0f);
					}
				}
			}
			else
			{
				// 绘制调试射线
				if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
				{
					if ( Settings->bDrawDebug)
					{
						DrawDebugLine(GetWorld(), RayStart, RayEnd, FColor::Green, false, 2.0f, 0, 1.0f);
					}
				}
				
			}
		}
	}
}

void UInteractiveSubsystem::SetInteractiveActor(AActor* Actor)
{
	SelectedActor = Actor;
	if (SelectedActor)
	{
		OnUISectionChange.Broadcast(FEditItem(),false);
		OnActivateGizmo.Broadcast(SelectedActor);
	}
}

void UInteractiveSubsystem::InteractiveInspection()
{
	if (AActor* ResultActor = LineTraceActor())
	{
		if (ResultActor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
		{
			if (IDeviceInterface::Execute_GetbIsCanReView(ResultActor))
			{
				InteractiveOnClickedSignature.Broadcast(ResultActor);
			}
		}
		else if (ResultActor->ActorHasTag(FName("Device")))
		{
			InteractiveOnClickedSignature.Broadcast(ResultActor);
		}
	}
}

void UInteractiveSubsystem::InteractiveOption(AActor* Actor)
{
	if (Actor)
	{
		InteractiveOptionSignature.Broadcast(Actor);
		return;
	}
	if (AActor* ResultActor = LineTraceActor())
	{
		//初始只能编辑设备
		// if (ResultActor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
		// {
		// 	InteractiveOptionSignature.Broadcast(ResultActor);
		// }
		//ToDo 编辑场景actor可交互属性
		InteractiveOptionSignature.Broadcast(ResultActor);
	}
}

void UInteractiveSubsystem::DeInteractiveOption()const
{
	InteractiveOptionSignature.Broadcast(nullptr);
}

void UInteractiveSubsystem::PressPointer()
{
	if (bUIIsSelected)
	{
		
	}
	else
	{
		UPrintToolLibrary::Debug(FString::Printf(TEXT("PressPointer")));
	
		if (HitActor&&HitComponent)
		{
			if (HitActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
			{
				bIsDragging = ICommonGizmoInterface::Execute_OnPointerDown(HitActor,HitComponent);
			}
		}
	}
}

void UInteractiveSubsystem::ReleasePointer()
{
	if (bUIIsSelected)
	{
		
	}
	else
	{
		UPrintToolLibrary::Debug(FString::Printf(TEXT("ReleasePointer")));

		if (HitActor&&HitComponent)
		{
			if (HitActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
			{
				ICommonGizmoInterface::Execute_OnPointerUp(HitActor,HitComponent);
				bIsDragging = false;
				if (SaveSubsystem && SelectedActor)
				{
					FString GUID;
					if (SceneManagerSubsystem->GetActorGUID(SelectedActor,GUID))
					{
						SaveSubsystem->OnSceneSaveActor.Broadcast(GUID,TArray<FString>());
					}
				}
			}
		}
	}
}

void UInteractiveSubsystem::SetLockHover(bool LockHover)
{
	bIsHoverLocked = LockHover;
}

void UInteractiveSubsystem::OnSelectionActor(bool bSelection, AActor* SelectionActor)
{
	if (!bSelection||SelectionActor==nullptr)
	{
		DeInteractive();
	}
	else
	{
		SetInteractiveActor(SelectionActor);
	}
}

void UInteractiveSubsystem::DisableInteraction()
{
	HitActor = nullptr;
	HitComponent = nullptr;
}

void UInteractiveSubsystem::StartQueryInteractables()
{
	if (UWorld* World = GetWorld())
	{
		
		InteractionSettings.InteractableTypes.AddUnique(EObjectTypeQuery::ObjectTypeQuery1);
		InteractionSettings.InteractableTypes.AddUnique(EObjectTypeQuery::ObjectTypeQuery2);
		InteractionSettings.bUseMultiRayHit = true;
		InteractionSettings.InteractionSource= CommonToolTags::PointerInteractionSource_MouseCursor;

		if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
		{
			if (Settings->bDrawDebug)
			{
				InteractionScanRate = Settings->InteractionScanRate;
			}
		}
		World->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
	}
}

void UInteractiveSubsystem::EndQueryInteractables()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}
}

void UInteractiveSubsystem::QueryInteractables()
{
	if (bIsDragging)
	{
		if (InteractionSettings.bAutoStartDrag)
		{
			DragPointer();
		}
	}
	else
	{
		UpdatePointerHover();
	}
}

void UInteractiveSubsystem::DragPointer()
{
	if (HitActor&&HitComponent)
	{
		if (HitActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
		{
			ICommonGizmoInterface::Execute_OnPointerDrag(HitActor,HitComponent);
		}
	}
}

void UInteractiveSubsystem::UpdatePointerHover()
{
	AActor * TestActor = nullptr;
	UPrimitiveComponent * TestComponent = nullptr;
	if (bIsHoverLocked)
	{
		if (HitActor&&HitComponent)
		{
			if (HitActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
			{
				ICommonGizmoInterface::Execute_OnPointerMove(HitActor,HitComponent);
			}
		}
	}
	else
	{
		if (InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_MouseCursor||
			InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_World||
			InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_ScreenCenter)
		{
			if (RaycastTest(SavedHitResult))
			{
				TestActor = SavedHitResult.GetActor();
				TestComponent = SavedHitResult.GetComponent();
			}
			else
			{
				TestActor = nullptr;
				TestComponent = nullptr;
				if (HitActor&&HitComponent)
				{
					if (HitActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
					{
						ICommonGizmoInterface::Execute_OnPointerLeave(HitActor,HitComponent);
						HitActor = TestActor;
						HitComponent = TestComponent;
					}
				}
				return;
			}
		}
		else if (InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_Custom)
		{
			TestActor = SavedHitResult.GetActor();
			TestComponent = SavedHitResult.GetComponent();
		}
		else if (InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_Overlap)
		{
			OverlapTest(TestActor,TestComponent);
		}
		else
		{
			return;
		}

		if (!TestActor)
		{
			if (HitActor&&HitComponent)
			{
				if (HitActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
				{
					ICommonGizmoInterface::Execute_OnPointerLeave(HitActor,HitComponent);
					HitActor = TestActor;
					HitComponent = TestComponent;
				}
			}
			return;
		}
		
		if (TestActor == HitActor)
		{
			if (TestComponent == HitComponent)
			{
				if (HitActor&&HitComponent)
				{
					if (HitActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
					{
						ICommonGizmoInterface::Execute_OnPointerMove(HitActor,HitComponent);
						return;
					}
				}
			}
			else
			{
				if (HitActor && HitComponent)
				{
					if (HitActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
					{
						ICommonGizmoInterface::Execute_OnPointerLeave(HitActor,HitComponent);
					}
				}
				if (HitActor && TestComponent)
				{
					if (HitActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
					{
						ICommonGizmoInterface::Execute_OnPointerEnter(HitActor,TestComponent);
					}
				}
				HitComponent = TestComponent;
			}
		}
		else
		{
			if (HitActor&&HitComponent)
			{
				if (HitActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
				{
					ICommonGizmoInterface::Execute_OnPointerLeave(HitActor,HitComponent);
				}
			}
			if (TestActor&&TestComponent)
			{
				if (TestActor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
				{
					ICommonGizmoInterface::Execute_OnPointerEnter(TestActor,TestComponent);
				}
			}
			
			HitActor = TestActor;
			HitComponent = TestComponent;
		}
	}
	
	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
	{
		if (Settings->bDrawDebug)
		{
			if (HitActor&&HitComponent)
			{
				UPrintToolLibrary::Debug(FString::Printf(TEXT("UpdatePointerHover Acotr::%s,UpdatePointerHover Component::%s"), *HitActor->GetName(),*HitComponent->GetName()));
			}
		}
	}
}

bool UInteractiveSubsystem::IsActorInteractable(AActor* Actor)
{
	if (Actor->GetClass()->ImplementsInterface(UCommonGizmoInterface::StaticClass()))
	{
		if (InteractionSettings.IncludeActors.IsEmpty())
		{
			return true;
		}
		else
		{
			return InteractionSettings.IncludeActors.Contains(Actor);
		}
	}
	else
	{
		return false;
	}
}

bool UInteractiveSubsystem::RaycastTest(FHitResult& HitResult)
{
	int32 HighestPriority = -999999;
	float MinDistance  = 9999999.0f;
	FHitResult NearestHit;
	FVector RayStart; FVector RayEnd;
	GetPointerRay(RayStart,RayEnd);
	TArray<AActor*> ActorsToIgnore;
	EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None;
	bool bHit = false;
	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
	{
		DrawDebugType = Settings->bDrawDebug?EDrawDebugTrace::ForDuration:EDrawDebugTrace::None;
	}
	
	bool bIgnoreSelf = true;
	
	if (InteractionSettings.bUseMultiRayHit)
	{
		TArray<FHitResult> OutHits;
		
		bHit = UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(),RayStart,RayEnd,InteractionSettings.InteractableTypes,false,ActorsToIgnore,DrawDebugType,OutHits,bIgnoreSelf);

		for (auto OutHit : OutHits)
		{
			if (IsActorInteractable(OutHit.GetActor()))
			{
				if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
				{
					if (Settings->bDrawDebug)
					{
						UPrintToolLibrary::Debug(FString::Printf(TEXT("UpdatePointerHover Component:%s"), *OutHit.GetComponent()->GetName()));
					}
				}
				
				if (ICommonGizmoInterface::Execute_GetHitPriority(OutHit.GetActor(),OutHit.GetComponent())>HighestPriority)
				{
					HighestPriority = ICommonGizmoInterface::Execute_GetHitPriority(OutHit.GetActor(),OutHit.GetComponent());
					MinDistance = OutHit.Distance;
					NearestHit = OutHit;
				}
				else
				{
					if (ICommonGizmoInterface::Execute_GetHitPriority(OutHit.GetActor(),OutHit.GetComponent()) == HighestPriority)
					{
						if (OutHit.Distance<MinDistance)
						{
							MinDistance = OutHit.Distance;
							NearestHit = OutHit;
							return true;
						}
						
					}
				}
			}
		}
	}
	else
	{
		bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),RayStart,RayEnd,InteractionSettings.InteractableTypes,false,ActorsToIgnore,DrawDebugType,NearestHit,bIgnoreSelf);

	}
	HitResult = NearestHit;
	return bHit;
}

void UInteractiveSubsystem::OverlapTest(AActor* OutActor, UPrimitiveComponent* OutComponent)
{
	int32 HighestPriority = -999999;
	float MinDistance  = 9999.0f;
	float Distance  = 0;
	UPrimitiveComponent*NearestComponent = nullptr;
	if (InteractionSettings.OverlapTestComponent)
	{
		TArray<UPrimitiveComponent*> PrimitiveComponents;
		UKismetSystemLibrary::ComponentOverlapComponents(InteractionSettings.OverlapTestComponent,InteractionSettings.OverlapTestComponent->GetComponentTransform(),InteractionSettings.InteractableTypes,AActor::StaticClass(),InteractionSettings.ExcludeActors,PrimitiveComponents);

		for (auto PrimitiveComponent : PrimitiveComponents)
		{
			if (AActor* ComponentOwner= PrimitiveComponent->GetOwner())
			{
				if (IsActorInteractable(ComponentOwner))
				{
					FVector ComponentWorldLocation = InteractionSettings.OverlapTestComponent->GetComponentLocation();
					FVector OutPointOnBody;
					PrimitiveComponent->GetClosestPointOnCollision(ComponentWorldLocation,OutPointOnBody);
					if (ICommonGizmoInterface::Execute_GetHitPriority(ComponentOwner,PrimitiveComponent)>HighestPriority)
					{
						HighestPriority = ICommonGizmoInterface::Execute_GetHitPriority(ComponentOwner,PrimitiveComponent);
						
						Distance = UKismetMathLibrary::Vector_Distance(ComponentWorldLocation,OutPointOnBody);
						MinDistance = Distance;
						NearestComponent = PrimitiveComponent;
					}
					else
					{
						if (ICommonGizmoInterface::Execute_GetHitPriority(ComponentOwner,PrimitiveComponent) == HighestPriority)
						{
							Distance = UKismetMathLibrary::Vector_Distance(ComponentWorldLocation,OutPointOnBody);
							if (Distance<MinDistance)
							{
								MinDistance = Distance;
								NearestComponent = PrimitiveComponent;
							}
						}
					}
					
				}
			}
			
		}
		OutActor = NearestComponent->GetOwner();
		OutComponent = NearestComponent;
		return;
	}
	else
	{
		
	}
	
}

bool UInteractiveSubsystem::GetIsDragging() const
{
	return bIsDragging;
}

void UInteractiveSubsystem::SetTransformMode(FGameplayTag InTransformMode, bool CombineTranslateRotate)
{
	OnTransformModeChange.Broadcast(InTransformMode,CombineTranslateRotate);
}

void UInteractiveSubsystem::SetAlignSpace(FGameplayTag AlignSpace)
{
	OnAlignSpaceChange.Broadcast(AlignSpace);
}

void UInteractiveSubsystem::SetCurrentSectionItem(FEditItem editItem, bool bSelected)
{
	OnUISectionChange.Broadcast(editItem,bSelected);
}

void UInteractiveSubsystem::UISectionChange(const FEditItem& Item, bool bSelected)
{
	EditItem = Item;
	bUIIsSelected = bSelected;
}

AActor* UInteractiveSubsystem::GetSelectedActor() const
{
	return SelectedActor;
}

bool UInteractiveSubsystem::GetUIIsSelected() const
{
	return bUIIsSelected;
}

void UInteractiveSubsystem::EditForceActor(AActor* actor)
{
	if (actor)
	{
		EditActor = actor;
	}
	
	ForceActor(EditActor);
}

void UInteractiveSubsystem::EditEditActor(AActor * actor)
{
	if (actor)
	{
		EditActor = actor;
	}
	
	if (EditActor)
	{
		InteractiveEditSignature.Broadcast(EditActor);
	}
}

void UInteractiveSubsystem::DeleteEditActor()
{
	if (EditActor)
	{
		if (SceneManagerSubsystem)
		{
			FString GUID;
			SceneManagerSubsystem->GetActorGUID(EditActor,GUID);
			
			if (SceneManagerSubsystem->GetActorStatic("",GUID))
			{
				
			}
			else
			{
				EditActor->Destroy();
			}
			InteractiveOptionSignature.Broadcast(nullptr);
			if (SaveSubsystem)
			{
				SaveSubsystem->OnSceneSaveActor.Broadcast(GUID,TArray<FString>());
			}
		}
	}
}

AActor* UInteractiveSubsystem::GetEditActor()
{
	return EditActor;
}

void UInteractiveSubsystem::ForceDeviceSetMaterial(AActor* device,bool SetOrCancel)
{
	if (device)
	{
		if (SetOrCancel)
		{
			InteractiveRoamDevice.Broadcast(CurrentInteractiveActor);
			if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
			{
				TArray<UActorComponent*> ActorComponents = device->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
				for (auto Component : ActorComponents)
				{
					Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(Settings->OverlayMaterial.LoadSynchronous());
				}
			}
		}
		else
		{
			InteractiveRoamDevice.Broadcast(nullptr);
			TArray<UActorComponent*> ActorComponents = device->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
			for (auto Component : ActorComponents)
			{
				Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(nullptr);
			}
		}
	}
}

void UInteractiveSubsystem::ForceDeviceByID(const FString& Deviceid)
{
	if (!Deviceid.IsEmpty())
	{
		if (IsCurrentForceDevice(Deviceid))
		{
			
		}
		else
		{
			CurrentForceDeviceID = Deviceid;
		}
		ForceDeviceSignature.Broadcast(CurrentForceDeviceID);
	}
}

void UInteractiveSubsystem::GetForceDeviceID(FString& DeviceID)
{
	DeviceID = CurrentForceDeviceID;
}

bool UInteractiveSubsystem::IsCurrentForceDevice(const FString& DeviceID)
{
	return DeviceID.Equals(CurrentForceDeviceID);
}

void UInteractiveSubsystem::GetDeviceDatabyID(const FString& DeviceID,const FGameplayTag& newState)
{
	if (UHttpSubSystem* HttpSubSystem = UHttpSubSystem::Get(this))
	{
		if (UConfigSubSystem *ConfigSubSystem = UConfigSubSystem::Get(this))
		{
			FString DeviceData;
			if (newState==CommonToolTags::DeviceState_Ledger)
			{
				UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"DeviceData",DeviceData);
			}
			else if (newState==CommonToolTags::DeviceState_OperatingData)
			{
				UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"DeviceLog",DeviceData);
			}else if (newState==CommonToolTags::DeviceState_History)
			{
				UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"DeviceMeas",DeviceData);
			}else if (newState==CommonToolTags::DeviceState_Document)
			{
				UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"DeviceFile",DeviceData);
			}
			if (!DeviceData.IsEmpty())
			{
				FString Data = FString::Printf(TEXT("astId=%s"), *DeviceID);
				FHttpSingleCallBack SingleCallBack;
				SingleCallBack.BindDynamic(this,&ThisClass::OnGetDeviceDataResult);
				HttpSubSystem->HttpGetCallBack(DeviceData,Data,SingleCallBack);
			}
		}
	}
}

void UInteractiveSubsystem::ForceActor( AActor* Actor)
{
	if (Actor)
	{
		CurrentInteractiveActor = Actor;
		FVector Origin;
		FVector BoxExtent;
			
		Actor->GetActorBounds(false, Origin, BoxExtent);
			
		FTransform Transform;
		Transform.SetLocation(Origin);
		FRotator Rotator = UGameplayStatics::GetPlayerCharacter(Actor, 0)->GetActorRotation();
		Rotator.Pitch = -45.0f;
		Transform.SetRotation(Rotator.Quaternion());

		if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
		{
			if (StateSubsystem)
			{
				StateSubsystem->ChangeUserState(CommonToolTags::UserState_Focus, Transform, BoxExtent.Length() * Settings->Forcesize);
			}
			
			if (Settings->bDrawDebug)
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

void UInteractiveSubsystem::ReturnRootView()
{
	if (StateSubsystem)
	{
		StateSubsystem->ChangeSystemState(CommonToolTags::State_Null);
	}
}

void UInteractiveSubsystem::ReturnCurrentSceneView(bool breturnMain)
{
	if (TrainStationManager)
	{
		TrainStationManager->ReSetCurrentTrainStationView(breturnMain);
	}
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->ForceDevice("");
	}
}

void UInteractiveSubsystem::OnGetDeviceDataResult(const FString& JsonStr)
{
	GetDeviceDataResultSignature.Broadcast(JsonStr);
}
