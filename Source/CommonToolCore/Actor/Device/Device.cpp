// Fill out your copyright notice in the Description page of Project Settings.


#include "Device.h"

#include "DeviceManager.h"
#include "JsonToolLibrary.h"
#include "StateSubsystem.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADevice::ADevice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ADevice::SetDeviceNodes(const FDeviceConstructStruct &DeviceConstructStruct)
{
	DeviceStruct = DeviceConstructStruct;

	for (auto Element : DeviceComponentMap)
	{
		if (Element.Key!=nullptr)
		{
			Element.Key->DestroyComponent();
		}
	}
	DeviceComponentMap.Empty();
	DisassembleTypes.Empty();
	for (auto Element : DeviceStruct.DeviceTreeNodeStruct)
	{
		UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this);
		StaticMeshComponent->CreationMethod = EComponentCreationMethod::SimpleConstructionScript;
		StaticMeshComponent->bAutoRegister = true;
		StaticMeshComponent->SetupAttachment(RootComponent);
		StaticMeshComponent->SetNetAddressable();

		UAssetManager::GetStreamableManager().RequestAsyncLoad(Element.StaticMesh.ToSoftObjectPath(),FStreamableDelegate::CreateLambda(
[this,Element,StaticMeshComponent]()
		{
			if (Element.StaticMesh)
			{
				StaticMeshComponent->SetStaticMesh(Element.StaticMesh.Get());
			}
			StaticMeshComponent->SetMobility(EComponentMobility::Movable);
			//Element.bInteractive = true;

			if (Element.bShowPOI)
			{
				StaticMeshComponent->SetCollisionProfileName(FName(TEXT("Interactive")));
				StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
			}
			else
			{
				StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
			}
			if (!Element.Type.IsEmpty())
			{
				DisassembleTypes.AddUnique(Element.Type);
			}
			StaticMeshComponent->CastShadow = true;
			StaticMeshComponent->bAffectDynamicIndirectLighting  = true;
			StaticMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
			FinishAndRegisterComponent(StaticMeshComponent);
			DeviceComponentMap.Add(StaticMeshComponent,Element);
			if (DeviceComponentMap.Num()>=DeviceStruct.DeviceTreeNodeStruct.Num())
			{
				DisassembleDevice();
			}
		}));
	}
}

void ADevice::FindDeviceComponentData(UStaticMeshComponent* MeshComponent, FDeviceTreeNodeStruct& DeviceComponentData)
{
	if (!DeviceComponentMap.IsEmpty() && MeshComponent)
	{
		DeviceComponentData = DeviceComponentMap.FindRef(MeshComponent);
	}
}

// Called when the game starts or when spawned
void ADevice::BeginPlay()
{
	Super::BeginPlay();

	StateSubsystem = UStateSubsystem::Get(this);
	StateSubsystem->DeviceStateOnChange.AddDynamic(this,&ThisClass::OnDeviceStateOnChange);
	DeviceManager = UDeviceManager::Get(this);

	if (DeviceManager)
	{
		DeviceManager->OnDeviceDisassemble.AddDynamic(this,&ThisClass::OnDeviceDisassemble);
	}
	
	if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(CommonSettings->OverlayMaterial.ToSoftObjectPath(),FStreamableDelegate::CreateLambda(
[this,CommonSettings]()
		{
			OverlayMaterial = CommonSettings->OverlayMaterial.Get();
		}));
		
		UAssetManager::GetStreamableManager().RequestAsyncLoad(CommonSettings->DeviceDataTable.ToSoftObjectPath(),FStreamableDelegate::CreateLambda(
[this,CommonSettings]()
		{
			DeviceDataTable = CommonSettings->DeviceDataTable.Get();
		}));
	}
}

void ADevice::OnDeviceStateOnChange(FGameplayTag State, const FString& ID)
{
	if (State.MatchesTag(CommonToolTags::DeviceState_Disassemble)&&!ID.IsEmpty() && DeviceDataTable)
	{
		
		if (DeviceManager)
		{
			FString DeviceData;
			if (DeviceManager->GetDeviceData(ID,DeviceData))
			{
				FString psrType;
				UJsonToolLibrary::GetStringFromJsonString(DeviceData, TEXT("psrType"), psrType);

				FString ContextString; // 用于错误输出，如果查找失败会记录在这里
				// 使用FindRow查找特定行
				FDeviceConstructStruct* RowData = DeviceDataTable->FindRow<FDeviceConstructStruct>(FName(psrType), ContextString);
				if (RowData)
				{
					// 使用行数据，例如读取Score
					SetDeviceNodes(*RowData);
					SetActorHiddenInGame(false);
					StartQuery();
					return;
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to find row: %s"), *ContextString);
				}
			}
			else
			{
				
			}
		}
	}
	EndQuery();
	SetActorHiddenInGame(true);
}

void ADevice::DisassembleDevice()
{
	if (DeviceManager)
	{
		DeviceManager->DisassembleDevice(this,"");
	}
	
	BlendAlphaMap.Empty();
	
	// GetWorld()->GetTimerManager().ClearTimer(DisassembleDeviceTimeHandle);
	// DisassembleBlendAlpha = 0.0f;
	// FTimerDelegate TimerDelegate;
	// TimerDelegate.BindUFunction(this, FName("UpdateDisassembleDevice"));
	// GetWorld()->GetTimerManager().SetTimer(DisassembleDeviceTimeHandle, TimerDelegate, 0.001, true);
	for (auto Element : DeviceComponentMap)
	{
		// FVector TargetRelativeLocation = FMath::Lerp(Element.Key->GetRelativeLocation(), Element.Value.DisassembleTargetPosition,DisassembleBlendAlpha);
		// Element.Key->SetRelativeLocation(TargetRelativeLocation);
		//
		if (Element.Key->IsValidLowLevel()&&Element.Key)
		{
			// FTimerHandle TimerHandle;
			// TimerHandleMap.Add(Element.Key,TimerHandle);
			// BlendAlphaMap.Add(Element.Key,0);
			//FTimerDelegate TimerDelegate;
			//TimerDelegate.BindUFunction(this, "UpdateDisassembleDevice",Element.Key,Element.Value); // 注意这里传递参数的方式略有不同，需要使用BindUFunction而非BindRaw或Lambda表达式。
			FTimerHandle TimerHandle;
			int32 DisassembleBlendAlpha = 0;
			double StartTime = GetWorld()->GetTime().GetWorldTimeSeconds();
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda(
	[this,&TimerHandle,StartTime,Element]()
			{
				double LoopTime = (GetWorld()->GetTime().GetWorldTimeSeconds() - StartTime)*0.01;
				if (LoopTime >= 1)
				{
					GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
				}
				else
				{
					FVector TargetRelativeLocation = FMath::Lerp(Element.Key->GetRelativeLocation(), Element.Value.DisassembleTargetPosition,LoopTime);
					Element.Key->SetRelativeLocation(TargetRelativeLocation);
				}
			}), 0.01, true,Element.Value.DelayTime);
			TimerHandleMap.Add(Element.Key,TimerHandle);
		}
	}
}

void ADevice::UpdateDisassembleDevice(UStaticMeshComponent* MeshComponent, const FDeviceTreeNodeStruct &DeviceTreeNodeStruct)
{
	int32 DisassembleBlendAlpha = BlendAlphaMap.FindRef(MeshComponent);
	DisassembleBlendAlpha++;
	if (DisassembleBlendAlpha >= 1000)
	{
		auto TimerHandle = TimerHandleMap.FindRef(MeshComponent);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		DisassembleBlendAlpha = 1000;
	}
	else
	{
		FVector TargetRelativeLocation = FMath::Lerp(MeshComponent->GetRelativeLocation(), DeviceTreeNodeStruct.DisassembleTargetPosition,DisassembleBlendAlpha*0.01);
		MeshComponent->SetRelativeLocation(TargetRelativeLocation);
		BlendAlphaMap.Add(MeshComponent,DisassembleBlendAlpha);
	}
}

void ADevice::OnDeviceDisassemble(ADevice* Device, const FString& Type)
{
	if (!Type.IsEmpty()&&!DisassembleTypes.IsEmpty())
	{
		for (auto Element : DeviceComponentMap)
		{
			Element.Key->SetVisibility(Element.Value.Type == Type);
		}
	}
	else
	{
		for (auto Element : DeviceComponentMap)
		{
			Element.Key->SetVisibility(true);
		}
	}
}

void ADevice::StartQuery()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::Query, 0.1, true);
	}
}

void ADevice::Query()
{
	FHitResult HitResult = LineTraceProfileActor(FName(TEXT("Interactive")));
	if (UMeshComponent * MeshComponent = Cast<UMeshComponent>(HitResult.GetComponent()))
	{
		if (QueryComponent == MeshComponent)
		{
		
		}else
		{
			if (QueryComponent)
			{
				QueryComponent->SetOverlayMaterial(nullptr);
				QueryComponent = nullptr;
			}
			QueryComponent = MeshComponent;
			if (OverlayMaterial&&QueryComponent)
			{
				QueryComponent->SetOverlayMaterial(OverlayMaterial);
			}
		}
	}
	else
	{
		if (QueryComponent)
		{
			QueryComponent->SetOverlayMaterial(nullptr);
			QueryComponent = nullptr;
		}
	}
}
void ADevice::EndQuery()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
		for (auto Element : TimerHandleMap)
		{
			if (Element.Value.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(Element.Value);
			}
		}
		TimerHandleMap.Empty();
	}
}

FHitResult ADevice::LineTraceProfileActor(const FName& name)
{
	// 获取鼠
	float MouseX, MouseY;
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController == nullptr)
	{
		return FHitResult();
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
			CollisionParams.AddIgnoredActor(UGameplayStatics::GetPlayerController(this, 0));
			CollisionParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(this, 0));
			// 执行射线检测
			bool bHit = GetWorld()->LineTraceSingleByProfile(HitResult, RayStart, RayEnd, name,
															 CollisionParams);
			if (bHit)
			{
				// 处理碰撞结果
				AActor* LocalHitActor = HitResult.GetActor();
				if (LocalHitActor)
				{
					return HitResult;
				}
			}
		}
	}
	return FHitResult();
}
