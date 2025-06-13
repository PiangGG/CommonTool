// Fill out your copyright notice in the Description page of Project Settings.


#include "TrainStationManager.h"

#include "CommonToolTags.h"
#include "CommonTool/Library/JsonToolLibrary.h"
#include "CommonTool/Library/PrintToolLibrary.h"
#include "CommonTool/SubSystem/LoadingSubsystem.h"
#include "CommonTool/SubSystem/SceneManagerSubsystem.h"
#include "CommonTool/SubSystem/StateSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
FName L_MainDefualt = FName(TEXT("L_MainDefualt"));
UTrainStationManager::UTrainStationManager()
{
	
}

void UTrainStationManager::OnSystemStateOnChange(FGameplayTag newState)
{
	if (newState == CommonToolTags::State_Null)
	{
		USceneManagerSubsystem* SceneManagerSubsystem = USceneManagerSubsystem::Get(GetWorld());
		if (SceneManagerSubsystem)
		{
			SceneManagerSubsystem->SetCurrentSceneName("");
		}
		TArray<FString> LoadLevels;
		LoadLevels.AddUnique(L_MainDefualt.ToString());
		LoadMaps(LoadLevels);
	}

	if (newState == CommonToolTags::State_MainMenu)
	{
		ReSetCurrentTrainStationView(false);
	}
}

void UTrainStationManager::OnUnLevelLoaded()
{
	if (ULoadingSubsystem::Get(this))
	{
		ULoadingSubsystem::Get(this)->ShowLoadScreen(false);
	}
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		if (this->OnTrainStationSelectedComplete.IsBound())
		{
			this->OnTrainStationSelectedComplete.Broadcast();
		}
		GetWorld()->GetTimerManager().ClearTimer(LoadMapsTimerHandle);
	}));
}

void UTrainStationManager::OnLevelLoaded()
{
	if (ULoadingSubsystem::Get(this))
	{
		ULoadingSubsystem::Get(this)->ShowLoadScreen(false);
	}
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		if (this->OnTrainStationSelectedComplete.IsBound())
		{
			this->OnTrainStationSelectedComplete.Broadcast();
		}
		GetWorld()->GetTimerManager().ClearTimer(LoadMapsTimerHandle);
		ReSetCurrentTrainStationView();
	}));
	
}

UTrainStationManager* UTrainStationManager::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return UWorld::GetSubsystem<UTrainStationManager>(WorldContextObject->GetWorld());
	}
	return nullptr;
}

void UTrainStationManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	TrainsStationSelected.AddDynamic(this,&ThisClass::UTrainStationManager::OnTrainsStationSelected);
	TrainStationSelected.AddDynamic(this,&ThisClass::UTrainStationManager::OnTrainStationSelected);

	if (UStateSubsystem * StateSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UStateSubsystem>())
	{
		StateSubsystem->SystemStateOnChange.AddDynamic(this,&ThisClass::OnSystemStateOnChange);
	}
	IntiTrainStation();
}

void UTrainStationManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("TrainStationManager::Initialize"));
}

void UTrainStationManager::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("TrainStationManager::Deinitialize"));
	Super::Deinitialize();
}

void UTrainStationManager::IntiTrainStation()
{
	FString JsonStr;
	UJsonToolLibrary::GetJsonString("TrainStation",JsonStr);
	
	TArray<FString> JsonStringArray;
	UJsonToolLibrary::GetJsonStringArrayFromJsonString(JsonStr,"Data",JsonStringArray);
	
	for (auto Item : JsonStringArray)
	{
		FString TrainStationName;
		UJsonToolLibrary::GetStringFromJsonString(Item,"Name",TrainStationName);
		TrainsStationMap.Add(TrainStationName,Item);
	}
}

void UTrainStationManager::LoadMaps(TArray<FString> LoadLevels)
{
	//显示加载界面
	ULoadingSubsystem::ShowLoadScreen(true);
	GetWorld()->GetTimerManager().ClearTimer(LoadMapsTimerHandle);
	TArray<FString> AllLoadLevels;
	//两个并集
	for (auto LoadLevel : CurrentLoadLevels)
	{
		AllLoadLevels.AddUnique(LoadLevel);
	}
	for (auto LoadLevel : LoadLevels)
	{
		AllLoadLevels.AddUnique(LoadLevel);
	}
	float TimerInterval = 0.0;
	for (auto LoadLevel : AllLoadLevels)
	{
		TimerInterval+=0.1;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this,TEXT("UpdateStreamLevel"),LoadLevel,LoadLevels,AllLoadLevels,LoadMapsTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(LoadMapsTimerHandle,TimerDelegate,TimerInterval,true);
	}
}

void UTrainStationManager::UpdateStreamLevel(const FString& LoadLevel,const TArray<FString>&LoadLevels,const TArray<FString>&AllLoadLevels,FTimerHandle & TimerHandle)
{
	
	//两个集合都包含 不处理
	if (CurrentLoadLevels.Contains(LoadLevel)&&LoadLevels.Contains(LoadLevel))
	{
		if (LoadLevel.Equals(AllLoadLevels.Last()))
		{
			ULoadingSubsystem::ShowLoadScreen(false);
			GetWorld()->GetTimerManager().ClearTimer(LoadMapsTimerHandle);
		}
	}
	//之前包含,新加不包含 卸载关卡
	else if (CurrentLoadLevels.Contains(LoadLevel)&&!LoadLevels.Contains(LoadLevel))
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.Linkage = FMath::RandRange(0,65535);
		if (LoadLevel.Equals(AllLoadLevels.Last()))
		{
			LatentInfo.ExecutionFunction = "OnUnLevelLoaded";
		}
		CurrentLoadLevels.Remove(LoadLevel);
		UGameplayStatics::UnloadStreamLevel(this,FName(LoadLevel),LatentInfo,false);
		UPrintToolLibrary::Debug(FString::Printf(TEXT("卸载地图：%s"), *LoadLevel));
	}
	//之前不包含,新加包含 加载关卡
	else if (!CurrentLoadLevels.Contains(LoadLevel)&&LoadLevels.Contains(LoadLevel))
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.Linkage = FMath::RandRange(0,65535);
		if (LoadLevel.Equals(AllLoadLevels.Last()))
		{
			LatentInfo.ExecutionFunction = "OnLevelLoaded";
		}
		CurrentLoadLevels.AddUnique(LoadLevel);
		
		FSoftObjectPath LevelSoftPath = FSoftObjectPath(FString::Printf(TEXT("/Game/Maps/%s.%s"), *LoadLevel, *LoadLevel));
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		UWorld* World = GetWorld();
		TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(
			LevelSoftPath,FStreamableDelegate::CreateLambda([this,World,LevelSoftPath,LatentInfo]()
		{
				const TSoftObjectPtr<UWorld> LevelPtr = TSoftObjectPtr<UWorld>(LevelSoftPath);
				// 验证有效性
				if (LevelPtr.IsValid())
				{
					// 实例化关卡
					FString LevelName = FPackageName::GetShortName(LevelPtr.GetAssetName());
					// bool bOutSuccess;
					// ULevelStreamingDynamic::LoadLevelInstance(World, LevelName, FVector::ZeroVector, FRotator::ZeroRotator, bOutSuccess);
					
					//TODO 后面优化考虑使用这种方式(加载地图不卡顿)
					//UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this,LevelPtr,true,false,LatentInfo);
					UGameplayStatics::LoadStreamLevel(this,FName(LevelName),true,false,LatentInfo);
					UPrintToolLibrary::Debug(FString::Printf(TEXT("加载地图：%s"), *LevelName));
				}
				else
				{
					UPrintToolLibrary::Error(FString::Printf(TEXT("加载地图：%s.失败"), *LevelSoftPath.ToString()));
					
				}
		})
		);
		// UGameplayStatics::LoadStreamLevel(this,FName(LoadLevel),true,false,LatentInfo);
		// UToolFunctionLibrary::Debug(FString::Printf(TEXT("加载地图：%s"), *LoadLevel));
	}
}

void UTrainStationManager::OnTrainsStationSelected(const FString& TrainsStation)
{
	UPrintToolLibrary::Debug(FString::Printf(TEXT("OnTrainsStationSelected::%s"), *TrainsStation));
	CurrentTrainsStation = TrainsStation;

	//清空
	TrainStationMap.Empty();
	TrainStationMarkPosition.Empty();
	
	TArray<FString> JsonStringArray;
	//获取线路上数据列表
	UJsonToolLibrary::GetJsonStringArrayFromJsonString(TrainsStationMap.FindRef(CurrentTrainsStation),"TrainStations",JsonStringArray);
	for (auto Item : JsonStringArray)
	{
		FString TrainStationName;
		UJsonToolLibrary::GetStringFromJsonString(Item,"Name",TrainStationName);
		TrainStationMap.Add(TrainStationName,Item);
	}
}

void UTrainStationManager::OnTrainStationSelected(const FString& TrainStation)
{
	//加载该站的数据
	LoadTrainStationFromJson(TrainStation);
	UPrintToolLibrary::Debug(FString::Printf(TEXT("OnTrainStationSelected::%s"), *TrainStation));
	CurrentTrainStation = TrainStation;
	
	FString TrainStationData;
	//获取当前站点数据,能获取 不能获取
	if (GetCurrentTrainStationData(CurrentTrainStation,TrainStationData))
	{
		TArray<FString> LoadLevelsDate;
		UJsonToolLibrary::GetJsonStringArrayFromJsonString(TrainStationData,"LoadMaps",LoadLevelsDate);
		
		TArray<FString> LoadLevels;
		for (auto LoadLevel : LoadLevelsDate)
		{
			FString LevelName;
			UJsonToolLibrary::GetStringFromJsonString(LoadLevel,"Name",LevelName);
			LoadLevels.AddUnique(LevelName);
		}
		
		LoadMaps(LoadLevels);
	}
	else
	{
		TArray<FString> LoadLevels;
		LoadLevels.AddUnique(L_MainDefualt.ToString());
		LoadMaps(LoadLevels);
	}
}


void UTrainStationManager::AddTrainStation(const FString &TrainStationName, AActor* TrainStation)
{
	TrainStationActorMap.Add(TrainStationName,TrainStation);
}

void UTrainStationManager::ChangedSelectedTrainsStation(const FString& TrainsStationName)
{
	if (TrainsStationMap.Contains(TrainsStationName))
	{
		TrainsStationSelected.Broadcast(TrainsStationName);
	}
	else
	{
		UPrintToolLibrary::Error(FString::Printf(TEXT("OnTrainsStationSelected::%s:失败"), *TrainsStationName));
	}
}

void UTrainStationManager::ChangedSelectedTrainStation(const FString& TrainStationName)
{
	TrainStationSelected.Broadcast(TrainStationName);
}

void UTrainStationManager::GetCurrentTrainsStations(TArray<FString>& TrainsStations)
{
	TrainsStationMap.GetKeys(TrainsStations);
}

bool UTrainStationManager::GetCurrentTrainStationsData(const FString& TrainsStationName, FString& TrainsStationData)
{
	TrainsStationData = TrainsStationMap.FindRef(TrainsStationName);

	return !TrainsStationData.IsEmpty();
}

void UTrainStationManager::GetCurrentTrainStation(FString& TrainsStation)
{
	TrainsStation = CurrentTrainStation;
}

bool UTrainStationManager::GeTrainStationData(const FString& TrainsStationName, const FString& TrainStationName,
                                              FString& TrainStationData)
{
	FString TrainStationsData;
	GetCurrentTrainStationsData(TrainsStationName,TrainStationsData);
	TArray<FString> JsonStringArray;
	UJsonToolLibrary::GetJsonStringArrayFromJsonString(TrainStationsData,"TrainStations",JsonStringArray);
	for (auto Item : JsonStringArray)
	{
		FString Name;
		UJsonToolLibrary::GetStringFromJsonString(Item,"Name",Name);
		if (Name.Equals(TrainStationName))
		{
			TrainStationData = Item;
			return true;
		}
	}
	return false;
}

bool UTrainStationManager::GetCurrentTrainStationData(const FString& TrainStationName, FString& TrainStationData)
{
	TrainStationData = TrainStationMap.FindRef(TrainStationName);
	return !TrainStationData.IsEmpty();
}

AActor* UTrainStationManager::GetCurrentTrainStationActor(const FString& TrainStationName)
{
	return TrainStationActorMap.FindRef(TrainStationName);
}

bool UTrainStationManager::GetTrainStationPoints(const FString& TrainsStationName, TArray<FVector>& TrainsStationPoints)
{
	bool Result = false;
	FString TrainStationData;
	GetCurrentTrainStationsData(TrainsStationName,TrainStationData);

	//站点列表
	TArray<FString> JsonStringArray;
	UJsonToolLibrary::GetJsonStringArrayFromJsonString(TrainStationData,"TrainStations",JsonStringArray);
	TArray<FVector> Points;

	for (auto Item : JsonStringArray)
	{
		FString Name;
		UJsonToolLibrary::GetStringFromJsonString(Item,"Name",Name);
		if (AActor* ItemActor =  GetCurrentTrainStationActor(Name))
		{
			Points.AddUnique(ItemActor->GetActorLocation());
		}
	}
	TrainsStationPoints = Points;
	Result = !TrainsStationPoints.IsEmpty();
	return Result;
}

bool UTrainStationManager::LoadTrainStationFromJson(const FString& TrainStationName)
{
	UWorld *World = GetWorld();
	if (!World||TrainStationName.IsEmpty()) return false;
	USceneManagerSubsystem* SceneManagerSubsystem = USceneManagerSubsystem::Get(World);
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->SetCurrentSceneName(TrainStationName);
		return true;
	}
	return false;
}

void UTrainStationManager::ReSetCurrentTrainStationView(bool breturnMain)
{
	if (UStateSubsystem* StateSubsystem = UStateSubsystem::Get(this))
	{
		FTransform Transform;
		if (GetCurrentTrainStationViewTransform(Transform))
		{
			StateSubsystem->ChangeUserState(CommonToolTags::UserState_Focus,Transform,10000.0);
			if (breturnMain)
			{
				StateSubsystem->ChangeSystemState(CommonToolTags::State_MainMenu);
			}
		}
	}
}

bool UTrainStationManager::GetCurrentTrainStationViewTransform(FTransform& Transform)
{
	if (CurrentTrainStation.IsEmpty())
	{
		return false;
	}
	FString TrainStationData;
	GetCurrentTrainStationData(CurrentTrainStation,TrainStationData);
	FString LocationString;
	UJsonToolLibrary::GetStringFromJsonString(TrainStationData,"OffSize",LocationString);
	FVector Location = FVector::ZeroVector;
	bool LocationConv = false;
	UKismetStringLibrary::Conv_StringToVector(LocationString,Location,LocationConv);
		
	FString RotationString;
	UJsonToolLibrary::GetStringFromJsonString(TrainStationData,"Rotator",RotationString);
	FVector RotationV = FVector::ZeroVector;
	bool RotationConv = false;
	UKismetStringLibrary::Conv_StringToVector(RotationString,RotationV,RotationConv);

	if (LocationConv && RotationConv)
	{
		FRotator Rotation = FRotator(RotationV.Y, RotationV.Z, RotationV.X);
		Transform.SetLocation(Location);
		Transform.SetRotation(Rotation.Quaternion());
		return true;
	}
	return false;
}

bool UTrainStationManager::GetTrainsStationData(const FString& TrainsStationName, FString& TrainsStationData)
{
	TrainsStationData = TrainsStationMap.FindRef(TrainsStationName);
	return !TrainsStationData.IsEmpty();
}

void UTrainStationManager::AddTrainStationMarkPosition(const FString& TrainStationName, FVector Position)
{
	TrainStationMarkPosition.Add(TrainStationName,Position);
}

void UTrainStationManager::RemoveTrainStationMarkPosition(const FString& TrainsStationName)
{
	TrainStationMarkPosition.Remove(TrainsStationName);
}

bool UTrainStationManager::GetTrainStationPoints(TArray<FVector>& TrainStationPoints)
{
	TrainStationPoints.Empty();
	for (auto Element : TrainStationMarkPosition)
	{
		TrainStationPoints.Add(Element.Value);
	}
	return true;
}

bool UTrainStationManager::GetTrainStationPoint(const FVector& Point)
{
	return TrainStationMarkPosition.FindKey(Point)->Equals(CurrentTrainStation);
}
