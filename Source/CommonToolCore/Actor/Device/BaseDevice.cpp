// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDevice.h"

#include "AttributeToolLibrary.h"
#include "CommonSettings.h"
#include "CommonToolCore/Library/JsonToolLibrary.h"
#include "CommonToolCore/SubSystem/InteractiveSubsystem.h"
#include "CommonToolCore/SubSystem/SceneManagerSubsystem.h"
#include "CommonToolCore/SubSystem/StateSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

// Sets default values
ABaseDevice::ABaseDevice(): DeviceIndex(""), Data()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(Root);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(Root);
	BoxCollision->SetHiddenInGame(true);
	BoxCollision->SetCollisionProfileName(TEXT("Editable"));
	
}

// Called when the game starts or when spawned
void ABaseDevice::BeginPlay()
{
	Super::BeginPlay();

	GetOnOnActorReDrawDelegateChecked().AddDynamic(this,&ThisClass::ReDraw);
	DeviceBoundsSet.AddDynamic(this,&ThisClass::OnDeviceBoundsSet);
	if (UStateSubsystem * StateSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UStateSubsystem>())
	{
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);

		FGameplayTag SystemState;
		StateSubsystem->GetSystemState(SystemState);
		BoxCollision->SetHiddenInGame(SystemState != CommonToolTags::State_Edit);
	}
}

void ABaseDevice::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FString GUID;
	USceneManagerSubsystem::Get(this)->GetActorGUID(this,GUID);
	USceneManagerSubsystem::Get(this)->UnRegisterActor(GUID);
	Super::EndPlay(EndPlayReason);
}

AActor* ABaseDevice::GetActor_Implementation()
{
	return this;
}

FString ABaseDevice::GetDeviceData_Implementation()
{
	if (Data.IsEmpty())
	{
		UJsonToolLibrary::GetJsonStringFromJsonObject(GetDeviceJsonObjectData(FString()),Data);
	}
	return Data;
}

FString ABaseDevice::GetDeviceIndex_Implementation()
{
	if (DeviceIndex.IsEmpty())
	{
		return GetName();
	}
	return DeviceIndex;
}

void ABaseDevice::InteractiveDevice_Implementation()
{
	// if (InfoWidget)
	// {
	// 	InfoWidget->SetVisibility(true);
	// 	//InfoWidget->SetWidgetClass(InfoWidgetClass);
	// }
}

void ABaseDevice::DeInteractiveDevice_Implementation()
{
	// if (InfoWidget)
	// {
	// 	InfoWidget->SetVisibility(false);
	// 	//InfoWidget->SetWidgetClass(nullptr);
	// }
}

TSharedRef<FJsonObject> ABaseDevice::GetDeviceJsonObjectData(const FString &GUID)
{
	TSharedRef<FJsonObject> ActorJson = MakeShared<FJsonObject>();
	if (GUID.IsEmpty())
	{
		return ActorJson;
	}
	ActorJson->SetStringField(TEXT("GUID"),GUID);
	//actor的静态属性集合
	TArray<TSharedPtr<FJsonValue>> Attribute;
	TSharedPtr<FJsonObject> ActorJsonName = MakeShared<FJsonObject>();
	ActorJsonName->SetStringField(TEXT("AttributeName"), "ActorName");
	ActorJsonName->SetStringField(TEXT("AttributeValue"), GetName());
	ActorJsonName->SetStringField(TEXT("AttributeType"), "EditType.Normal");
	ActorJsonName->SetBoolField(TEXT("bDisplay"), true);
	ActorJsonName->SetBoolField(TEXT("bCanEditor"), true);
	Attribute.Add(MakeShared<FJsonValueObject>(ActorJsonName));
	
	TSharedPtr<FJsonObject> ActorJsonClass = MakeShared<FJsonObject>();
	ActorJsonClass->SetStringField(TEXT("AttributeName"), "ActorClass");
	ActorJsonClass->SetStringField(TEXT("AttributeValue"), TSoftClassPtr<AActor>(GetClass()).ToString());
	ActorJsonClass->SetStringField(TEXT("AttributeType"), "EditType.Normal");
	ActorJsonClass->SetBoolField(TEXT("bDisplay"), true);
	ActorJsonClass->SetBoolField(TEXT("bCanEditor"), false);
	Attribute.Add(MakeShared<FJsonValueObject>(ActorJsonClass));

	TSharedPtr<FJsonObject> ActorJsonTransform = MakeShared<FJsonObject>();
	ActorJsonTransform->SetStringField(TEXT("AttributeName"), "ActorTransform");
	ActorJsonTransform->SetStringField(TEXT("AttributeValue"),GetActorTransform().ToString());
	ActorJsonTransform->SetStringField(TEXT("AttributeType"), "EditType.Normal");
	ActorJsonTransform->SetBoolField(TEXT("bDisplay"), true);
	ActorJsonTransform->SetBoolField(TEXT("bCanEditor"), false);
	Attribute.Add(MakeShared<FJsonValueObject>(ActorJsonTransform));
	
	TSharedPtr<FJsonObject> ActorJsonbLoad = MakeShared<FJsonObject>();
	ActorJsonbLoad->SetStringField(TEXT("AttributeName"), "bLoad");
	ActorJsonbLoad->SetBoolField(TEXT("AttributeValue"), true);
	ActorJsonbLoad->SetStringField(TEXT("AttributeType"), "EditType.Check");
	ActorJsonbLoad->SetBoolField(TEXT("bDisplay"), true);
	ActorJsonbLoad->SetBoolField(TEXT("bCanEditor"), true);
	Attribute.Add(MakeShared<FJsonValueObject>(ActorJsonbLoad));

	TSharedPtr<FJsonObject> ActorJsonInspection = MakeShared<FJsonObject>();
	ActorJsonInspection->SetStringField(TEXT("AttributeName"), "bInspection");
	ActorJsonInspection->SetBoolField(TEXT("AttributeValue"), false);
	ActorJsonInspection->SetStringField(TEXT("AttributeType"), "EditType.Check");
	ActorJsonInspection->SetBoolField(TEXT("bDisplay"), true);
	ActorJsonInspection->SetBoolField(TEXT("bCanEditor"), true);
	Attribute.Add(MakeShared<FJsonValueObject>(ActorJsonInspection));

	ActorJson->SetArrayField(TEXT("Attribute"),Attribute);
	return ActorJson;
}

void ABaseDevice::SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject)
{
	UJsonToolLibrary::GetJsonStringFromJsonObject(JsonObject,Data);
	
	FString StaticMeshPath;
	UAttributeToolLibrary::GetStringAttribute(JsonObject,TEXT("DynamicStaticMesh"),StaticMeshPath);
	SetMeshComponent(StaticMeshPath);

	bool bEnablePOI;
	UAttributeToolLibrary::GetBoolAttribute(JsonObject,TEXT("POI"),bEnablePOI);
	SetEnablePOI(bEnablePOI);
}

void ABaseDevice::SetEnablePOI(bool bset)
{
	if (bset)
	{
		
	}
	else
	{
		
	}
}

void ABaseDevice::SetMeshComponent(const FString& Path)
{
	if (UStaticMeshComponent *DynamicMeshComponent = Cast<UStaticMeshComponent>(FindComponentByName(TEXT("DynamicStaticMesh"))))
	{
		if (Path.IsEmpty())
		{
			DynamicMeshComponent->SetStaticMesh(nullptr);
			DeviceBoundsSet.Broadcast(false);
		}
		else
		{
			FSoftObjectPath SoftPath(Path);
			// 创建异步加载请求
			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
			 	SoftPath,
			 	FStreamableDelegate::CreateLambda([this,SoftPath,DynamicMeshComponent]()
			 	{
			 		if (DynamicMeshComponent)
			 		{
			 			DynamicMeshComponent->SetStaticMesh(Cast<UStaticMesh>(SoftPath.ResolveObject()));
			 			DeviceBoundsSet.Broadcast(true);
			 		}
			 	})
			 );
		}
	}
}

UActorComponent* ABaseDevice::FindComponentByName(FName ComponentName)
{
	if (!ComponentName.IsValid())
	{
		return nullptr;
	}

	TArray<UActorComponent*> Components;
	GetComponents(Components);

	for (UActorComponent* Component : Components)
	{
		if (Component && Component->GetFName() == ComponentName)
		{
			return Component;
		}
	}
	UStaticMeshComponent *DynamicMeshComponent = NewObject<UStaticMeshComponent>(this,ComponentName);
	if (DynamicMeshComponent)
	{
		// 注册组件
		DynamicMeshComponent->RegisterComponent();
		DynamicMeshComponent->Rename(*ComponentName.ToString());
		// 附加到根组件
		DynamicMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		// 设置碰撞预设
		DynamicMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		return DynamicMeshComponent;
	}
	return nullptr;
}

UUserWidget* ABaseDevice::GetDevicePOI()
{
	if (UserWidget)
	{
		return UserWidget;
	}
	else
	{
		if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
		{
			if (CommonSettings->DefaultPOI != nullptr)
			{
				UserWidget = CreateWidget<UUserWidget>(GetWorld(),CommonSettings->DefaultPOI.Get());
			}
		}
		return UserWidget;
	}
}

void ABaseDevice::ReDraw()
{
	
}

void ABaseDevice::OnDeviceBoundsSet(bool bSetMesh)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this,bSetMesh]
	{
		if (BoxCollision)
		{
			if (bSetMesh)
			{
				FVector Origin;
			FVector BoxExtent;
			GetActorBounds(false, Origin,BoxExtent);
			BoxCollision->SetBoxExtent(BoxExtent);
			BoxCollision->SetRelativeLocation(FVector(0,0,BoxExtent.Z));
			}
			else
			{
				BoxCollision->SetBoxExtent(FVector(32));
				BoxCollision->SetRelativeLocation(FVector(0));
			}
		}
	}));
}

FOnActorInitializationCompleted* ABaseDevice::GetOnActorInitializationCompletedDelegate()
{
	return &OnActorInitializationCompletedDelege;
}

FOnActorReDraw* ABaseDevice::GetOnActorReDrawDelegate()
{
	return &OnActorReDrawDelege;
}

void ABaseDevice::SetDeviceLoad_Implementation(const FString& bload)
{
	bool b = bload =="true";
	SetActorHiddenInGame(!b);
}

void ABaseDevice::OnSystemStateOnChange(FGameplayTag newState)
{
	if (bReView)
	{
		SetEditing(newState == CommonToolTags::State_Edit || newState == CommonToolTags::State_Inspection);
	}
	else
	{
		SetEditing(newState == CommonToolTags::State_Edit || newState == CommonToolTags::State_Simulate_Maintenance);
	}
}

void ABaseDevice::SetEditing(bool edit)
{
	BoxCollision->SetHiddenInGame(!edit);
}


bool ABaseDevice::GetbIsCanReView_Implementation()
{
	return bReView;
}

FString ABaseDevice::GetItemIcon_Implementation()
{
	return ItemIcon;
}

void ABaseDevice::GetAttributeValue_Implementation(const FString& AttributeName, FString& AttributeValue)
{
	TArray<FString> Attributes;
	UJsonToolLibrary::GetJsonStringArrayFromJsonString(Data,TEXT("Attribute"),Attributes);
	FString Name;
	for (auto Attribute : Attributes)
	{
		UJsonToolLibrary::GetStringFromJsonString(Attribute,"AttributeName",Name);
		if (Name == AttributeName)
		{
			UJsonToolLibrary::GetStringFromJsonString(Attribute,"AttributeValue",AttributeValue);
			return;
		}
	}
}

void ABaseDevice::GetAttributes_Implementation(TArray<FString>& Attributes)
{
	if (Data.IsEmpty())
	{
		
	}
	else
	{
		UJsonToolLibrary::GetJsonStringArrayFromJsonString(Data,TEXT("Attribute"),Attributes);
	}
}
