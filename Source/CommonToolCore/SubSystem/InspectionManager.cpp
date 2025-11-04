// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionManager.h"

#include "CommonToolCoreTags.h"
#include "ConfigSubSystem.h"
#include "GameplayTagContainer.h"
#include "JsonToolLibrary.h"
#include "StateSubsystem.h"
#include "PrintToolLibrary.h"
#include "CommonToolCore/Actor/LinePath/LinePath.h"
#include "CommonToolCore/Gameplay/CommonCharacter.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

UInspectionManager::UInspectionManager()
{
}

UInspectionManager* UInspectionManager::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return UWorld::GetSubsystem<UInspectionManager>(GWorld);
	}
	return nullptr;
}

void UInspectionManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("InspectionManager::Initialize"));
}

void UInspectionManager::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("InspectionManager::Deinitialize"));
	Super::Deinitialize();
}

void UInspectionManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	InspectionCheckSignature.AddDynamic(this,&ThisClass::InspectionCheck);
	
	ConfigSubSystem = InWorld.GetGameInstance()->GetSubsystem<UConfigSubSystem>();
	if (ConfigSubSystem)
	{
		ConfigSubSystem->OnConfigChanged.AddDynamic(this, &ThisClass::OnConfigChanged);
	}
	StateSubsystem = InWorld.GetGameInstance()->GetSubsystem<UStateSubsystem>();

	if (StateSubsystem)
	{
		StateSubsystem->OnUserStateChangeCompletedSignature.AddDynamic(this, &ThisClass::OnUserStateChangeCompletedSignature);
	}
}

void UInspectionManager::InspectionCheck(bool bInspection, const FTransform& Transform)
{
	if (bInspection)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("InspectionIngLine"));
		InspectionLocation = Transform.GetLocation();
		GetWorld()->GetTimerManager().SetTimer(InspectionTimerHandle,TimerDelegate,0.001,true);
	}
	else
	{
		StateSubsystem->ChangeSystemState(CommonToolTags::State_MainMenu);
	}
}

void UInspectionManager::OnConfigChanged(const FString& ConfigString)
{
	FString InspectionRateString;
	UJsonToolLibrary::GetStringFromJsonString(ConfigString,"InspectionRate",InspectionRateString);
	InspectionRate = FCString::Atoi(*InspectionRateString);
	if (bInspectionStarted)
	{
		StartInspection(false);
	}
}

void UInspectionManager::OnSystemStateOnChange(FGameplayTag State)
{
	// if (State == CommonToolTags::State_Inspection)
	// {
	// 	for (auto Element : DynamicActorsMap)
	// 	{
	// 		RegisterActor(Element.Value);
	// 	}
	// }
	// else
	// {
	// 	StopInspection();
	// }
	// StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Null);
}

void UInspectionManager::OnUserStateChangeCompletedSignature(const FTransform& Transform, float Zoom)
{
	if (StateSubsystem)
	{
		FGameplayTag UserState;
		StateSubsystem->GetUserState(UserState);
		if (UserState == CommonToolTags::UserState_AutoRoam)
		{
			bInspectionStarted = true;
			InspectionCheckSignature.Broadcast(bInspectionStarted,Transform);
		}
	}
	//TODO到切换完成状态完成开始
	// FTimerDelegate TimerDelegate;
	// TimerDelegate.BindUFunction(this, FName("InspectionIngLine"));
	// GetWorld()->GetTimerManager().SetTimer(InspectionTimerHandle,TimerDelegate,0.001,true);
}

void UInspectionManager::RegisterActor(AActor* Actor)
{
	bool bInspection = false;
	//GetBoolAttribute(JsonObject,TEXT(""),bInspection);
	if (bInspection)
	{
		InspectionActors.AddUnique(Actor);
	}
}

void UInspectionManager::UnRegisterActor(AActor* Actor)
{
	if (InspectionActors.Contains(Actor))
	{
		InspectionActors.Remove(Actor);
	}
}

void UInspectionManager::RegisterLineActor(AActor* Actor)
{
	InspectionLineActor = Actor;
}

void UInspectionManager::StartInspection(bool bReStart)
{
	bInspectionStarted = true;
	InspectionCheckSignature.Broadcast(bInspectionStarted,FTransform());
	//是否重新开始检视
	if (bReStart)
	{
		InspectionIndex = 0;
	}
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("InspectionIng"),InspectionIndex);
	GetWorld()->GetTimerManager().SetTimer(InspectionTimerHandle,TimerDelegate,InspectionRate,true);
}

void UInspectionManager::InspectionIng()
{
	if (InspectionActors.IsEmpty())
	{
		UPrintToolLibrary::Waring("ViewActors is Empty!");
	}
	else
	{
		// if (InspectionIndex<InspectionActors.Num() && InteractiveSubsystem)
		// {
		// 	if (InspectionActors.IsValidIndex(InspectionIndex))
		// 	{
		// 		InteractiveSubsystem->InteractiveOnClickedSignature.Broadcast(ViewActors[InspectionIndex]);
		// 	}
		// 	InspectionIndex++;
		// }
		// else
		// {
		// 	InspectionIndex = 0;
		// 	InteractiveSubsystem->InteractiveOnClickedSignature.Broadcast(ViewActors[InspectionIndex]);
		// 	InspectionIndex++;
		// }
	}
}

void UInspectionManager::StopInspection()
{
	bInspectionStarted = false;
	GetWorld()->GetTimerManager().ClearTimer(InspectionTimerHandle);
	InspectionCheckSignature.Broadcast(bInspectionStarted,FTransform());
}

bool UInspectionManager::GetInspectionStarted()
{
	return bInspectionStarted;
}

void UInspectionManager::StartInspectionLine(bool bReStart)
{
	if (!InspectionLineActor)
	{
		return;
	}
	//TODO 后面改成距离样条线距离最近的点的Progress
	FTransform StartInspectionTransform;
	if (USplineComponent * SplineComponent = InspectionLineActor->FindComponentByClass<USplineComponent>())
	{
		if (ACharacter *Character = UGameplayStatics::GetPlayerCharacter(GWorld,0))
		{
			DebugSplineInfo();
			if (!SplineComponent) return;
			// 获取输入键并转换为进度
			float InputKey = SplineComponent->FindInputKeyClosestToWorldLocation(Character->GetActorLocation());
			// 将输入键转换为世界位置
			FVector Location = SplineComponent->GetLocationAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);
			FRotator Rotation = SplineComponent->GetRotationAtSplineInputKey(InputKey,ESplineCoordinateSpace::World);
			CurrentProgress = FMath::Clamp(InputKey / SplineComponent->GetNumberOfSplinePoints(), 0.0f, 1.0f);
			
			StartInspectionTransform.SetLocation(Location);
			// 确保方向非零
			if (!Rotation.IsNearlyZero())
			{
				// 可选：调整旋转（如仅绕Z轴旋转）
				// TargetRotation.Pitch = 0;
				// TargetRotation.Roll = 0;
				StartInspectionTransform.SetRotation(Rotation.Quaternion());
				if (StateSubsystem)
				{
					StateSubsystem->ChangeUserState(CommonToolTags::UserState_AutoRoam,StartInspectionTransform,0,true);
				}
			}
		}
	}
}

void UInspectionManager::InspectionIngLine()
{
	if (InspectionLineActor)
	{
		if (USplineComponent * SplineComponent = InspectionLineActor->FindComponentByClass<USplineComponent>())
		{
			// 更新进度（移动）
			const float FixedDeltaTime = 1.0f / 60.0f; // 60Hz固定更新
			CurrentProgress = FMath::Clamp(CurrentProgress+MoveSpeed * 10 * FixedDeltaTime/SplineComponent->GetSplineLength(), 0.0f, 1.0f);
		
			FVector NewLocation = SplineComponent->GetLocationAtSplineInputKey(CurrentProgress*SplineComponent->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
			FRotator NewRotation = SplineComponent->GetRotationAtSplineInputKey(CurrentProgress*SplineComponent->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
			
			FTransform NewTransform;
			NewTransform.SetLocation(NewLocation+FVector(0, 0, 360));
			NewTransform.SetRotation(NewRotation.Quaternion());
			InspectionTransformSignature.Broadcast(NewTransform);
		}
	}
}

void UInspectionManager::StopInspectionLine()
{
	bInspectionStarted = false;
	GetWorld()->GetTimerManager().ClearTimer(InspectionTimerHandle);
	InspectionCheckSignature.Broadcast(bInspectionStarted,FTransform());
}

void UInspectionManager::SetInspectionLineSpeed(float speed)
{
	MoveSpeed = speed;
}

void UInspectionManager::GetInspectionLineLocationAndDirection(const FVector& BaseLocation, FVector& ResultLocation,
	FVector& ResultDirection)
{
	if (InspectionLineActor)
	{
		
	}
	else
	{
		InspectionLineActor = UGameplayStatics::GetActorOfClass(GWorld,ALinePath::StaticClass());
	}
	if (InspectionLineActor)
	{
		if (USplineComponent * SplineComponent = InspectionLineActor->FindComponentByClass<USplineComponent>())
		{
			ResultLocation  = SplineComponent->FindLocationClosestToWorldLocation(BaseLocation,ESplineCoordinateSpace::World);
			ResultDirection = SplineComponent->FindDirectionClosestToWorldLocation(BaseLocation,ESplineCoordinateSpace::World);
		}
	}
}

void UInspectionManager::DebugSplineInfo()
{
	if (USplineComponent * Spline = InspectionLineActor->FindComponentByClass<USplineComponent>())
	{
		float SplineLength = Spline->GetSplineLength();
		int32 NumPoints = Spline->GetNumberOfSplinePoints();
		bool bIsClosed = Spline->IsClosedLoop();
    
		UE_LOG(LogTemp, Warning, TEXT("Spline Info:"));
		UE_LOG(LogTemp, Warning, TEXT("  Length: %.2f"), SplineLength);
		UE_LOG(LogTemp, Warning, TEXT("  Points: %d"), NumPoints);
		UE_LOG(LogTemp, Warning, TEXT("  Closed: %d"), bIsClosed);
    
		// 获取所有点的输入键
		for (int32 i = 0; i < NumPoints; i++)
		{
			FVector PointLocation = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			float InputKey = Spline->FindInputKeyClosestToWorldLocation(PointLocation);
			UE_LOG(LogTemp, Warning, TEXT("  Point %d: InputKey=%.2f"), i, InputKey);
		}
	}
}
