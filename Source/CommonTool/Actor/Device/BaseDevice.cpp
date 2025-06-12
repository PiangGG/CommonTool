// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDevice.h"

#include "CommonTool/Library/JsonFunctionLibrary.h"
#include "CommonTool/SubSystem/InteractiveSubsystem.h"
#include "CommonTool/SubSystem/SceneManagerSubsystem.h"
#include "CommonTool/SubSystem/StateSubsystem.h"

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
		UJsonFunctionLibrary::GetJsonStringFromJsonObject(GetDeviceJsonObjectData(FString()),Data);
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
	UJsonFunctionLibrary::GetJsonStringFromJsonObject(JsonObject,Data);
	// FString ActorTransform;
	// JsonObject->TryGetStringField(TEXT("ActorTransform"),ActorTransform);
	// Transform.InitFromString(ActorTransform);
	// SetActorTransform(Transform);
	// FString ActorKey;
	// FString GUID;
	// JsonObject->TryGetStringField(TEXT("ActorName"),ActorKey);
	// JsonObject->TryGetStringField(TEXT("GUID"),GUID);
	// JsonObject->TryGetStringField(TEXT("bLoad"),BIsLoad);
	// //DeviceGuid = FGuid(GUID);
	// if (USceneManagerSubsystem* SceneManagerSubsystem = UWorld::GetSubsystem<USceneManagerSubsystem>(
	// 		GWorld))
	// {
	// 	SceneManagerSubsystem->UnRegisterActor(this);
	// 	Rename(*ActorKey);
	// 	SceneManagerSubsystem->RegisterActor(this);
	// }
	// UJsonFunctionLibrary::GetJsonStringFromJsonObject(JsonObject,Data);
	//
	// SetDeviceLoad_Implementation(BIsLoad);
}

void ABaseDevice::ReDraw()
{
	
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
	UJsonFunctionLibrary::GetJsonStringArrayFromJsonString(Data,TEXT("Attribute"),Attributes);
}

void ABaseDevice::GetAttributes_Implementation(TArray<FString>& Attributes)
{
	UJsonFunctionLibrary::GetJsonStringArrayFromJsonString(Data,TEXT("Attribute"),Attributes);
}
