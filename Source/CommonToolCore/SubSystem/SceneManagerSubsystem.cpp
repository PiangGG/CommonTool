// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneManagerSubsystem.h"

#include "AttributeToolLibrary.h"
#include "Cesium3DTileset.h"
#include "LidarPointCloud.h"
#include "CommonToolCoreTags.h"
#include "DeviceInterface.h"
#include "DeviceManager.h"
#include "InteractiveSubsystem.h"
#include "LoadingSubsystem.h"
#include "SaveSubsystem.h"
#include "StateSubsystem.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "CommonToolCore/Actor/Positioning.h"
#include "CommonToolCore/Actor/Scene/ComDatasmithActor.h"
#include "CommonToolCore/Actor/Scene/ComLidarPointCloudActor.h"
#include "CommonToolCore/Actor/Tool/CommonInfo.h"
#include "CommonToolCore/Components/DeviceMarkComponent.h"
#include "CommonToolCore/Gameplay/CommonCharacter.h"
#include "CommonSettings.h"
#include "JsonToolLibrary.h"
#include "PrintToolLibrary.h"
#include "CommonToolCore/Object/SceneTreeNode.h"
#include "CommonToolCore/Object/WorldAssetTreeNode.h"
#include "Engine/AssetManager.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"

USceneManagerSubsystem::USceneManagerSubsystem():
StateSubsystem(nullptr),
SaveSubsystem(nullptr),
InteractiveSubsystem(nullptr),
DeviceManager(nullptr),
CommonInfo(nullptr),
SceneTreeNode(nullptr)
{
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

void USceneManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	UPrintToolLibrary::Debug(FString("SceneManagerSubsystem::OnWorldBeginPlay"));

	OnSceneChangeSignature.AddDynamic(this,&ThisClass::OnSceneChange);
	OnSceneChangeCompleteSignature.AddDynamic(this,&ThisClass::OnSceneChangeComplete);
	OnCheckLevelChangeSignature.AddDynamic(this,&ThisClass::OnCheckLevelChange);
	
	StateSubsystem = InWorld.GetGameInstance()->GetSubsystem<UStateSubsystem>();
	if (StateSubsystem)
	{
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);
	}
	
	InteractiveSubsystem = UWorld::GetSubsystem<UInteractiveSubsystem>(GWorld);
	if (InteractiveSubsystem)
	{
		//InteractiveSubsystem->OnUISectionChange.AddDynamic(this, &ThisClass::UISectionChange);
	}
	
	SaveSubsystem = InWorld.GetGameInstance()->GetSubsystem<USaveSubsystem>();
	if (SaveSubsystem)
	{
		SaveSubsystem->OnSceneSave.AddDynamic(this,&ThisClass::OnSave);
		SaveSubsystem->OnSceneSaveActor.AddDynamic(this,&ThisClass::OnSaveActor);
	}
	
	DeviceManager = UWorld::GetSubsystem<UDeviceManager>(GWorld);
	
	//初始化场景编辑actor 与设备显示Actor
	InitializeScene();
	InitializeSceneTree();
}

void USceneManagerSubsystem::OnSystemStateOnChange(const FGameplayTag State)
{
	if (State == CommonToolTags::State_Null)
	{
		SetMarkType(CommonToolTags::Actor_Type_Null,false);
		ChangeScene("");
	}
}

FString USceneManagerSubsystem::GetCurrentSceneName() const
{
	return CurrentSceneName;
}

AComDatasmithActor* USceneManagerSubsystem::ResetDataSmithActor(const FString& Path,const bool& bLoad,AComDatasmithActor* Actor, const FTransform& Transform)
{
	if (!bLoad)
	{
		if (Actor)
		{
			Actor->Reset();
		}
	}
	else
	{
		if (Actor)
		{
			Actor->Reset();
		}
		else
		{
			Actor = Cast<AComDatasmithActor>
		(UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(),
															AComDatasmithActor::StaticClass(), Transform,
															ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
			UGameplayStatics::FinishSpawningActor(Actor, Transform);
		}
	
	
		Actor->LoadFile(Path);
		GetWorld()->GetTimerManager().ClearTimer(LoadSceneTimerHandle);
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("OnLoad"), Actor);
		GetWorld()->GetTimerManager().SetTimer(LoadSceneTimerHandle, TimerDelegate, 1.0, true);
	}
	return Actor;
}

AComLidarPointCloudActor* USceneManagerSubsystem::ResetPointCloudActor(const FString& Path,const bool& bLoad,
	AComLidarPointCloudActor* Actor, const FTransform& Transform)
{
	if (Path.IsEmpty())
	{
		if (Actor)
		{
			Actor->Reset();
			Actor->SetPointCloud(nullptr);
		}
		return Actor;
	}

	if (!bLoad)
	{
		if (Actor)
		{
			Actor->Reset();
			Actor->SetPointCloud(nullptr);
		}
	}
	else
	{
		if (Actor)
		{
			Actor->Reset();
			Actor->SetPointCloud(nullptr);
		}
		else
		{
			Actor = Cast<AComLidarPointCloudActor>
		(UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(),
															AComLidarPointCloudActor::StaticClass(), Transform,
															ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
			UGameplayStatics::FinishSpawningActor(Actor, Transform);
		}
		
		const FString& Filename = Path;
		bool bUseAsync = true;
		FLatentActionInfo LatentInfo;
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = FMath::Rand();
		LatentInfo.ExecutionFunction = TEXT("ResetPointCloudActorCompleted");
		LatentInfo.CallbackTarget = this;
		ELidarPointCloudAsyncMode AsyncMode;
		float Progress;
		ULidarPointCloud* PointCloud=nullptr;
		ULidarPointCloudBlueprintLibrary::CreatePointCloudFromFile(this,Filename,bUseAsync,LatentInfo,AsyncMode,Progress,PointCloud);
		PointCloud->OnPointCloudRebuilt().AddUObject(this,&ThisClass::PointCloudRebuilt);
		if (PointCloud&&Actor)
		{
			Actor->SetPointCloud(PointCloud);
		}
	}
	return Actor;
}

ACesium3DTileset* USceneManagerSubsystem::ResetCesium3DTileSetActor(const FString& Path, const bool& bLoad,
	ACesium3DTileset* Actor, const FTransform& Transform)
{

	if (Path.IsEmpty())
	{
		if (Actor)
		{
			Actor->Reset();
		}
		return Actor;
	}
	

	if (!bLoad)
	{
		if (Actor)
		{
			Actor->Reset();
		}
	}
	else
	{
		if (Actor)
		{
			Actor->Reset();
		}
		else
		{
			Actor = Cast<ACesium3DTileset>
		(UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(),
															ACesium3DTileset::StaticClass(), Transform,
															ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
			UGameplayStatics::FinishSpawningActor(Actor, Transform);
		}
		Actor->SetTilesetSource(ETilesetSource::FromUrl);
		FString FilePath = "file:///"+Path;
		Actor->SetUrl(FilePath);
		//ACesiumGeoreference::GetDefaultGeoreference(this)->PlaceGeoreferenceOriginHereRuntime();
		//ACesiumGeoreference::GetDefaultGeoreference(this)->PlaceGeoreferenceOriginHere();
	}
	return Actor;
}

void USceneManagerSubsystem::PointCloudRebuilt()
{
	// 这段代码将在游戏线程执行
	if (IsInGameThread())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("PointCloudRebuiltNextTick"));
		GetWorld()->GetTimerManager().ClearTimer(LoadSceneTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(LoadSceneTimerHandle, TimerDelegate, 0.1, true,0.5);
	}
}

void USceneManagerSubsystem::PointCloudRebuiltNextTick()
{
	GetWorld()->GetTimerManager().ClearTimer(LoadSceneTimerHandle);
}

void USceneManagerSubsystem::InitializeScene()
{
	UPrintToolLibrary::Debug(FString("SceneManagerSubsystem::InitializeScene"));

	CommonInfo = Cast<ACommonInfo>
	(UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(), ACommonInfo::StaticClass(),
	FTransform(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	UGameplayStatics::FinishSpawningActor(CommonInfo, FTransform());
}

void USceneManagerSubsystem::InitializeSceneTree()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]
	{
		FString JsonSceneData;
		UJsonToolLibrary::GetJsonString("SceneData",JsonSceneData,false,"SceneData");
		SceneTreeNode = BuildSceneTree(JsonSceneData);
		OnSceneTreeNodesUpdate.Broadcast(SceneTreeNode);
	}));
}

void USceneManagerSubsystem::ChangeScene(FString name)
{
	
	if (name.Equals(CurrentSceneName))
	{
		
	}
	else
	{
		ULoadingSubsystem::ShowLoadScreen(true);
		if (SceneActorMap.Contains(name))
		{
			LastSceneName = CurrentSceneName;
			CurrentSceneName = name;
			OnSceneChangeSignature.Broadcast(CurrentSceneName);
		}
		else
		{
			LastSceneName = CurrentSceneName;
			CurrentSceneName = "";
			OnSceneChangeSignature.Broadcast("");
		}
	}
}

bool USceneManagerSubsystem::GetCurrentJsonSceneData(FString& JsonSceneData)
{
	if (USceneTreeNode* SceneTreeNode = SceneLeafNodeMap.FindRef(CurrentSceneName))
	{
		if (SceneTreeNode->JsonObject)
		{
			UJsonToolLibrary::GetJsonStringFromJsonObject(SceneTreeNode->JsonObject,JsonSceneData);
			return true;
		}
	}
	return false;
}

bool USceneManagerSubsystem::GetJsonSceneData(const FString& SceneName, FString& JsonSceneData)
{
	if (USceneTreeNode* SceneTreeNode = SceneLeafNodeMap.FindRef(SceneName))
	{
		if (SceneTreeNode->JsonObject)
		{
			UJsonToolLibrary::GetJsonStringFromJsonObject(SceneTreeNode->JsonObject,JsonSceneData);
			return true;
		}
	}
	return false;
}

bool USceneManagerSubsystem::GetCurrentSceneTransform(FTransform& Transform)
{
	if (SceneActorMap.Contains(CurrentSceneName))
	{
		Transform = SceneActorMap.FindRef(CurrentSceneName)->GetActorTransform();
		return true;
	}
	return false;
}

AActor* USceneManagerSubsystem::GetTargetSceneActor(const FString& SceneName)
{
	return SceneActorMap.FindRef(SceneName);
}

void USceneManagerSubsystem::OnSceneChange(const FString& name)
{
	//场景改变时候卸载之前场景加载当前场景
	UnLoadSceneHierarchyFromJson(LastSceneName);
	LoadSceneHierarchyFromJson(CurrentSceneName);
	FString JsonSceneData;
	TArray<FString> LoadLevelsDate;
	if (GetCurrentJsonSceneData(JsonSceneData))
	{
		
		UJsonToolLibrary::GetJsonStringArrayFromJsonString(JsonSceneData,"LoadMaps",LoadLevelsDate);
		LoadMaps(LoadLevelsDate);
	}
	else
	{
		LoadMaps(LoadLevelsDate);
	}
}

void USceneManagerSubsystem::OnSceneChangeComplete()
{
	ULoadingSubsystem::ShowLoadScreen(false);
	if (!CurrentSceneName.IsEmpty())
	{
		MarkTypeMap.Empty();
		PositioningsMap.Empty();
		for (auto Element : CurrentStreamLevelMap)
		{
			if (Element.Value && Element.Value->GetLoadedLevel())
			{
				for (auto Actor : Element.Value->GetLoadedLevel()->Actors)
				{
					if (Actor)
					{
						OnActorSpawned(Actor);
					}
				}
			}
		}
	}
}

void USceneManagerSubsystem::OnCheckLevelChange()
{
	if (RequestAsyncLoadMapNum == 0 && RequestAsyncUnLoadMapNum == 0)
	{
		OnSceneChangeCompleteSignature.Broadcast();
	}
}

void USceneManagerSubsystem::OnActorSpawned(AActor* Actor)
{
	// 处理 Actor
	StaticActorsMap.Add(Actor->GetName(),Actor);
	if (UDeviceMarkComponent* DeviceMarkComponent = Actor->FindComponentByClass<UDeviceMarkComponent>())
	{
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

void USceneManagerSubsystem::OnActorDestroyed(AActor* Actor)
{
	// 处理 Actor
	if (StaticActorsMap.Contains(Actor->GetName()))
	{
		StaticActorsMap.Remove(Actor->GetName());
	}
	if (UDeviceMarkComponent* DeviceMarkComponent = Actor->FindComponentByClass<UDeviceMarkComponent>())
	{
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
					
				MarkTypeActors.Remove(Actor);
				for (AActor* ChildMarkTypeActor : ChildMarkTypeActors)
				{
					MarkTypeActors.Remove(ChildMarkTypeActor);
				}
				Actor->GetAttachedActors(ChildMarkTypeActors);
				for (AActor* ChildMarkTypeActor : ChildMarkTypeActors)
				{
					MarkTypeActors.Remove(ChildMarkTypeActor);
				}
				if (MarkTypeActors.Num()<=0)
				{
					MarkTypeMap.Remove(TypeTag);
				}
			}
		}
	}
	if (APositioning* Positioning = Cast<APositioning>(Actor))
	{
		if (!Positioning->LocationName.IsEmpty())
		{
			PositioningsMap.Remove(Positioning->LocationName);
		}
	}
}

void USceneManagerSubsystem::InteractiveHit(FHitResult HitResult)
{
	// if (bIsSelection)
	// {
	// 	if (CurrentSpawningActor)
	// 	{
	// 		if (IDeviceInterface* CurrentSpawnActorInterface = Cast<IDeviceInterface>(CurrentSpawningActor))
	// 		{
	// 			DrawDebugSphere(GetWorld(),HitResult.Location, 5.0f, 12, FColor::Green, false,5.0);
	// 			CurrentSpawnActorInterface->Execute_AddHitData(CurrentSpawningActor,HitResult);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		SpawnActor(EditItem);
	// 	}
	// }
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
	
	for (auto Item : DynamicActorsMap)
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
	AActor *Actor = DynamicActorsMap.FindRef(GUID);
	if (!Actor) return nullptr;

	//保存基本属性,使用在序列化与反序列化
	TSharedPtr<FJsonObject> ActorJson = MakeShared<FJsonObject>();
	
	if (Actor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
	{
		ActorJson = Cast<IDeviceInterface>(Actor)->GetDeviceJsonObjectData(GUID);
		//Cast<IDeviceInterface>(Actor)->SetDeviceJsonObjectData(ActorJson.ToSharedRef());
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
		TSharedPtr<FJsonObject> ChildJson = SerializeActor(*DynamicActorsMap.FindKey(Child));
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
	DynamicActorsMap.GetKeys(Keys);
	if (!Keys.IsEmpty())
	{
		for (auto Element : DynamicActorsMap)
		{
			if (Element.Value)
			{
				Element.Value->Destroy();
			}
		}
		DynamicActorsMap.Empty();
	}
	UpdateWorldHierarchyToJson("");
	return true;
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

AActor* USceneManagerSubsystem::DeSerializeActor(const TSharedPtr<FJsonObject>& JsonObject,AActor* Parent)
{
	//预先判断是否已经是场景中存在的actor
	bool bStatic = false;
	JsonObject->TryGetBoolField(TEXT("bStatic"),bStatic);
	if (bStatic)
	{
		FString StaticActorName;
		JsonObject->TryGetStringField(TEXT("StaticActorName"),StaticActorName);
		if (StaticActorsMap.Find(StaticActorName))
		{
			AActor* Actor = *StaticActorsMap.Find(StaticActorName);
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
	if (!UAttributeToolLibrary::GetStringAttribute(JsonObject,TEXT("ActorClass"),ActorPath)||!UAttributeToolLibrary::GetStringAttribute(JsonObject,TEXT("ActorTransform"),ActorTransform))
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

void USceneManagerSubsystem::LoadMaps(TArray<FString> LoadLevelsData)
{
	TArray<FString> AllLoadLevelsData;
	//两个并集
	for (auto LoadLevelData : CurrentLoadLevelsData)
	{
		AllLoadLevelsData.AddUnique(LoadLevelData);
	}
	for (auto LoadLevelData : LoadLevelsData)
	{
		AllLoadLevelsData.AddUnique(LoadLevelData);
	}
	RequestAsyncLoadMapNum = 0;
	RequestAsyncUnLoadMapNum = 0;
	
	for (int32 i = 0; i < AllLoadLevelsData.Num(); i++)
	{
		UpdateStreamLevel(AllLoadLevelsData[i],LoadLevelsData,AllLoadLevelsData);
	}
	if (RequestAsyncLoadMapNum == 0 && RequestAsyncUnLoadMapNum == 0)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
		{
			OnCheckLevelChangeSignature.Broadcast();
		}));
	}
}

void USceneManagerSubsystem::UpdateStreamLevel(const FString& LoadLevelData, const TArray<FString>& LoadLevelsData,
	const TArray<FString>& AllLoadLevelsData)
{
	//两个集合都包含 不处理
	if (CurrentLoadLevelsData.Contains(LoadLevelData)&&LoadLevelsData.Contains(LoadLevelData))
	{
		if (LoadLevelData.Equals(AllLoadLevelsData.Last()))
		{
		}
	}
	//之前包含,新加不包含 卸载关卡
	else if (CurrentLoadLevelsData.Contains(LoadLevelData)&&!LoadLevelsData.Contains(LoadLevelData))
	{
		RequestAsyncUnLoadMapNum++;
		CurrentLoadLevelsData.Remove(LoadLevelData);
		RequestAsyncUnLoadMap(LoadLevelData);
	}
	//之前不包含,新加包含 加载关卡
	else if (!CurrentLoadLevelsData.Contains(LoadLevelData)&&LoadLevelsData.Contains(LoadLevelData))
	{
		RequestAsyncLoadMapNum++;
		CurrentLoadLevelsData.AddUnique(LoadLevelData);
		RequestAsyncLoadMap(LoadLevelData);
	}
}

void USceneManagerSubsystem::RequestAsyncLoadMap(const FString& LoadMapData)
{
	FString LevelName;
	FString Path;
	FString TransformString;
	UJsonToolLibrary::GetStringFromJsonString(LoadMapData,"Name",LevelName);
	UJsonToolLibrary::GetStringFromJsonString(LoadMapData,"Path",Path);
	UJsonToolLibrary::GetStringFromJsonString(LoadMapData,"Transform",TransformString);
	if (CurrentStreamLevelMap.Contains(LevelName))
	{
		return;
	}

	// 构建正确的软引用路径
	FString PackagePath = FString::Printf(TEXT("/Game/Maps/%s/%s"),*Path, *LevelName);
	FString AssetName = LevelName;
	FSoftObjectPath LevelSoftPath(FString::Printf(TEXT("%s.%s"), *PackagePath, *AssetName));
	
	// 验证路径
	if (!LevelSoftPath.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid map path: %s"), *LevelSoftPath.ToString());
		return;
	}
    
	// 异步加载
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(
		LevelSoftPath,FStreamableDelegate::CreateLambda([this,LevelSoftPath,TransformString]()
	{
		UWorld* LoadedLevel = Cast<UWorld>(LevelSoftPath.ResolveObject());
		if (LoadedLevel)
		{
		 
		}
		else
		{
			// 尝试同步加载
			LoadedLevel = Cast<UWorld>(LevelSoftPath.TryLoad());
		}
		
		if (LoadedLevel)
		{
			FString LevelName = FPackageName::GetShortName(LoadedLevel->GetName());
			bool bOutSuccess;
			FTransform Transform;
			Transform.InitFromString(TransformString);

			if (GetWorld())
			{
				if (ULevelStreamingDynamic* LevelStreamingDynamic = ULevelStreamingDynamic::LoadLevelInstance(
				GetWorld(), LevelName, Transform.GetLocation(), Transform.GetRotation().Rotator(), bOutSuccess))
				{
					LevelStreamingDynamic->OnLevelLoaded.AddDynamic(this, &ThisClass::OnLevelLoaded);
					LevelStreamingDynamic->SetShouldBeLoaded(true);
					LevelStreamingDynamic->SetShouldBeVisible(true);
					CurrentStreamLevelMap.Add(LevelName, LevelStreamingDynamic);
					UPrintToolLibrary::Debug(FString::Printf(TEXT("加载地图：%s"), *LevelName));
				}
			}
		}
		else
		{
			UPrintToolLibrary::Error(FString::Printf(TEXT("加载地图失败：%s"), *LevelSoftPath.ToString()));
		}
	})
	);
}

void USceneManagerSubsystem::RequestAsyncUnLoadMap(const FString& LoadMapData)
{
	FString LevelName;
	FString Path;
	UJsonToolLibrary::GetStringFromJsonString(LoadMapData,"Name",LevelName);
	UJsonToolLibrary::GetStringFromJsonString(LoadMapData,"Path",Path);
	
	if (!CurrentStreamLevelMap.Contains(LevelName))
	{
		return;
	}
	ULevelStreamingDynamic* LevelStreamingDynamic = CurrentStreamLevelMap.FindRef(LevelName);
	if (LevelStreamingDynamic)
	{
		LevelStreamingDynamic->OnLevelUnloaded.AddDynamic(this,&ThisClass::OnUnLevelLoaded);
		LevelStreamingDynamic->OnLevelLoaded.RemoveAll(this);
		LevelStreamingDynamic->SetShouldBeLoaded(false);
		LevelStreamingDynamic->SetShouldBeVisible(false);
		CurrentStreamLevelMap.Remove(LevelName);
		UPrintToolLibrary::Debug(FString::Printf(TEXT("卸载地图：%s"), *LevelName));
	}
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
	if (DynamicActorsMap.Find(GUID))
	{
		TreeNode->ActorPtr = *DynamicActorsMap.Find(GUID);
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
	if (DynamicActorsMap.FindKey(actor))
	{
		GUID =*DynamicActorsMap.FindKey(actor);
		return true;
	}
	return false;
}

AActor* USceneManagerSubsystem::GetActor(const FString& GUID)
{
	if (DynamicActorsMap.Contains(GUID))
	{
		return  DynamicActorsMap.FindRef(GUID);
	}
	return nullptr;
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

TArray<UWorldAssetTreeNode*> USceneManagerSubsystem::GetWorldAssetTreeNodes()
{
	return WorldAssetTreeNodes;
}

void USceneManagerSubsystem::OnLoad(const AComDatasmithActor* CommonDataSmithRuntimeActor)
{
	if (CommonDataSmithRuntimeActor)
	{
		if (CommonDataSmithRuntimeActor->bBuilding)
		{
			
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(LoadSceneTimerHandle);
		}
	}
}

void USceneManagerSubsystem::OnLevelLoaded()
{
	RequestAsyncLoadMapNum--;
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		OnCheckLevelChangeSignature.Broadcast();
	}));
}

void USceneManagerSubsystem::RegisterScene(const FString& GUID, AActor* actor)
{
	if (!SceneActorMap.Contains(GUID))
	{
		SceneActorMap.Add(GUID,actor);
	}
}

void USceneManagerSubsystem::UnRegisterScene(const FString& GUID)
{
	if (SceneActorMap.Contains(GUID))
	{
		SceneActorMap.Remove(GUID);
	}
}

bool USceneManagerSubsystem::GetScenePoints(TArray<FVector>& ScenePoints)
{
	for (auto Element : SceneActorMap)
	{
		ScenePoints.Add(Element.Value->GetActorLocation());
	}
	return true;
}

void USceneManagerSubsystem::MoveToRecentlyLineScene()
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	if (SceneTreeNode && Character)
	{
		if (USceneTreeNode* CurrentSceneTreeNode = SceneTreeNode->FindNodeByName(TEXT("接触网")))
		{
			FString TargetScene;
			float Distance = 100000000000000000.0;
			TArray<USceneTreeNode*> AllTargetSceneTreeNode= CurrentSceneTreeNode->GetAllLeafNodes();
			for (auto Element : AllTargetSceneTreeNode)
			{
				if (auto SceneActor = SceneActorMap.FindRef(Element->Name))
				{
					float CurrentDistance = FVector::Distance(Character->GetActorLocation(),SceneActor->GetActorLocation());

					if (Distance>CurrentDistance)
					{
						Distance = CurrentDistance;
						TargetScene = Element->Name;
					}
				}
			}
			if (SceneActorMap.Find(TargetScene))
			{
				ChangeScene(TargetScene);
			}
		}
	}
}

void USceneManagerSubsystem::OnUnLevelLoaded()
{
	RequestAsyncUnLoadMapNum--;
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		OnCheckLevelChangeSignature.Broadcast();
	}));
}

USceneTreeNode* USceneManagerSubsystem::BuildSceneTree(const FString& JsonString)
{
	SceneTreeNodeMap.Empty();
	// 创建根节点
	USceneTreeNode* Root = NewObject<USceneTreeNode>();
	Root->Initialize(TEXT("广湛线"), TSharedPtr<FJsonObject>());
	SceneTreeNodeMap.Add(TEXT("广湛线"), Root);
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const FString& Key = "SceneList";
		TArray<TSharedPtr<FJsonValue>> SceneList = JsonObject->GetArrayField(Key);
		for (auto Scene : SceneList)
		{
			const TSharedPtr<FJsonObject> SceneJsonObject =  Scene->AsObject();
			
			if (SceneJsonObject.IsValid())
			{
				FString Id = "";
				SceneJsonObject->TryGetStringField(TEXT("Id"), Id);
				FString Path = "";
				SceneJsonObject->TryGetStringField(TEXT("Path"), Path);
				TArray<FString> PathParts;
				Path.ParseIntoArray(PathParts, TEXT("/"));
				FString CurrentPath = TEXT("广湛线");
				USceneTreeNode* CurrentNode = Root;
        
				for (const FString& Part : PathParts)
				{
					FString NewPath = CurrentPath + "/" + Part;
            
					if (!SceneTreeNodeMap.Contains(NewPath))
					{
						USceneTreeNode* NewNode = NewObject<USceneTreeNode>();
						NewNode->Initialize(Part,SceneJsonObject);
						CurrentNode->AddChild(NewNode);
						SceneTreeNodeMap.Add(NewPath, NewNode);
					}
            
					CurrentPath = NewPath;
					CurrentNode = SceneTreeNodeMap[CurrentPath];
				}
        
				// 设置叶节点ID
				if (CurrentNode && !Id.IsEmpty())
				{
					CurrentNode->Id = Id;
					SceneLeafNodeMap.Add(PathParts.Last(),CurrentNode);
				}
			}
		}
	}
	return Root;
}

void USceneManagerSubsystem::MoveToPosition(const FString& PositionName)
{
	if (InteractiveSubsystem)
	{
		InteractiveSubsystem->ClearInteractiveActor();
	}
	if (AActor* Actor = PositioningsMap.FindRef(PositionName))
	{
		if (StateSubsystem)
		{
			StateSubsystem->ChangeUserState(CommonToolTags::UserState_FirstPerson, Actor->GetActorTransform(), 1);
		}
	}
}

void USceneManagerSubsystem::GetPositionings(TArray<FString>& Positionings,const FGameplayTag& matchGameplayTag)
{
	TArray<FString> Results;
	for (auto Positioning : PositioningsMap)
	{
		if (Positioning.Value)
		{
			if (Positioning.Value->GameplayTag == matchGameplayTag)
			{
				Results.Add(Positioning.Key);
			}
		}
	}
	Positionings = Results;
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
			if (Element.Key && Element.Key->IsValidLowLevel())
			{
				for (int32 i = 0; i< Element.Key->GetMaterials().Num();i++)
				{
					Element.Key->SetMaterial(i,Element.Value[i]);
				}
			}
		}
		LastMarkActorsMaterialMap.Empty();
		LastMarkActors.Empty();
	}
	else
	{
		if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
		{
		
			if (UMaterialInterface* TransparentMaterial = CommonSettings->TransparentMaterial.LoadSynchronous())
			{
				UMaterialInstanceDynamic* MaterialInstanceDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this,TransparentMaterial);
				MaterialInstanceDynamic->SetScalarParameterValue(FName(TEXT("opaque")),CurrentMarkScale);
				// UMateriaL
				// TransparentMaterial->set
				for (auto Element : LastMarkActorsMaterialMap)
				{
					if (Element.Key && Element.Key->IsValidLowLevel())
					{
						for (int32 i = 0; i< Element.Key->GetMaterials().Num();i++)
						{
							Element.Key->SetMaterial(i,Element.Value[i]);
						}
					}
				}
				
				LastMarkActorsMaterialMap.Empty();
				LastMarkActors.Empty();
				
				GetMarkTypeActors(MarkType,LastMarkActors);

				for (auto Element : LastMarkActors)
				{
					if (Element&&Element->IsValidLowLevel())
					{
						TArray<UStaticMeshComponent*> ComponentArray;
						Element->GetComponents(ComponentArray);
						for (TObjectPtr<UStaticMeshComponent> Component : ComponentArray)
						{
							TArray<UMaterialInterface*> Materias = Component->GetMaterials();
							TArray<TObjectPtr<UMaterialInterface>> MaterialPtrs;
							for (auto Materia : Materias)
							{
								MaterialPtrs.Add(Materia);
							}
							
							LastMarkActorsMaterialMap.Add(Component,MaterialPtrs);
							for (int32 i = 0; i < Materias.Num(); i++)
							{
								Component->SetMaterial(i,MaterialInstanceDynamic);
							}
						}
					}
				}
			}
		}		
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

USceneTreeNode* USceneManagerSubsystem::GetSceneTreeNode()
{
	return SceneTreeNode;	
}

void USceneManagerSubsystem::GetRegisterSceneNumSort(int32 Num,TArray<FString>& ResultKeys)
{
	AActor* SceneActor = SceneActorMap.FindRef(GetCurrentSceneName());
	if (!SceneActor)
	{
		return; // 安全检查
	}

	// 创建临时数组来存储键值对
	TArray<TPair<FString, float>> SortArray;

	for (const auto& Element : SceneActorMap)
	{
		if (Element.Value && SceneActor!=Element.Value) // 安全检查
		{
			float Distance = FVector::Distance(SceneActor->GetActorLocation(), Element.Value->GetActorLocation());
			SortArray.Emplace(Element.Key, Distance);
		}
	}

	// 按值（距离）排序
	SortArray.Sort([](const TPair<FString, float>& A, const TPair<FString, float>& B) {
		return A.Value < B.Value; // 升序排序
		// return A.Value > B.Value; // 降序排序
	});

	// 现在 SortArray 包含了按距离排序的键值对
	for (const auto& SortedElement : SortArray)
	{
		ResultKeys.Emplace(SortedElement.Key, SortedElement.Value);
	}
	if (ResultKeys.Num()>Num)
	{
		ResultKeys.Reserve(Num);
	}
}

void USceneManagerSubsystem::InitScenePOI(const FString& SceneName, const bool bShow,UUserWidget* UserWidget,const FVector& Offsize)
{
	if (UserWidget && SceneActorMap.Contains(SceneName))
	{
		FVector Origin;
		FVector BoxExtent;
		if (bShow)
		{

			SceneActorMap.FindRef(SceneName)->GetActorBounds(false, Origin, BoxExtent);
		}
		else
		{
			Origin = FVector(0,0,-100000);
		}
		OnScenePOIInit.Broadcast(SceneName,Origin+Offsize,bShow,UserWidget);
	}
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
							// const TArray<TSharedPtr<FJsonValue>>* AttributeArrayValues;
							// JsonObject->TryGetArrayField(TEXT("Attribute"),AttributeArrayValues);
							if (Attributes.IsEmpty() /*&& AttributeArrayValues->IsEmpty()*/)
							{
								DynamicActorsMap.Remove(GUID);
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
					DynamicActorsMap.Remove(GUID);
				}
				else
				{
					TopLevelActors.Add(MakeShared<FJsonValueObject>(JsonObject));
				}
				RootJsonObject->SetArrayField(TEXT("Actors"), TopLevelActors);
			}

			AActor* Actor = DynamicActorsMap.FindRef(GUID);
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
						DynamicActorsMap.Remove(GUID);
					}
					else
					{
						TopLevelActors.AddUnique(MakeShared<FJsonValueObject>(JsonObjectLocal));
						AActor* Actor = DynamicActorsMap.FindRef(GUID);
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
	if (Attributes.IsEmpty())
	{
		if (AActor* Actor = DynamicActorsMap.FindRef(GUID))
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
		if (AActor* Actor = DynamicActorsMap.FindRef(GUID))
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
				NewJsonObject->SetBoolField(TEXT("bDisplay"), bDisplay);

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
		else
		{
			return true;
		}
	}
	return false;
}

void USceneManagerSubsystem::SaveActor(AActor* actor)
{
	if (DynamicActorsMap.FindKey(actor))
	{
		TArray<FString> Attributes;
		FString GUID = *DynamicActorsMap.FindKey(actor);
		if (UAttributeToolLibrary::GetAttributes(GetCurrentSceneName(),GUID,Attributes))
		{
			SaveSubsystem->OnSceneSaveActor.Broadcast(GUID,Attributes);
		}
	}
}

void USceneManagerSubsystem::SaveNewActorToJson(AActor* actor, FString& GUID)
{
	if (SaveSubsystem && actor)
	{
		RegisterActor(GUID,actor,true);
		TArray<FString> Attributes = TArray<FString>();
		if (TSharedPtr<FJsonObject> JsonObject = Cast<IDeviceInterface>(actor)->GetDeviceJsonObjectData(GUID))
		{
			Cast<IDeviceInterface>(actor)->SetDeviceJsonObjectData(JsonObject.ToSharedRef());
			if (actor->GetClass()->ImplementsInterface(UDeviceInterface::StaticClass()))
			{
				IDeviceInterface::Execute_GetAttributes(actor,Attributes);
			}
		}
		SaveSubsystem->OnSceneSaveActor.Broadcast(GUID,Attributes);
	}
}

void USceneManagerSubsystem::SaveActorToJson(FString& GUID,const TArray<FString> Attributes)
{
	SaveSubsystem->OnSceneSaveActor.Broadcast(GUID,Attributes);
}

void USceneManagerSubsystem::RegisterActor(FString& GUID,AActor* Actor,bool bRegenerate)
{
	if (bRegenerate)
	{
		GUID = FGuid::NewGuid().ToString();
	}
	if (!DynamicActorsMap.Contains(GUID))
	{
		DynamicActorsMap.Add(GUID,Actor);
	}
}

void USceneManagerSubsystem::UnRegisterActor(const FString& GUID)
{
	if (DynamicActorsMap.Contains(GUID))
	{
		DynamicActorsMap.Remove(GUID);
	}
}