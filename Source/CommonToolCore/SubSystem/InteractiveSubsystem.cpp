// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveSubsystem.h"

#include "AttributeToolLibrary.h"
#include "CesiumGeoJsonDocument.h"
#include "CommonToolCoreTags.h"
#include "ConfigSubSystem.h"
#include "DeviceManager.h"
#include "HttpSubSystem.h"
#include "DeviceInterface.h"
#include "PrintToolLibrary.h"
#include "InputCoreTypes.h"
#include "SaveSubsystem.h"
#include "SceneManagerSubsystem.h"
#include "StateSubsystem.h"
#include "DeviceMarkComponent.h"
#include "IInteractive.h"
#include "JsonToolLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
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

	InteractiveActorSignature.AddDynamic(this, &ThisClass::OnInteractiveActor);
	InteractiveActorRoamSignature.AddDynamic(this, &ThisClass::OnInteractiveRoamActor);
	OnChangeSectionItem.AddDynamic(this, &ThisClass::OnSectionChange);
	
	StateSubsystem = InWorld.GetGameInstance()->GetSubsystem<UStateSubsystem>();
	if (StateSubsystem)
	{
		StateSubsystem->UserStateOnChange.AddDynamic(this, &ThisClass::OnUserStateOnChange);
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);
		StateSubsystem->DeviceStateOnChange.AddDynamic(this, &ThisClass::OnDeviceStateOnChange);
	}
	SceneManagerSubsystem = InWorld.GetSubsystem<USceneManagerSubsystem>();
	
	SaveSubsystem = InWorld.GetGameInstance()->GetSubsystem<USaveSubsystem>();
	
	DeviceManager = UWorld::GetSubsystem<UDeviceManager>(GWorld);;

	if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
	{
		InteractionSettings = CommonSettings->InteractionSettings;
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
	}
	ClearInteractiveActor();
}

void UInteractiveSubsystem::OnDeviceStateOnChange(FGameplayTag newState,const FString& ID)
{
	GetDeviceDatabyID(ID,newState);
}

AActor* UInteractiveSubsystem::LineTraceActor(FHitResult& HitResult)
{
	FCesiumGeoJsonDocument CesiumGeoJsonDocument;
	
	FVector RayStart = FVector::ZeroVector;
	FVector	RayEnd= FVector::ZeroVector;
	GetPointerRay(RayStart,RayEnd);
	
	FCollisionQueryParams CollisionParams;

	// 执行射线检测
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, RayStart, RayEnd, ECC_Visibility,
													 CollisionParams);
	if (bHit)
	{
		// 绘制调试射线
		if (UPrintToolLibrary::DrawDebug())
		{
			DrawDebugLine(GetWorld(), RayStart,  HitResult.Location, FColor::Red, false, 2.0f, 0, 1.0f);
		}
		// 处理碰撞结果
		AActor* LocalHitActor = HitResult.GetActor();
		if (LocalHitActor)
		{
			return LocalHitActor;
		}
	}
	else
	{
		// 绘制调试射线
		if (UPrintToolLibrary::DrawDebug())
		{
			DrawDebugLine(GetWorld(), RayStart, RayEnd, FColor::Green, false, 2.0f, 0, 1.0f);
		}
	}
	return nullptr;
}

AActor* UInteractiveSubsystem::GetInteractiveActor()
{
	return InteractiveActor;
}

void UInteractiveSubsystem::OnInteractiveActor(AActor* Actor)
{
	if (Actor)
	{
		SetDeviceMaterial(Actor);	
		InteractiveActor = Actor;
	}
	else
	{
		if (InteractiveActor)
		{
			if (InteractiveActor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
			{
				IDeviceInterface::ActorInitializationCompleted(InteractiveActor);	
			}
		}
		SetDeviceMaterial(nullptr);
		InteractiveActor = nullptr;
	}
}

void UInteractiveSubsystem::OnInteractiveRoamActor(const FHitResult& HitResult)
{
	if (HitResult.GetActor())
	{
		SetDeviceMaterial(HitResult.GetActor());	
		InteractiveActor = HitResult.GetActor();
	}
	else
	{
		if (InteractiveActor)
		{
			if (InteractiveActor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
			{
				IDeviceInterface::ActorInitializationCompleted(InteractiveActor);	
			}
		}
		SetDeviceMaterial(nullptr);
		InteractiveActor = nullptr;
	}
}

void UInteractiveSubsystem::InteractiveRoamOnClicked(AActor* Actor,const FVector& HitLocation)
{
	// if (CurrentInteractiveActor == Actor)
	// {
	// 	CleanInteractiveActor();
	// }
	// else
	// {
	// 	ForceDeviceSetMaterial(CurrentInteractiveActor,false);
	// 	
	// 	CurrentInteractiveActor = Actor;
	// 	ForceDeviceSetMaterial(CurrentInteractiveActor,true);
	//
	// 	if (UDeviceMarkComponent* DeviceMarkComponent= CurrentInteractiveActor->FindComponentByClass<UDeviceMarkComponent>())
	// 	{
	// 		CurrentForceDeviceID = DeviceMarkComponent->DeviceID;
	// 		GetDeviceDatabyID(CurrentForceDeviceID,CommonToolTags::DeviceState_Ledger);
	//
	// 		if (!DeviceMarkComponent->DeviceID.IsEmpty())
	// 		{
	// 			InteractiveRoamDevice.Broadcast(CurrentInteractiveActor,HitLocation);
	// 		}
	// 	}
	// }
}

void UInteractiveSubsystem::InteractiveSimulate()
{
	FHitResult HitHitResult;
	if (AActor* ResultActor = LineTraceActor(HitHitResult))
	{
		if (ResultActor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
		{
			IDeviceInterface::Execute_InteractiveDevice(ResultActor);
		}
	}
}

void UInteractiveSubsystem::InteractiveOnClickedOption(AActor* Actor)
{
	//EditActor = Actor;
	
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

void UInteractiveSubsystem::SetInteractiveActor()
{
	FHitResult HitResult;
	if (AActor* ResultActor = LineTraceActor(HitResult))
	{
		if (ResultActor == InteractiveActor)
		{
			ClearInteractiveActor();
		}
		else
		{
			ClearInteractiveActor();
			InteractiveActorSignature.Broadcast(ResultActor);
			return;
		}
	}
}

void UInteractiveSubsystem::ClearInteractiveActor()
{
	InteractiveActorSignature.Broadcast(nullptr);
	InteractiveActorRoamSignature.Broadcast(FHitResult());
}

void UInteractiveSubsystem::InteractiveRoam()
{
	ClearInteractiveActor();
	FHitResult HitResult; 
	if (AActor* ResultActor = LineTraceActor(HitResult))
	{
		if (UDeviceMarkComponent* DeviceMarkComponent= ResultActor->FindComponentByClass<UDeviceMarkComponent>())
		{
			if (DeviceMarkComponent->DeviceID.IsEmpty())
			{
				
			}
			else
			{
				InteractiveActorRoamSignature.Broadcast(HitResult);
			}
		}
	}
}

void UInteractiveSubsystem::InteractiveDevice()
{
	FGameplayTag DeviceState;
	StateSubsystem->GetDeviceState(DeviceState);
	FGameplayTag SystemState;
	StateSubsystem->GetSystemState(SystemState);
	if (DeviceState != CommonToolTags::DeviceState_Disassemble)
	{
		ClearInteractiveActor();
		FHitResult HitResult; 
		if (AActor* ResultActor = LineTraceActor(HitResult))
		{
			if (UDeviceMarkComponent* DeviceMarkComponent= ResultActor->FindComponentByClass<UDeviceMarkComponent>())
			{
				if (DeviceMarkComponent->DeviceID.IsEmpty())
				{
					
				}
				else
				{
					if (SystemState == CommonToolTags::State_DeviceManager)
					{
						
					}
					else
					{
						StateSubsystem->ChangeSystemState(CommonToolTags::State_DeviceManager);
					}
					DeviceManager->SetFocusDevice(DeviceMarkComponent->DeviceID);
				}
			}
		}	
	}
}

void UInteractiveSubsystem::InteractiveInspection()
{
	// FVector HitLocation;
	// if (AActor* ResultActor = LineTraceActor(HitLocation))
	// {
	// 	if (ResultActor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
	// 	{
	// 		if (IDeviceInterface::Execute_GetbIsCanReView(ResultActor))
	// 		{
	// 			InteractiveOnClickedSignature.Broadcast(ResultActor);
	// 		}
	// 	}
	// 	else if (ResultActor->ActorHasTag(FName("Device")))
	// 	{
	// 		InteractiveOnClickedSignature.Broadcast(ResultActor);
	// 	}
	// }
}

void UInteractiveSubsystem::InteractiveOption(AActor* Actor)
{
	if (Actor)
	{
		InteractiveOptionSignature.Broadcast(Actor);
	}
	else
	{
		InteractiveOptionSignature.Broadcast(nullptr);
	}
}

void UInteractiveSubsystem::OnPressLeftPointer()
{
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PlayerController->GetMousePosition(LeftMousePosition.X, LeftMousePosition.Y))
		return;
	FGameplayTag SystemState;
	StateSubsystem->GetSystemState(SystemState);
	if (SystemState == CommonToolTags::State_Edit)
	{
		if (bSelectedItem)
		{
		
		}
		else
		{
			UPrintToolLibrary::Debug(FString::Printf(TEXT("PressPointer")));
	
			if (HitComponent)
			{
				bIsDragging = true;
				FVector RayStart; FVector RayEnd;
				GetPointerRay(RayStart,RayEnd);
				OnPointerDown.Broadcast(HitComponent,RayStart,RayEnd);
			}
		}
	}
}

void UInteractiveSubsystem::OnReleaseLeftPointer()
{
	FVector2D LastMousePosition = LeftMousePosition;
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
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
		InteractiveInspection();
	}
	else if (SystemState == CommonToolTags::State_Edit)
	{
		if (GetIsDragging())
		{
			if (bSelectedItem)
			{
		
			}
			else
			{
				UPrintToolLibrary::Debug(FString::Printf(TEXT("ReleasePointer")));

				if (FMath::Abs(LastMousePosition-LeftMousePosition).Length()>1)
				{
					if (HitComponent)
					{
						OnPointerUp.Broadcast(HitComponent);
						bIsDragging = false;
						if (InteractiveActor && SceneManagerSubsystem)
						{
							SceneManagerSubsystem->SaveActor(InteractiveActor);
						}
					}
				}
				else
				{
					SetInteractiveActor();
				}
				
			}
		}
		else
		{
			if (FMath::Abs(LastMousePosition-LeftMousePosition).Length()>1)
			{
				return;
			}
			SetInteractiveActor();
		}
	}else if (SystemState == CommonToolTags::State_Simulate_Maintenance)
	{
		if (FMath::Abs(LastMousePosition-LeftMousePosition).Length()>1)
		{
			return;
		}
		InteractiveSimulate();
	}else if (SystemState == CommonToolTags::State_Roam)
	{
		if (FMath::Abs(LastMousePosition-LeftMousePosition).Length()>1)
		{
			return;
		}
		InteractiveRoam();
	}else if (SystemState == CommonToolTags::State_DeviceManager || SystemState == CommonToolTags::State_MainMenu)
	{
		if (FMath::Abs(LastMousePosition-LeftMousePosition).Length()>1)
		{
			return;
		}
		InteractiveDevice();
	}
}

void UInteractiveSubsystem::OnPressRightPointer()
{
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PlayerController->GetMousePosition(RightMousePosition.X, RightMousePosition.Y))
		return;
}

void UInteractiveSubsystem::OnReleaseRightPointer()
{
	FVector2D LastMousePosition = RightMousePosition;
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PlayerController->GetMousePosition(RightMousePosition.X, RightMousePosition.Y))
		return;
	FGameplayTag SystemState;
	StateSubsystem->GetSystemState(SystemState);
	if (SystemState == CommonToolTags::State_Edit)
	{
		if (FMath::Abs(LastMousePosition-RightMousePosition).Length()>1)
		{
			InteractiveOption(nullptr);
			return;
		}
		else
		{
			InteractiveOption(InteractiveActor);
		}
	}
	else
	{
		if (FMath::Abs(LastMousePosition-RightMousePosition).Length()>1)
		{
			InteractiveOption(nullptr);
			ClearInteractiveActor();
			return;
		}
	}
}

void UInteractiveSubsystem::StartQueryInteractables()
{
	if (UWorld* World = GetWorld())
	{
		
		InteractionSettings.InteractableTypes.AddUnique(EObjectTypeQuery::ObjectTypeQuery1);
		InteractionSettings.InteractableTypes.AddUnique(EObjectTypeQuery::ObjectTypeQuery2);
		InteractionSettings.bUseMultiRayHit = true;
		InteractionSettings.InteractionSource= CommonToolTags::PointerInteractionSource_MouseCursor;
		
		if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
		{
			InteractionScanRate = CommonSettings->InteractionScanRate;
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
	if (GetIsDragging())
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
	if (HitComponent)
	{
		FVector RayStart; FVector RayEnd;
		GetPointerRay(RayStart,RayEnd);
		OnPointerDrag.Broadcast(HitComponent,RayStart,RayEnd);
	}
}

void UInteractiveSubsystem::UpdatePointerHover()
{
	UPrimitiveComponent * TestComponent = nullptr;
	if (bIsDragging)
	{
		if (HitComponent)
		{
			OnPointerMove.Broadcast(HitComponent);
		}
		return;
	}
	else
	{
		if (InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_MouseCursor||
			InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_World||
			InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_ScreenCenter)
		{
			if (RaycastTest(SavedHitResult))
			{
				TestComponent = SavedHitResult.GetComponent();
			}
			else
			{
				TestComponent = nullptr;
				if (HitComponent)
				{
					OnPointerLeave.Broadcast(HitComponent);
					HitComponent = TestComponent;
				}
				return;
			}
		}
		else if (InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_Custom)
		{
			TestComponent = SavedHitResult.GetComponent();
		}
		else if (InteractionSettings.InteractionSource == CommonToolTags::PointerInteractionSource_Overlap)
		{
			OverlapTest(TestComponent);
		}
		else
		{
			return;
		}

		if (TestComponent == HitComponent)
		{
			if (HitComponent)
			{
				OnPointerMove.Broadcast(HitComponent);
				return;
			}
		}
		else
		{
			if (HitComponent)
			{
				OnPointerLeave.Broadcast(HitComponent);
			}
			if (TestComponent)
			{
				OnPointerEnter.Broadcast(TestComponent);
			}
			HitComponent = TestComponent;
		}
	}
	
	if (UPrintToolLibrary::PrintDebug())
	{
		if (HitComponent)
		{
			UPrintToolLibrary::Debug(FString::Printf(TEXT("UpdatePointerHover Component::%s"),*HitComponent->GetName()));
		}
	}
}

bool UInteractiveSubsystem::IsActorInteractable(AActor* Actor)
{
	if (InteractionSettings.IncludeActors.IsEmpty() && Actor->GetClass()->ImplementsInterface(UIInteractive::StaticClass()))
	{
		return true;
	}
	else
	{
		return InteractionSettings.IncludeActors.Contains(Actor);
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
	
	DrawDebugType = UPrintToolLibrary::DrawDebug()?EDrawDebugTrace::ForDuration:EDrawDebugTrace::None;
	
	bool bIgnoreSelf = true;
	
	if (InteractionSettings.bUseMultiRayHit)
	{
		TArray<FHitResult> OutHits;
		
		bHit = UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(),RayStart,RayEnd,InteractionSettings.InteractableTypes,false,ActorsToIgnore,DrawDebugType,OutHits,bIgnoreSelf);

		for (auto OutHit : OutHits)
		{
			if (IsActorInteractable(OutHit.GetActor()))
			{
				if (UPrintToolLibrary::DrawDebug())
				{
					UPrintToolLibrary::Debug(FString::Printf(TEXT("UpdatePointerHover Component:%s"), *OutHit.GetComponent()->GetName()));
				}
				
				if (IIInteractive::Execute_GetHitPriority(OutHit.GetActor(),OutHit.GetComponent())>HighestPriority)
				{
					HighestPriority = IIInteractive::Execute_GetHitPriority(OutHit.GetActor(),OutHit.GetComponent());
					MinDistance = OutHit.Distance;
					NearestHit = OutHit;
				}
				else
				{
					if (IIInteractive::Execute_GetHitPriority(OutHit.GetActor(),OutHit.GetComponent()) == HighestPriority)
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

void UInteractiveSubsystem::OverlapTest(UPrimitiveComponent* OutComponent)
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
					if (IIInteractive::Execute_GetHitPriority(ComponentOwner,PrimitiveComponent)>HighestPriority)
					{
						HighestPriority = IIInteractive::Execute_GetHitPriority(ComponentOwner,PrimitiveComponent);
						
						Distance = UKismetMathLibrary::Vector_Distance(ComponentWorldLocation,OutPointOnBody);
						MinDistance = Distance;
						NearestComponent = PrimitiveComponent;
					}
					else
					{
						if (IIInteractive::Execute_GetHitPriority(ComponentOwner,PrimitiveComponent) == HighestPriority)
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

void UInteractiveSubsystem::SetSectionItem(FEditItem editItem)
{
	OnChangeSectionItem.Broadcast(editItem);
}

FEditItem UInteractiveSubsystem::GetSectionItem(bool& HasSection) const
{
	HasSection = EditItem.Name.IsEmpty();
	return EditItem;
}

void UInteractiveSubsystem::OnSectionChange(const FEditItem& Item)
{
	EditItem = Item;
}

void UInteractiveSubsystem::SpawnActor(TSoftClassPtr<AActor> actor)
{
	if (auto SpawnActor= actor.LoadSynchronous())
	{
		ClearInteractiveActor();
		
		// 定义碰撞参数
		FHitResult HitResult;
				
		FTransform Transform;
		// 执行射线检测
		if (LineTraceActor(HitResult))
		{
			Transform.SetLocation(HitResult.Location);
		}
		else
		{
			APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));

			if (PlayerController == nullptr)
			{
				return;
			}
			FVector2D MousePosition  = UWidgetLayoutLibrary::GetMousePositionOnViewport(PlayerController);
			
			if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
			{
				// 将屏幕位置转换为世界空间中的射线
				FVector WorldLocation, WorldDirection;
				if (PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection))
				{
					// 计算射线起点和终点
					FVector RayStart = WorldLocation;
					FVector RayEnd = RayStart + (WorldDirection * 10000000.0f);
					FVector EndLocation = RayStart + (WorldDirection * 500.0f);
					Transform.SetLocation(EndLocation);

					// 绘制调试射线
					if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
					{
						if ( CommonSettings->bDrawDebug)
						{
							DrawDebugLine(GetWorld(), RayStart, EndLocation, FColor::Green, false, 2.0f, 0, 1.0f);
						}
					}
				}
			}
		}
		
		InteractiveActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(),
														SpawnActor, Transform,
														ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (IDeviceInterface* ActorInterface = Cast<IDeviceInterface>(InteractiveActor))
		{
			ActorInterface->GetOnActorInitializationCompletedDelegateChecked().AddDynamic(this
				,&ThisClass::OnActorInitializationCompleted);
		}
		UGameplayStatics::FinishSpawningActor(InteractiveActor, Transform);
	}
}

void UInteractiveSubsystem::SpawnActor(FEditItem ActorEditItem)
{
	this->SpawnActor(ActorEditItem.Actor.LoadSynchronous());
}

void UInteractiveSubsystem::OnActorInitializationCompleted(UObject* Object)
{
	IDeviceInterface* ActorInterface = Cast<IDeviceInterface>(Object);
	if (ActorInterface)
	{
		ActorInterface->GetOnActorInitializationCompletedDelegateChecked().RemoveDynamic(this
			,&ThisClass::OnActorInitializationCompleted);
	}
	FString GUID;
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->SaveNewActorToJson(Cast<AActor>(Object),GUID);
		IDeviceInterface::ActorInitializationCompleted(Object);
	}
}

void UInteractiveSubsystem::EditActor(AActor * actor)
{
	if (actor)
	{
		InteractiveActor = actor;
	}
	
	// if (InteractiveActor)
	// {
	// 	InteractiveEditSignature.Broadcast(InteractiveActor);
	// }
}

void UInteractiveSubsystem::DeleteActor(AActor * actor)
{
	if (actor)
	{
		InteractiveActor = actor;
		if (SceneManagerSubsystem)
		{
			FString GUID;
			SceneManagerSubsystem->GetActorGUID(InteractiveActor,GUID);
			if (UAttributeToolLibrary::GetActorStatic("",GUID))
			{
				
			}
			else
			{
				InteractiveActor->Destroy();
			}
			ClearInteractiveActor();
			if (SaveSubsystem)
			{
				SaveSubsystem->OnSceneSaveActor.Broadcast(GUID,TArray<FString>());
			}
		}
	}
}

void UInteractiveSubsystem::SetDeviceMaterial(AActor* device)
{
	if (device)
	{
		if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
		{
			TArray<UActorComponent*> ActorComponents = device->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
			for (auto Component : ActorComponents)
			{
				Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(CommonSettings->OverlayMaterial.LoadSynchronous());
			}
		}
	}
	else
	{
		if (InteractiveActor)
		{
			TArray<UActorComponent*> ActorComponents = InteractiveActor->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
			for (auto Component : ActorComponents)
			{
				Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(nullptr);
			}
		}
	}
}

void UInteractiveSubsystem::GetDeviceDatabyID(const FString& DeviceID,const FGameplayTag& newState)
{
	if (UHttpSubSystem* HttpSubSystem = UHttpSubSystem::Get(this))
	{
		if (UConfigSubSystem *ConfigSubSystem = UConfigSubSystem::Get(this))
		{
			FString DeviceData;
			if (newState== CommonToolTags::DeviceState_Ledger)
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
				FString Data = FString::Printf(TEXT("deviceId=%s"), *DeviceID);
				FHttpSingleCallBack SingleCallBack;
				SingleCallBack.BindDynamic(this,&ThisClass::OnGetDeviceDataResult);
				HttpSubSystem->HttpGetCB(DeviceData,Data,SingleCallBack);
			}
		}
	}
}

void UInteractiveSubsystem::FocusActor(AActor* Actor)
{
	if (Actor)
	{
		FVector Origin;
		FVector BoxExtent;
			
		Actor->GetActorBounds(false, Origin, BoxExtent);
		
		InteractiveActorSignature.Broadcast(Actor);
		
		FTransform Transform;
		Transform.SetLocation(Origin);
		FRotator Rotator = UGameplayStatics::GetPlayerCharacter(Actor, 0)->GetActorRotation();
		Rotator.Pitch = -45.0f;
		Transform.SetRotation(Rotator.Quaternion());

		if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
		{
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

void UInteractiveSubsystem::ReturnCurrentSceneView(bool breturnMain)
{
	if (DeviceManager)
	{
		DeviceManager->SetFocusDevice("");
	}
}

void UInteractiveSubsystem::OnGetDeviceDataResult(const FString& JsonStr)
{
	GetDeviceDataResultSignature.Broadcast(JsonStr);
}
