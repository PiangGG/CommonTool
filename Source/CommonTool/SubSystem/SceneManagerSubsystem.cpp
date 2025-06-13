// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneManagerSubsystem.h"
#include "CommonTool/Interface/DeviceInterface.h"
#include "CommonToolTags.h"
#include "Blueprint/UserWidget.h" 
#include "ConfigSubSystem.h"
#include "DeviceManager.h"
#include "EngineUtils.h"
#include "ImportExportSubsystem.h"
#include "LoadingSubsystem.h"
#include "SaveSubsystem.h"
#include "StateSubsystem.h"
#include "TrainStationManager.h"
#include "UIManagerSubsystem.h"
#include "Animation/AnimRootMotionProvider.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "CommonTool/Actor/CommonInfo.h"
#include "CommonTool/Actor/Positioning.h"
#include "CommonTool/Components/DeviceMarkComponent.h"
#include "CommonTool/Library/CommDeveloperSettings.h"
#include "CommonTool/Library/JsonToolLibrary.h"
#include "CommonTool/Library/PrintToolLibrary.h"
#include "CommonTool/Object/WorldAssetTreeNode.h"
#include "CommonTool/RuntimeGizemo/CommonGizmoTransform.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"

void USceneManagerSubsystem::PrintTree()
{
	OnWorldAssetTreeNodesUpdate.Broadcast(WorldAssetTreeNodes);
}

USceneManagerSubsystem::USceneManagerSubsystem():
CommonInfo(nullptr),
CommonGizmoTransform(nullptr),
StateSubsystem(nullptr),
ConfigSubSystem(nullptr),
SaveSubsystem(nullptr),
ImportExportSubsystem(nullptr),
InteractiveSubsystem(nullptr)
{
	// GWorld->OnWorldBeginPlay.AddUObject(this,&ThisClass::WorldBeginPlay);
	// GWorld->OnActorsInitialized.AddUObject(this,&ThisClass::ActorsInitialized);
}

USceneManagerSubsystem* USceneManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return UWorld::GetSubsystem<USceneManagerSubsystem>(GWorld);
	}
	return nullptr;
}

void USceneManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("SceneManagerSubsystem::Initialize"));
	
}

void USceneManagerSubsystem::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("SceneManagerSubsystem::Deinitialize"));
	Super::Deinitialize();
}

void USceneManagerSubsystem::OnUserStateOnChange(FGameplayTag State, const FTransform& Transform, float Zoom,bool bblend)
{
	// if (State != CommonToolTags::UserState_Focus)
	// {
	// 	StopInspection();
	// }
}

void USceneManagerSubsystem::OnSystemStateOnChange(const FGameplayTag State)
{
	if (State == CommonToolTags::State_Inspection)
	{
		for (auto Element : ActorsMap)
		{
			AddInspectionActor(Element.Value);
		}
	}
	else
	{
		StopInspection();
		ReSetDeviceInfo(TArray<FString>());
	}
	StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Null);
}

void USceneManagerSubsystem::OnDeviceStateOnChange(FGameplayTag newState)
{
	if (newState == CommonToolTags::DeviceState_Ledger)
	{
		if (UIManagerSubsystem)
		{
			if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
			{
				if (Settings->ActorInfoWidget != nullptr)
				{
					//InfoWidget->SetWidgetClass(Settings->ActorInfoWidget);
					if (!UserWidget)
					{
						UserWidget = CreateWidget<UUserWidget>(GetWorld(),Settings->DeviceInfoSelectWidget.Get());
					}
					UIManagerSubsystem->ReSetDeviceInfo(CurrentForceDeviceID,CurrentForceDevice,true,CommonToolTags::E3DUI_Type_DeviceInfoSelect,UserWidget);
				}
			}
		}
	}
	else if (newState == CommonToolTags::DeviceState_Null)
	{
		if (UserWidget && CurrentForceDevice)
		{
			UIManagerSubsystem->ReSetDeviceInfo(CurrentForceDeviceID,CurrentForceDevice,false,CommonToolTags::E3DUI_Type_DeviceInfoSelect,UserWidget);
		}
	}
}

void USceneManagerSubsystem::OnConfigChanged(const FString& ConfigString)
{
	FString InspectionRateString;
	UJsonToolLibrary::GetStringFromJsonString(ConfigString,"InspectionRate",InspectionRateString);
	InspectionRate = FCString::Atoi(*InspectionRateString);
	if (bInspectionStarted)
	{
		StartInspection(false);
	}
}

void USceneManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	UPrintToolLibrary::Debug(FString("SceneManagerSubsystem::OnWorldBeginPlay"));

	OnSceneChanged.AddDynamic(this,&ThisClass::HandleOnSceneChanged);
	StateSubsystem = InWorld.GetGameInstance()->GetSubsystem<UStateSubsystem>();
	if (StateSubsystem)
	{
		StateSubsystem->UserStateOnChange.AddDynamic(this, &ThisClass::OnUserStateOnChange);
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);
	}

	ConfigSubSystem = InWorld.GetGameInstance()->GetSubsystem<UConfigSubSystem>();
	if (ConfigSubSystem)
	{
		ConfigSubSystem->OnConfigChanged.AddDynamic(this, &ThisClass::OnConfigChanged);
	}
	
	InteractiveSubsystem = UWorld::GetSubsystem<UInteractiveSubsystem>(GWorld);
	if (InteractiveSubsystem)
	{
		InteractiveSubsystem->OnUISectionChange.AddDynamic(this, &ThisClass::UISectionChange);
		InteractiveSubsystem->ForceDeviceSignature.AddDynamic(this, &ThisClass::ForceDevice);
		StateSubsystem->DeviceStateOnChange.AddDynamic(this, &ThisClass::OnDeviceStateOnChange);
	}
	
	ImportExportSubsystem = InWorld.GetGameInstance()->GetSubsystem<UImportExportSubsystem>();
	
	SaveSubsystem = InWorld.GetGameInstance()->GetSubsystem<USaveSubsystem>();
	if (SaveSubsystem)
	{
		SaveSubsystem->OnSceneSave.AddDynamic(this,&ThisClass::OnSave);
		SaveSubsystem->OnSceneSaveActor.AddDynamic(this,&ThisClass::OnSaveActor);
	}
	TrainStationManager =  UWorld::GetSubsystem<UTrainStationManager>(GWorld);
	if (TrainStationManager)
	{
		TrainStationManager->OnTrainStationSelectedComplete.AddDynamic(this,&ThisClass::OnTrainStationSelectedComplete);
	}
	DeviceManager = UWorld::GetSubsystem<UDeviceManager>(GWorld);
	if (DeviceManager)
	{
		DeviceManager->OnDeviceInfoPoolRefresh.AddDynamic(this,&ThisClass::OnDeviceInfoPoolRefresh);
	}

	UIManagerSubsystem = UUIManagerSubsystem::Get(this);
	//初始化场景编辑actor 与设备显示Actor
	InitializeScene();
	
	InitSceneAllDevice();
}

void USceneManagerSubsystem::HandleOnSceneChanged(const FString& NewSceneName)
{
	//关卡切换的时候卸载被切换掉的actor
	LastSceneName = CurrentSceneName;
	UnLoadSceneHierarchyFromJson(LastSceneName);
	//关卡切换的时候加载切换到目标场景的actor
	CurrentSceneName = NewSceneName;
}

void USceneManagerSubsystem::SetCurrentSceneName(const FString& NewSceneName) const
{
	OnSceneChanged.Broadcast(NewSceneName);
}

void USceneManagerSubsystem::PointCloudRebuilt()
{
	// 这段代码将在游戏线程执行
	if (IsInGameThread())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("PointCloudRebuiltNextTick"));
		GetWorld()->GetTimerManager().ClearTimer(SceneBuildTimer);
		GetWorld()->GetTimerManager().SetTimer(SceneBuildTimer, TimerDelegate, 0.1, true,0.5);
	}
}

void USceneManagerSubsystem::PointCloudRebuiltNextTick()
{
	ULoadingSubsystem::ShowLoadScreen(false);
	GetWorld()->GetTimerManager().ClearTimer(SceneBuildTimer);
}

void USceneManagerSubsystem::InitializeScene()
{
	UPrintToolLibrary::Debug(FString("SceneManagerSubsystem::InitializeScene"));

	CommonInfo = Cast<ACommonInfo>
	(UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(), ACommonInfo::StaticClass(),
	                                                    FTransform(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	UGameplayStatics::FinishSpawningActor(CommonInfo, FTransform());

	CommonGizmoTransform = Cast<ACommonGizmoTransform>
	(UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(), ACommonGizmoTransform::StaticClass(),
	                                                    FTransform(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	UGameplayStatics::FinishSpawningActor(CommonGizmoTransform, FTransform());
}

void USceneManagerSubsystem::SetTransformMode(FGameplayTag InTransformMode, bool CombineTranslateRotate)
{
	if (CommonGizmoTransform)
	{
		CommonGizmoTransform->SetTransformMode(InTransformMode, CombineTranslateRotate);
	}
}

void USceneManagerSubsystem::SetAlignSpace(FGameplayTag InAlignSpace)
{
	if (CommonGizmoTransform)
	{
		CommonGizmoTransform->SetAlignSpace(InAlignSpace);
	}
}

void USceneManagerSubsystem::SpawnActor(TSoftClassPtr<AActor> actor)
{
	if (auto SpawnActor= actor.LoadSynchronous())
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

				// 定义碰撞参数
				FHitResult HitResult;
				FCollisionQueryParams CollisionParams;

				FTransform Transform;
				// 执行射线检测
				if (GetWorld()->LineTraceSingleByChannel(HitResult, RayStart, RayEnd, ECC_Visibility,CollisionParams))
				{
					Transform.SetLocation(HitResult.Location);
				}
				else
				{
					FVector EndLocation = RayStart + (WorldDirection * 500.0f);
					Transform.SetLocation(EndLocation);
					// 绘制调试射线
					if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
					{
						if ( Settings->bDrawDebug)
						{
							DrawDebugLine(GetWorld(), RayStart, EndLocation, FColor::Green, false, 2.0f, 0, 1.0f);
						}
					}
				
				}
				CurrentSpawningActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(),
													SpawnActor, Transform,
													ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				IDeviceInterface* CurrentSpawnActorInterface = Cast<IDeviceInterface>(CurrentSpawningActor);
				if (CurrentSpawnActorInterface)
				{
					CurrentSpawnActorInterface->GetOnActorInitializationCompletedDelegateChecked().AddDynamic(this
						,&ThisClass::OnActorInitializationCompleted);
				}
				UGameplayStatics::FinishSpawningActor(CurrentSpawningActor, Transform);
				// if (CurrentSpawnActorInterface)
				// {
				// 	CurrentSpawnActorInterface->GetOnActorInitializationCompletedDelegateChecked().Broadcast(this);
				// }
				//TODO 总体保存改成局部保存
				//SaveSubsystem->OnSceneSave.Broadcast();
			}
		}
	}
}

void USceneManagerSubsystem::SpawnActor(FEditItem ActorEditItem)
{
	this->SpawnActor(ActorEditItem.Actor.LoadSynchronous());
}

void USceneManagerSubsystem::UISectionChange(const FEditItem& myItem, bool bSelected)
{
	EditItem = myItem;
	bIsSelection = bSelected;
}

void USceneManagerSubsystem::ForceDevice(const FString& DeviceID)
{
	if (InteractiveSubsystem)
	{
		if (DeviceID.IsEmpty())
		{
			if (UserWidget && CurrentForceDevice)
			{
				UIManagerSubsystem->ReSetDeviceInfo(CurrentForceDeviceID,CurrentForceDevice,false,CommonToolTags::E3DUI_Type_DeviceInfoSelect,UserWidget);
			}
			InteractiveSubsystem->ForceDeviceSetMaterial(CurrentForceDevice,false);
			CurrentForceDevice = nullptr;
			CurrentForceDeviceID = "";
			OnAreaSet.Broadcast("");
		}
		else
		{
			if (StateSubsystem)
			{
				FGameplayTag GameplayTag = CommonToolTags::DeviceState_Null;
				StateSubsystem->GetDeviceState(GameplayTag);
				
				if (CurrentForceDevice == ComponentDeviceActorMap.FindRef(DeviceID))
				{
					if (GameplayTag == CommonToolTags::DeviceState_Null)
					{
						StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Ledger);
					}
					else
					{
						StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Null);
					}
				}
				else
				{
					if (CurrentForceDevice)
					{
						InteractiveSubsystem->ForceDeviceSetMaterial(CurrentForceDevice,false);
						StateSubsystem->ChangeDeviceState(CommonToolTags::DeviceState_Null);
						TArray<UActorComponent*> ActorComponents = CurrentForceDevice->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
						for (auto Component : ActorComponents)
						{
							Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(nullptr);
						}
					}
					CurrentForceDeviceID = DeviceID;
					
					CurrentForceDevice = ComponentDeviceActorMap.FindRef(DeviceID);
					if (UDeviceMarkComponent* DeviceMarkComponent = CurrentForceDevice->FindComponentByClass<UDeviceMarkComponent>())
					{
						OnAreaSet.Broadcast(DeviceMarkComponent->AreaSet);
					}
					else
					{
						OnAreaSet.Broadcast("");
					}
					InteractiveSubsystem->ForceDeviceSetMaterial(CurrentForceDevice,true);
					InteractiveSubsystem->ForceActor(CurrentForceDevice);
				}
			}
		}
	}
}

FString USceneManagerSubsystem::GetCurrentForceDeviceID()
{
	return CurrentForceDeviceID;
}

void USceneManagerSubsystem::SelectionActor(bool IsSelected, AActor* Actor)
{
	OnSelectionActor.Broadcast(IsSelected,Actor);
}

void USceneManagerSubsystem::InteractiveHit(FHitResult HitResult)
{
	if (bIsSelection)
	{
		if (CurrentSpawningActor)
		{
			if (IDeviceInterface* CurrentSpawnActorInterface = Cast<IDeviceInterface>(CurrentSpawningActor))
			{
				DrawDebugSphere(GetWorld(),HitResult.Location, 5.0f, 12, FColor::Green, false,5.0);
				CurrentSpawnActorInterface->Execute_AddHitData(CurrentSpawningActor,HitResult);
			}
		}
		else
		{
			SpawnActor(EditItem);
		}
	}
}

void USceneManagerSubsystem::DeInteractive()
{
	if (bIsSelection)
	{
		if (CurrentSpawningActor)
		{
			if (CurrentSpawningActor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
			{
				IDeviceInterface::ActorInitializationCompleted(CurrentSpawningActor);	
			}
		}
	}
}

bool USceneManagerSubsystem::SaveSceneHierarchyToJson(const FString& FileName)
{
	UWorld *World = GetWorld();
	if (!World) return false;
	//保存为当前文件
	if (FileName.IsEmpty())
	{
		return false;
	}
	// 创建根JSON对象
	TSharedPtr<FJsonObject> RootJsonObject = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> TopLevelActors;
	
	for (auto Item : ActorsMap)
	{
		if (!Item.Value->GetRootComponent()->GetAttachParent())
		{
			TSharedPtr<FJsonObject> ActorJson = SerializeActor(Item.Key);
			if (ActorJson.IsValid())
			{
				TopLevelActors.Add(MakeShared<FJsonValueObject>(ActorJson));
			}
		}
	}
	RootJsonObject->SetArrayField(TEXT("Actors"), TopLevelActors);
    
	// 将JSON转换为字符串
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), Writer);
    
	// 保存到文件
	UJsonToolLibrary::SaveJsonToFile(FileName,OutputString,"SceneData");
	return true;
}

TSharedPtr<FJsonObject> USceneManagerSubsystem::SerializeActor(const FString& GUID)
{
	AActor *Actor = ActorsMap.FindRef(GUID);
	if (!Actor) return nullptr;

	//保存基本属性,使用在序列化与反序列化
	TSharedPtr<FJsonObject> ActorJson = MakeShared<FJsonObject>();
	
	if (Actor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
	{
		ActorJson = Cast<IDeviceInterface>(Actor)->GetDeviceJsonObjectData(GUID);
		Cast<IDeviceInterface>(Actor)->SetDeviceJsonObjectData(ActorJson.ToSharedRef());
	}
	else
	{
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
		ActorJsonTransform->SetStringField(TEXT("AttributeValue"),Actor->GetActorTransform().ToString());
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
	}
	// 递归保存子Actor
	TArray<TSharedPtr<FJsonValue>> Children;
	GatherAttachedActors(Actor, Children);
	if (Children.Num() > 0)
	{
		ActorJson->SetArrayField(TEXT("Children"), Children);
	}
    
	return ActorJson;
}

void USceneManagerSubsystem::GatherAttachedActors(const AActor* Parent, TArray<TSharedPtr<FJsonValue>>& ChildrenArray)
{
	if (!Parent) return;
    
	TArray<AActor*> AttachedActors;
	Parent->GetAttachedActors(AttachedActors);
    
	for (AActor* Child : AttachedActors)
	{
		TSharedPtr<FJsonObject> ChildJson = SerializeActor(*ActorsMap.FindKey(Child));
		if (ChildJson.IsValid())
		{
			ChildrenArray.Add(MakeShared<FJsonValueObject>(ChildJson));
		}
	}
}

bool USceneManagerSubsystem::LoadSceneHierarchyFromJson(const FString& FileName)
{
	UWorld *World = GetWorld();
	if (!World) return false;
	
	//保存为当前文件
	if (FileName.IsEmpty())
	{
		return false;
	}

	FString JsonString;
	if (!UJsonToolLibrary::GetJsonString(FileName,JsonString,false,"SceneData"))
	{
		return false;
	}
	
	//序列化已保存的actor
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	// 创建 Json 对象
	TSharedPtr<FJsonObject> JsonObject;
	// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		const TArray<TSharedPtr<FJsonValue>>* ArrayValues;
		
		if (JsonObject->TryGetArrayField(TEXT("Actors"),ArrayValues))
		{
			for (const TSharedPtr<FJsonValue>& JsonValue : *ArrayValues)
			{
				if (TSharedPtr<FJsonObject> JsonObjectLocal = JsonValue->AsObject())
				{
					DeSerializeActor(JsonObjectLocal);
					
					DeSerializeWorldAssetTreeNode(JsonObjectLocal,nullptr);
				}
			}
		}
	}
	return true;
}

bool USceneManagerSubsystem::GetStringAttribute(const TSharedPtr<FJsonObject>& JsonObject,const FString& AttributeName, FString &AttributeValue)
{
	const TArray<TSharedPtr<FJsonValue>>* Attributes;
	if (JsonObject->TryGetArrayField(TEXT("Attribute"),Attributes))
	{
		for (const TSharedPtr<FJsonValue>& JsonValue : *Attributes)
		{
			if (TSharedPtr<FJsonObject> JsonObjectLocal = JsonValue->AsObject())
			{
				FString GetAttributeName;
				if (JsonObjectLocal->TryGetStringField(TEXT("AttributeName"),GetAttributeName))
				{
					if (AttributeName.Equals(GetAttributeName))
					{
						if (JsonObjectLocal->TryGetStringField(TEXT("AttributeValue"),AttributeValue))
						{
							return true;
						}
						return false;
					}
				}
			}
		}
	}
	return false;
}

bool USceneManagerSubsystem::GetBoolAttribute(const TSharedPtr<FJsonObject>& JsonObject, const FString& AttributeName,
	bool& AttributeValue)
{
	const TArray<TSharedPtr<FJsonValue>>* Attributes;
	if (JsonObject->TryGetArrayField(TEXT("Attribute"),Attributes))
	{
		for (const TSharedPtr<FJsonValue>& JsonValue : *Attributes)
		{
			if (TSharedPtr<FJsonObject> JsonObjectLocal = JsonValue->AsObject())
			{
				FString GetAttributeName;
				if (JsonObjectLocal->TryGetStringField(TEXT("AttributeName"),GetAttributeName))
				{
					if (AttributeName.Equals(GetAttributeName))
					{
						bool Value = false;
						if (JsonObjectLocal->TryGetBoolField(TEXT("AttributeValue"),Value))
						{
							AttributeValue = Value;
							return true;
						}
						return false;
					}
				}
			}
		}
	}
	return false;
}

bool USceneManagerSubsystem::GetActorAttributes(const FString& SceneName,const FString& GUID, TArray<FString>& Attributes)
{
	TSharedPtr<FJsonObject> RootJsonObject;
	FString TargetSceneData;
	//判断是否已经有场景数据
	if (UJsonToolLibrary::GetJsonString(SceneName,TargetSceneData,false,"SceneData"))
	{
		//序列化已保存的actor
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TargetSceneData);
		// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
		if (FJsonSerializer::Deserialize(JsonReader, RootJsonObject))
		{
			const TArray<TSharedPtr<FJsonValue>>* ArrayValues;

			if (RootJsonObject->TryGetArrayField(TEXT("Actors"),ArrayValues))
			{
				for (auto  JsonValue : *ArrayValues)
				{
					FString LocalGUID;
					JsonValue->AsObject()->TryGetStringField(TEXT("GUID"),LocalGUID);
					if (LocalGUID.Equals(GUID))
					{
						const TArray<TSharedPtr<FJsonValue>>* AttributeObjects;
						if (JsonValue->AsObject()->TryGetArrayField(TEXT("Attribute"),AttributeObjects))
						{
							for (auto AttributeObject : *AttributeObjects)
							{
								FString Attribute;
								UJsonToolLibrary::GetJsonStringFromJsonObject(AttributeObject->AsObject(),Attribute);
								Attributes.AddUnique(Attribute);
							}
						}
						return true;
					}
					//拥有guid才递归在子类中寻找
					TArray<TSharedPtr<FJsonValue>> ChildrenArray = JsonValue->AsObject()->GetArrayField(TEXT("Children"));
					if (ChildrenArray.Num()>0)
					{
						for (auto Children : ChildrenArray)
						{
							TSharedPtr<FJsonObject> JsonObjectLocal = Children->AsObject();

							if (GetActorAttributes(SceneName,GUID,Attributes))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool USceneManagerSubsystem::GetActorStatic(FString SceneName, const FString& GUID)
{
	if (SceneName.IsEmpty())
	{
		SceneName = CurrentSceneName;
	}
	TSharedPtr<FJsonObject> RootJsonObject;
	FString TargetSceneData;
	//判断是否已经有场景数据
	if (UJsonToolLibrary::GetJsonString(SceneName,TargetSceneData,false,"SceneData"))
	{
		//序列化已保存的actor
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TargetSceneData);
		// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
		if (FJsonSerializer::Deserialize(JsonReader, RootJsonObject))
		{
			const TArray<TSharedPtr<FJsonValue>>* ArrayValues;

			if (RootJsonObject->TryGetArrayField(TEXT("Actors"),ArrayValues))
			{
				for (auto  JsonValue : *ArrayValues)
				{
					FString LocalGUID;
					JsonValue->AsObject()->TryGetStringField(TEXT("GUID"),LocalGUID);
					if (LocalGUID.Equals(GUID))
					{
						bool bStatic = false;
						if (JsonValue->AsObject()->TryGetBoolField(TEXT("bStatic"),bStatic))
						{
							return true;
						}
						return false;
					}
					//拥有guid才递归在子类中寻找
					TArray<TSharedPtr<FJsonValue>> ChildrenArray = JsonValue->AsObject()->GetArrayField(TEXT("Children"));
					if (ChildrenArray.Num()>0)
					{
						for (auto Children : ChildrenArray)
						{
							TSharedPtr<FJsonObject> JsonObjectLocal = Children->AsObject();

							if (GetActorStatic(SceneName,GUID))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool USceneManagerSubsystem::MakeAttribute(const FString& AttributeName, const FString& AttributeValue,
                                           FString& ResultAttribute)
{
	TSharedPtr<FJsonObject> NewJsonObject = MakeShared<FJsonObject>();
	NewJsonObject->SetStringField(TEXT("AttributeName"), AttributeName);
	NewJsonObject->SetStringField(TEXT("AttributeValue"), AttributeValue);
	NewJsonObject->SetStringField(TEXT("AttributeType"), "EditType.Normal.Dynamic");
	NewJsonObject->SetBoolField(TEXT("bDisplay"), true);
	NewJsonObject->SetBoolField(TEXT("bCanEditor"), true);
	UJsonToolLibrary::GetJsonStringFromJsonObject(NewJsonObject,ResultAttribute);
	return true;
}

AActor* USceneManagerSubsystem::DeSerializeActor(const TSharedPtr<FJsonObject>& JsonObject,AActor* Parent)
{
	//预先判断是否已经是场景中存在的actor
	bool bStatic = false;
	JsonObject->TryGetBoolField(TEXT("bStatic"),bStatic);
	if (bStatic)
	{
		FString StaticActorName;
		JsonObject->TryGetStringField(TEXT("StaticActorName"),StaticActorName);
		if (StaticDeviceActorMap.Find(StaticActorName))
		{
			AActor* Actor = *StaticDeviceActorMap.Find(StaticActorName);
			TArray<TSharedPtr<FJsonValue>> ChildrenArray = JsonObject->GetArrayField(TEXT("Children"));
			if (ChildrenArray.Num()>0)
			{
				SatherAttachedActors(ChildrenArray,Actor);
			}
			FString GUID;
			JsonObject->TryGetStringField(TEXT("GUID"),GUID);
			RegisterActor(GUID,Actor);
			return Actor;
		}
	}
	FString ActorPath;
	FString ActorTransform;
	if (!GetStringAttribute(JsonObject,TEXT("ActorClass"),ActorPath)||!GetStringAttribute(JsonObject,TEXT("ActorTransform"),ActorTransform))
	{
		return nullptr;
	}
	
	FTransform Transform;
	Transform.InitFromString(ActorTransform);

	if (Parent)
	{
		Transform = UKismetMathLibrary::MakeRelativeTransform(Transform,Parent->GetTransform());
	}
	
	FSoftObjectPath SoftObjectPath(ActorPath);
	// 检查路径是否有效
	if (SoftObjectPath.IsValid())
	{
		if (auto SpawnActor  = TSoftClassPtr<UObject>(SoftObjectPath).LoadSynchronous())
		{
			AActor * SpawndActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(),
										SpawnActor, Transform,
										ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			if (IDeviceInterface* CurrentSpawnActorInterface = Cast<IDeviceInterface>(SpawndActor))
			{
				CurrentSpawnActorInterface->SetDeviceJsonObjectData(JsonObject.ToSharedRef());
				UGameplayStatics::FinishSpawningActor(SpawndActor, Transform);
			}
			
			SpawndActor->SetActorRelativeTransform(Transform);
			TArray<TSharedPtr<FJsonValue>> ChildrenArray = JsonObject->GetArrayField(TEXT("Children"));
			if (ChildrenArray.Num()>0)
			{
				SatherAttachedActors(ChildrenArray,SpawndActor);
			}
			FString GUID;
			JsonObject->TryGetStringField(TEXT("GUID"),GUID);
			RegisterActor(GUID,SpawndActor);
			
			return SpawndActor;
		}
		else
		{
			return nullptr;
		}
	}
	return nullptr;
}

void USceneManagerSubsystem::SatherAttachedActors(TArray<TSharedPtr<FJsonValue>> ChildrenArray,AActor* Parent)
{
	if (!Parent) return;

	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules( EAttachmentRule::KeepRelative, true );
	for (auto Child : ChildrenArray)
	{
		AActor* actor = DeSerializeActor(Child->AsObject(),Parent);
		actor->AttachToActor(Parent,AttachmentTransformRules);
	}
}

bool USceneManagerSubsystem::UnLoadSceneHierarchyFromJson(const FString& FileName)
{
	UWorld *World = GetWorld();
	if (!World) return false;
	
	//保存为当前文件
	if (FileName.IsEmpty())
	{
		return false;
	}
	TArray<FString> Keys;
	ActorsMap.GetKeys(Keys);
	if (!Keys.IsEmpty())
	{
		for (auto Element : ActorsMap)
		{
			if (!GetActorStatic(CurrentSceneName,Element.Key))
			{
				Element.Value->Destroy();
			}
		}
		WorldAssetTreeNodes.Empty();
	}
	
	return true;
}

void USceneManagerSubsystem::OnActorInitializationCompletedDelegateCheckedDelay(
	AActor * Actor)
{
	if (IDeviceInterface* DeviceInterface = Cast<IDeviceInterface>(Actor))
	{
		DeviceInterface->ActorInitializationCompleted(Actor);
	}
}

void USceneManagerSubsystem::UpdateWorldHierarchyToJson(const FString& FileName)
{
	WorldAssetTreeNodes.Empty();
	FString JsonString;
	UJsonToolLibrary::GetJsonString(FileName,JsonString,false,"SceneData");
	
	//序列化已保存的actor
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	// 创建 Json 对象
	TSharedPtr<FJsonObject> JsonObject;
	// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		const TArray<TSharedPtr<FJsonValue>>* ArrayValues;
		if (JsonObject->TryGetArrayField(TEXT("Actors"),ArrayValues))
		{
			for (const TSharedPtr<FJsonValue>& JsonValue : *ArrayValues)
			{
				if (TSharedPtr<FJsonObject> JsonObjectLocal = JsonValue->AsObject())
				{
					DeSerializeWorldAssetTreeNode(JsonObjectLocal);
				}
			}
		}
	}
	OnWorldAssetTreeNodesUpdate.Broadcast(WorldAssetTreeNodes);
}

UWorldAssetTreeNode* USceneManagerSubsystem::DeSerializeWorldAssetTreeNode(const TSharedPtr<FJsonObject>& JsonObject,
                                                                           UWorldAssetTreeNode* Parent,int32 layer)
{
	UWorldAssetTreeNode* TreeNode = NewObject<UWorldAssetTreeNode>();
	TreeNode->Init(JsonObject);
	TreeNode->Layer = layer;
	TreeNode->Parent = Parent;
	FString GUID = "";
	JsonObject->TryGetStringField(TEXT("GUID"),GUID);
	if (ActorsMap.Find(GUID))
	{
		TreeNode->ActorPtr = *ActorsMap.Find(GUID);
	}
	
	TArray<TSharedPtr<FJsonValue>> ChildrenArray = JsonObject->GetArrayField(TEXT("Children"));
	if (ChildrenArray.Num()>0)
	{
		SatherAttachedWorldAssetTreeNodes(ChildrenArray,TreeNode);
	}
	if (Parent==nullptr)
	{
		WorldAssetTreeNodes.AddUnique(TreeNode);
	}
	return TreeNode;
}

void USceneManagerSubsystem::SatherAttachedWorldAssetTreeNodes(TArray<TSharedPtr<FJsonValue>> ChildrenArray,
	UWorldAssetTreeNode* Parent)
{
	if (!Parent) return;
	for (auto Child : ChildrenArray)
	{
		UWorldAssetTreeNode* ChildNode = DeSerializeWorldAssetTreeNode(Child->AsObject(),Parent,Parent->Layer+1);
		Parent->Children.Add(ChildNode);
	}
}

bool USceneManagerSubsystem::GetActorGUID(AActor* actor,FString& GUID)
{
	if (ActorsMap.FindKey(actor))
	{
		GUID =*ActorsMap.FindKey(actor);
		return true;
	}
	return false;
}

void USceneManagerSubsystem::AddWorldAssetTreeNode(UWorldAssetTreeNode* TreeNode,UWorldAssetTreeNode* NewParentTreeNode)
{
	TreeNode->RemoveFromParent();
	if (NewParentTreeNode)
	{
		NewParentTreeNode->AddChild(TreeNode);
		if (NewParentTreeNode->ActorPtr && TreeNode->ActorPtr)
		{
			TreeNode->ActorPtr->AttachToActor(NewParentTreeNode->ActorPtr,FAttachmentTransformRules( EAttachmentRule::KeepWorld, true ));
		}
	}
	else
	{
		WorldAssetTreeNodes.AddUnique(TreeNode);
		if (TreeNode->ActorPtr && TreeNode->ActorPtr->GetAttachParentActor() )
		{
			TreeNode->ActorPtr->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld,true));
		}
	}
	SaveSubsystem->OnSceneSave.Broadcast();
}

void USceneManagerSubsystem::InitSceneAllDevice()
{
	ComponentDeviceActorMap.Empty();
	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (UDeviceMarkComponent* DeviceMarkComponent = Actor->FindComponentByClass<UDeviceMarkComponent>())
		{
			if (!DeviceMarkComponent->DeviceID.IsEmpty())
			{
				ComponentDeviceActorMap.Add(DeviceMarkComponent->DeviceID,Actor);
			}
		}
	}
}

void USceneManagerSubsystem::OnTrainStationSelectedComplete()
{
	StaticDeviceActorMap.Empty();
	ComponentDeviceActorMap.Empty();
	PositioningsMap.Empty();
	MarkTypeMap.Empty();
	
	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		// 处理 Actor
		StaticDeviceActorMap.Add(Actor->GetName(),Actor);
		
		if (UDeviceMarkComponent* DeviceMarkComponent = Actor->FindComponentByClass<UDeviceMarkComponent>())
		{
			if (!DeviceMarkComponent->DeviceID.IsEmpty())
			{
				ComponentDeviceActorMap.Add(DeviceMarkComponent->DeviceID,Actor);
			}

			//用于筛选的模型透明效果
			TArray<FGameplayTag> TypeTags;
			DeviceMarkComponent->ActorMarkTypes.GetGameplayTagArray(TypeTags);

			for (auto TypeTag : TypeTags)
			{
				if (MarkTypeMap.Contains(TypeTag))
				{
					TArray<AActor*> MarkTypeActors = MarkTypeMap.FindRef(TypeTag);
					TArray<AActor*> ChildMarkTypeActors;
					Actor->GetAllChildActors(ChildMarkTypeActors);
					
					MarkTypeActors.AddUnique(Actor);
					for (AActor* ChildMarkTypeActor : ChildMarkTypeActors)
					{
						MarkTypeActors.AddUnique(ChildMarkTypeActor);
					}
					Actor->GetAttachedActors(ChildMarkTypeActors);
					for (AActor* ChildMarkTypeActor : ChildMarkTypeActors)
					{
						MarkTypeActors.AddUnique(ChildMarkTypeActor);
					}
					MarkTypeMap.Add(TypeTag,MarkTypeActors);
				}
				else
				{
					TArray<AActor*> MarkTypeActors;
					MarkTypeActors.AddUnique(Actor);
					
					TArray<AActor*> ChildMarkTypeActors;
					Actor->GetAllChildActors(ChildMarkTypeActors);
					for (AActor* ChildMarkTypeActor : ChildMarkTypeActors)
					{
						MarkTypeActors.AddUnique(ChildMarkTypeActor);
					}
					Actor->GetAttachedActors(ChildMarkTypeActors);
					for (AActor* ChildMarkTypeActor : ChildMarkTypeActors)
					{
						MarkTypeActors.AddUnique(ChildMarkTypeActor);
					}
					MarkTypeMap.Add(TypeTag,MarkTypeActors);
				}
			}
		}
		if (APositioning* Positioning = Cast<APositioning>(Actor))
		{
			if (!Positioning->LocationName.IsEmpty())
			{
				PositioningsMap.Add(Positioning->LocationName,Positioning);
			}
		}
	}
	
	LoadSceneHierarchyFromJson(CurrentSceneName);
}

void USceneManagerSubsystem::OnDeviceInfoPoolRefresh()
{
	if (StateSubsystem)
	{
		FGameplayTag SystemState;
		StateSubsystem->GetSystemState(SystemState);

		if (SystemState == CommonToolTags::State_Inspection)
		{
			if (DeviceManager)
			{
				TArray<FString> DeviceKeys;
				//DeviceManager->GetDeviceListID(DeviceKeys);
				DeviceManager->GetSceneAllDevice(DeviceKeys);
				ReSetDeviceInfo(DeviceKeys);
			}
		}
	}
}

void USceneManagerSubsystem::ReSetDeviceInfo(const TArray<FString>& DeviceInfos)
{
	if (DeviceManager)
	{
		for (auto LastComponentDeviceActorKey : LastComponentDeviceActorKeys)
		{
			DeviceManager->PushDeviceInfoPool(LastComponentDeviceActorKey,ComponentDeviceActorMap.FindRef(LastComponentDeviceActorKey));
		}
		for (auto CurrentComponentDeviceActorKey : DeviceInfos)
		{
					
			DeviceManager->PopDeviceInfoPool(CurrentComponentDeviceActorKey,ComponentDeviceActorMap.FindRef(CurrentComponentDeviceActorKey));
		}
	}
	LastComponentDeviceActorKeys = DeviceInfos;
}

void USceneManagerSubsystem::MoveToPosition(const FString& PositionName)
{
	if (InteractiveSubsystem)
	{
		InteractiveSubsystem->CleanCurrentInteractiveActor();
	}
	if (AActor* Actor = PositioningsMap.FindRef(PositionName))
	{
		if (StateSubsystem)
		{
			StateSubsystem->ChangeUserState(CommonToolTags::UserState_FirstPerson, Actor->GetActorTransform(), 1);
		}
	}
}

void USceneManagerSubsystem::GetPositionings(TArray<FString>& Positionings)
{
	PositioningsMap.GetKeys(Positionings);
}

void USceneManagerSubsystem::GetAllMarkTypes(TArray<FGameplayTag>& MarkTypes)
{
	MarkTypeMap.GetKeys(MarkTypes);
}

void USceneManagerSubsystem::SetMarkType(const FGameplayTag& MarkType, bool bHideOtherActor ,const bool& ForceReset)
{
	if (CurrentMarkType==MarkType&&!ForceReset)
	{
		return;
	}
	CurrentMarkType = MarkType;

	if (MarkType==CommonToolTags::Actor_Type_Null)
	{
		for (auto Element : LastMarkActorsMaterialMap)
		{
			for (int32 i = 0; i< Element.Key->GetMaterials().Num();i++)
			{
				Element.Key->SetMaterial(i,Element.Value[i]);
			}
		}
		LastMarkActorsMaterialMap.Empty();
		LastMarkActors.Empty();
	}
	else
	{
		if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
		{
		
			if (UMaterialInterface* TransparentMaterial = Settings->TransparentMaterial.LoadSynchronous())
			{
				UMaterialInstanceDynamic* MaterialInstanceDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this,TransparentMaterial);
				MaterialInstanceDynamic->SetScalarParameterValue(FName(TEXT("opaque")),CurrentMarkScale);
				// UMateriaL
				// TransparentMaterial->set
				for (auto Element : LastMarkActorsMaterialMap)
				{
					for (int32 i = 0; i< Element.Key->GetMaterials().Num();i++)
					{
						Element.Key->SetMaterial(i,Element.Value[i]);
					}
				}
				
				LastMarkActorsMaterialMap.Empty();
				LastMarkActors.Empty();
				
				GetMarkTypeActors(MarkType,LastMarkActors);

				for (auto Element : LastMarkActors)
				{
					TArray<UStaticMeshComponent*> ComponentArray;
					Element->GetComponents(ComponentArray);
					for (UStaticMeshComponent* Component : ComponentArray)
					{
						TArray<UMaterialInterface*> Materias = Component->GetMaterials();
						LastMarkActorsMaterialMap.Add(Component,Materias);
						for (int32 i = 0; i < Materias.Num(); i++)
						{
							Component->SetMaterial(i,MaterialInstanceDynamic);
						}
					}
				}
			}
		}		
	}
	
	if (bHideOtherActor)
	{
		
	}
}

void USceneManagerSubsystem::GetCurrentMarkType(FGameplayTag& MarkType)
{
	MarkType = CurrentMarkType;
}

void USceneManagerSubsystem::GetMarkTypeActors(const FGameplayTag& MarkType,TArray<AActor*>& Actors)
{
	Actors = MarkTypeMap.FindRef(MarkType);
}

float USceneManagerSubsystem::GetCurrentMarkScale()
{
	return CurrentMarkScale;
}

void USceneManagerSubsystem::SetCurrentMarkScale(float currentMarkScale)
{
	CurrentMarkScale = currentMarkScale;
}

void USceneManagerSubsystem::GetSceneAllDevice(TArray<FString>& ResultList)
{
	ComponentDeviceActorMap.GetKeys(ResultList);
}

void USceneManagerSubsystem::FindSceneDevice(const FString& DeviceID, AActor*& Actor)
{
	Actor = *ComponentDeviceActorMap.Find(DeviceID);
}

void USceneManagerSubsystem::OnSave()
{
	//ToDo 改到场景中
	if (SaveSceneHierarchyToJson(CurrentSceneName))
	{
		UpdateWorldHierarchyToJson(CurrentSceneName);
	}
}

void USceneManagerSubsystem::OnSaveActor(const FString& GUID,const TArray<FString>& Attributes)
{
	if (!GUID.IsEmpty())
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		
		if (SaveActorJson(GUID,Attributes,JsonObject))
		{
			TSharedPtr<FJsonObject> RootJsonObject;
			TArray<TSharedPtr<FJsonValue>> TopLevelActors;
	
			FString TargetSceneData;
			//判断是否已经有场景数据
			if (UJsonToolLibrary::GetJsonString(CurrentSceneName,TargetSceneData,false,"SceneData"))
			{
				//序列化已保存的actor
				TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TargetSceneData);
				// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
				if (FJsonSerializer::Deserialize(JsonReader, RootJsonObject))
				{
					const TArray<TSharedPtr<FJsonValue>>* ArrayValues;

					if (RootJsonObject->TryGetArrayField(TEXT("Actors"),ArrayValues))
					{
						bool bFindActors = false;
						for (auto  JsonValue : *ArrayValues)
						{
							FString LocalGUID;
							JsonValue->AsObject()->TryGetStringField(TEXT("GUID"),LocalGUID);
							if (LocalGUID.Equals(GUID))
							{
								//找到需要里面有数据,不然就是删除
								bFindActors = true;
							}
							else
							{
								TopLevelActors.AddUnique(MakeShared<FJsonValueObject>(JsonValue->AsObject()));
							}
						}
						if (bFindActors)
						{
							if (Attributes.IsEmpty())
							{
								ActorsMap.Remove(GUID);
							}
							else
							{
								TopLevelActors.AddUnique(MakeShared<FJsonValueObject>(JsonObject));
							}
						}
						else
						{
							TopLevelActors.AddUnique(MakeShared<FJsonValueObject>(JsonObject));
						}
						RootJsonObject->SetArrayField(TEXT("Actors"), TopLevelActors);
					}
				}
			}else
			{
				//在该场景没有数据的情况下
				RootJsonObject = MakeShared<FJsonObject>();
				if (Attributes.IsEmpty())
				{
					ActorsMap.Remove(GUID);
				}
				else
				{
					TopLevelActors.Add(MakeShared<FJsonValueObject>(JsonObject));
				}
				RootJsonObject->SetArrayField(TEXT("Actors"), TopLevelActors);
			}

			AActor* Actor = ActorsMap.FindRef(GUID);
			if (Actor)
			{
				if (Actor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
				{
					Cast<IDeviceInterface>(Actor)->SetDeviceJsonObjectData(JsonObject.ToSharedRef());
				}
			}
			
			// 将JSON转换为字符串
			FString OutputString;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), Writer);
			// 保存到文件
			UJsonToolLibrary::SaveJsonToFile(CurrentSceneName,OutputString,"SceneData");
			//更新场景资源树
			UpdateWorldHierarchyToJson(CurrentSceneName);
		}
	}
	else
	{
		UPrintToolLibrary::Error(FString("OnSaveActor Erro"));
	}
}

void USceneManagerSubsystem::SaveActorAttributes(const FString& GUID, const TArray<FString>& Attributes, const FOnSaveActorAttributes& OnSaveActorAttributes)
{
	TSharedPtr<FJsonObject> RootJsonObject;
	TArray<TSharedPtr<FJsonValue>> TopLevelActors;
	
	FString TargetSceneData;
	//判断是否已经有场景数据
	if (UJsonToolLibrary::GetJsonString(CurrentSceneName,TargetSceneData,false,"SceneData"))
	{
		//序列化已保存的actor
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TargetSceneData);
		// 反序列化，将 JsonReader 里的数据，传入 JsonObject 中
		if (FJsonSerializer::Deserialize(JsonReader, RootJsonObject))
		{
			const TArray<TSharedPtr<FJsonValue>>* ArrayValues;

			if (RootJsonObject->TryGetArrayField(TEXT("Actors"),ArrayValues))
			{
				bool bFindActors = false;
				TSharedPtr<FJsonObject> JsonObjectLocal;
				for (auto  JsonValue : *ArrayValues)
				{
					FString LocalGUID;
					JsonValue->AsObject()->TryGetStringField(TEXT("GUID"),LocalGUID);
					if (LocalGUID.Equals(GUID))
					{
						bFindActors = true;
						JsonObjectLocal = JsonValue->AsObject();
						SaveActorJson(GUID,Attributes,JsonObjectLocal);
					}
					else
					{
						TopLevelActors.AddUnique(MakeShared<FJsonValueObject>( JsonValue->AsObject()));
					}
				}
				
				if (bFindActors)
				{
					if (Attributes.IsEmpty())
					{
						ActorsMap.Remove(GUID);
					}
					else
					{
						TopLevelActors.AddUnique(MakeShared<FJsonValueObject>(JsonObjectLocal));
						AActor* Actor = ActorsMap.FindRef(GUID);
						if (Actor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
						{
							Cast<IDeviceInterface>(Actor)->SetDeviceJsonObjectData(JsonObjectLocal.ToSharedRef());
						}
					}
				}
				RootJsonObject->SetArrayField(TEXT("Actors"), TopLevelActors);
				// 将JSON转换为字符串
				FString OutputString;
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
				FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), Writer);
				// 保存到文件
				UJsonToolLibrary::SaveJsonToFile(CurrentSceneName,OutputString,"SceneData");
				//更新场景资源树
				UpdateWorldHierarchyToJson(CurrentSceneName);

				if (OnSaveActorAttributes.IsBound())
				{
					OnSaveActorAttributes.Execute(GUID);
				}
			}
		}
	}
}

bool USceneManagerSubsystem::SaveActorJson(const FString& GUID,const TArray<FString>& Attributes,TSharedPtr<FJsonObject>& JsonObject)
{
	if (AActor* Actor = ActorsMap.FindRef(GUID))
	{
		if (Attributes.IsEmpty())
		{
			if (Actor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
			{
				JsonObject = Cast<IDeviceInterface>(Actor)->GetDeviceJsonObjectData(GUID);
			}
			else
			{
				JsonObject = MakeShared<FJsonObject>();
			}
			return true;
		}
		
		FString TempGUID;

		//如果获取到GUID
		if (JsonObject->TryGetStringField(TEXT("GUID"),TempGUID))
		{
			//如果不等于 会递归在子类中寻找
			if (TempGUID.Equals(GUID))
			{
				//需要修改的JsonObj
				FString DataJsonString;
				UJsonToolLibrary::GetJsonStringFromJsonObject(JsonObject,DataJsonString);

				//在属性中查找需要修改的属性
				TArray<FString> AttributeArray;
				UJsonToolLibrary::GetJsonStringArrayFromJsonString(DataJsonString,TEXT("Attribute"),AttributeArray);
				TArray<TSharedPtr<FJsonValue>> ResultAttribute;
				for (auto Attribute : Attributes)
				{
					TSharedPtr<FJsonObject> NewJsonObject = MakeShared<FJsonObject>();
					
					FString AttributeName;
					UJsonToolLibrary::GetStringFromJsonString(Attribute,TEXT("AttributeName"),AttributeName);
					NewJsonObject->SetStringField(TEXT("AttributeName"), AttributeName);
					
					FString AttributeValue;
					UJsonToolLibrary::GetStringFromJsonString(Attribute,TEXT("AttributeValue"),AttributeValue);
					NewJsonObject->SetStringField(TEXT("AttributeValue"), AttributeValue);
						
					FString AttributeType;
					UJsonToolLibrary::GetStringFromJsonString(Attribute,TEXT("AttributeType"),AttributeType);
					NewJsonObject->SetStringField(TEXT("AttributeType"), AttributeType);

					bool bDisplay;
					UJsonToolLibrary::GetBoolFromJsonString(Attribute,TEXT("bDisplay"),bDisplay);
					NewJsonObject->SetBoolField(TEXT("bDisplay"), true);

					bool bCanEditor;
					UJsonToolLibrary::GetBoolFromJsonString(Attribute,TEXT("bCanEditor"),bCanEditor);
					NewJsonObject->SetBoolField(TEXT("bCanEditor"), bCanEditor);
					ResultAttribute.Add(MakeShared<FJsonValueObject>(NewJsonObject));
				}
				JsonObject->SetArrayField(TEXT("Attribute"),ResultAttribute);
				
				return true;
			}
			//拥有guid才递归在子类中寻找
			TArray<TSharedPtr<FJsonValue>> ChildrenArray = JsonObject->GetArrayField(TEXT("Children"));
			if (ChildrenArray.Num()>0)
			{
				for (auto Children : ChildrenArray)
				{
					TSharedPtr<FJsonObject> JsonObjectLocal = Children->AsObject();
				
					if (SaveActorJson(GUID,Attributes,JsonObjectLocal))
					{
						return true;
					}
				}
			}
		}
		//没获取到
		else
		{
			TArray<TSharedPtr<FJsonValue>> ResultAttribute;
			for (auto Attribute : Attributes)
			{
				TSharedPtr<FJsonObject> NewJsonObject = MakeShared<FJsonObject>();
					
				FString AttributeName;
				UJsonToolLibrary::GetStringFromJsonString(Attribute,TEXT("AttributeName"),AttributeName);
				NewJsonObject->SetStringField(TEXT("AttributeName"), AttributeName);
					
				FString AttributeValue;
				UJsonToolLibrary::GetStringFromJsonString(Attribute,TEXT("AttributeValue"),AttributeValue);
				NewJsonObject->SetStringField(TEXT("AttributeValue"), AttributeValue);
						
				FString AttributeType;
				UJsonToolLibrary::GetStringFromJsonString(Attribute,TEXT("AttributeType"),AttributeType);
				NewJsonObject->SetStringField(TEXT("AttributeType"), AttributeType);

				bool bDisplay;
				UJsonToolLibrary::GetBoolFromJsonString(Attribute,TEXT("bDisplay"),bDisplay);
				NewJsonObject->SetBoolField(TEXT("bDisplay"), true);

				bool bCanEditor;
				UJsonToolLibrary::GetBoolFromJsonString(Attribute,TEXT("bCanEditor"),bCanEditor);
				NewJsonObject->SetBoolField(TEXT("bCanEditor"), bCanEditor);
				ResultAttribute.Add(MakeShared<FJsonValueObject>(NewJsonObject));
			}
			JsonObject->SetStringField(TEXT("GUID"), GUID);
			JsonObject->SetStringField(TEXT("StaticActorName"),Actor->GetName());
			JsonObject->SetBoolField(TEXT("bStatic"), true);
			JsonObject->SetArrayField(TEXT("Attribute"),ResultAttribute);
			return true;
		}
	}
	
	return false;
}

void USceneManagerSubsystem::OnActorInitializationCompleted(UObject* Object)
{
	IDeviceInterface* CurrentSpawnActorInterface = Cast<IDeviceInterface>(CurrentSpawningActor);
	if (CurrentSpawnActorInterface)
	{
		CurrentSpawnActorInterface->GetOnActorInitializationCompletedDelegateChecked().RemoveDynamic(this
			,&ThisClass::OnActorInitializationCompleted);
	}
	FString GUID;
	SaveNewActorToJson(CurrentSpawningActor,GUID,TArray<FString>());
	
	if (InteractiveSubsystem)
	{
		InteractiveSubsystem->SetInteractiveActor(CurrentSpawningActor);
		
		CurrentSpawningActor = nullptr;
	}
}

void USceneManagerSubsystem::SaveNewActorToJson(AActor* actor,FString& GUID,const TArray<FString>& Attributes)
{
	if (SaveSubsystem)
	{
		RegisterActor(GUID,actor,true);
		SaveSubsystem->OnSceneSaveActor.Broadcast(GUID,Attributes);
	}
}

void USceneManagerSubsystem::AddInspectionActor(AActor* Actor)
{
	bool bInspection = false;
	//GetBoolAttribute(JsonObject,TEXT(""),bInspection);
	if (bInspection)
	{
		ViewActors.AddUnique(Actor);
	}
}

void USceneManagerSubsystem::RemoveInspectionActor(AActor* Actor)
{
	if (ViewActors.Contains(Actor))
	{
		ViewActors.Remove(Actor);
	}
}

void USceneManagerSubsystem::StartInspection(bool bReStart)
{
	bInspectionStarted = true;
	InspectionCheckSignature.Broadcast(bInspectionStarted);
	//是否重新开始检视
	if (bReStart)
	{
		InspectionIndex = 0;
	}
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("InspectionIng"),InspectionIndex);
	GetWorld()->GetTimerManager().SetTimer(InspectionTimerHandle,TimerDelegate,InspectionRate,true);
}

void USceneManagerSubsystem::InspectionIng()
{
	if (ViewActors.IsEmpty())
	{
		UPrintToolLibrary::Waring("ViewActors is Empty!");
	}
	else
	{
		if (InspectionIndex<ViewActors.Num() && InteractiveSubsystem)
		{
			if (ViewActors.IsValidIndex(InspectionIndex))
			{
				InteractiveSubsystem->InteractiveOnClickedSignature.Broadcast(ViewActors[InspectionIndex]);
			}
			InspectionIndex++;
		}
		else
		{
			InspectionIndex = 0;
			InteractiveSubsystem->InteractiveOnClickedSignature.Broadcast(ViewActors[InspectionIndex]);
			InspectionIndex++;
		}
	}
}

void USceneManagerSubsystem::StopInspection()
{
	bInspectionStarted = false;
	GetWorld()->GetTimerManager().ClearTimer(InspectionTimerHandle);
	InspectionCheckSignature.Broadcast(bInspectionStarted);
}

void USceneManagerSubsystem::RegisterActor(FString& GUID,AActor* Actor,bool bRegenerate)
{
	if (bRegenerate)
	{
		GUID = FGuid::NewGuid().ToString();
	}
	ActorsMap.Add(GUID,Actor);
}

void USceneManagerSubsystem::UnRegisterActor(const FString& GUID)
{
	ActorsMap.Remove(GUID);
}