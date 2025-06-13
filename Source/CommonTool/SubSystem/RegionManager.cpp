// Fill out your copyright notice in the Description page of Project Settings.


#include "RegionManager.h"
#include "CommonToolTags.h"
#include "CommonTool/Library/JsonToolLibrary.h"
#include "CommonTool/Library/PrintToolLibrary.h"
#include "CommonTool/SubSystem/LoadingSubsystem.h"
#include "CommonTool/SubSystem/SceneManagerSubsystem.h"
#include "CommonTool/SubSystem/StateSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"

FName L_MainDefualt = FName(TEXT("L_MainDefualt"));

URegionManager::URegionManager(): StateSubsystem(nullptr)
{
}

void URegionManager::OnSystemStateOnChange(FGameplayTag newState)
{
	if (newState == CommonToolTags::State_Null)
	{
		//TODO 改到场景自己执行 解构 必须解耦
		// USceneManagerSubsystem* SceneManagerSubsystem = USceneManagerSubsystem::Get(GetWorld());
		// if (SceneManagerSubsystem)
		// {
		// 	SceneManagerSubsystem->SetCurrentSceneName("");
		// }
		TArray<FString> LoadLevels;
		LoadLevels.AddUnique(L_MainDefualt.ToString());
		LoadMaps(LoadLevels);
	}

	if (newState == CommonToolTags::State_MainMenu)
	{
		ReturnCurrentRegionView();
	}
}

void URegionManager::OnUnLevelLoaded()
{
	if (ULoadingSubsystem::Get(this))
	{
		ULoadingSubsystem::Get(this)->ShowLoadScreen(false);
	}
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		if (this->OnRegionChangingCompleteSignature.IsBound())
		{
			this->OnRegionChangingCompleteSignature.Broadcast();
		}
		GetWorld()->GetTimerManager().ClearTimer(LoadMapsTimerHandle);
	}));
}

void URegionManager::OnLevelLoaded()
{
	if (ULoadingSubsystem::Get(this))
	{
		ULoadingSubsystem::Get(this)->ShowLoadScreen(false);
	}
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		if (this->OnRegionChangingCompleteSignature.IsBound())
		{
			this->OnRegionChangingCompleteSignature.Broadcast();
		}
		GetWorld()->GetTimerManager().ClearTimer(LoadMapsTimerHandle);
		ReturnCurrentRegionView();
	}));
	
}

URegionManager* URegionManager::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return UWorld::GetSubsystem<URegionManager>(WorldContextObject->GetWorld());
	}
	return nullptr;
}

void URegionManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	StateSubsystem = UStateSubsystem::Get(this);
	
	OnRegionChangingSignature.AddDynamic(this,&ThisClass::OnRegionChanging);
	OnRegionChangingCompleteSignature.AddDynamic(this,&ThisClass::OnRegionChangingComplete);

	if (StateSubsystem)
	{
		StateSubsystem->SystemStateOnChange.AddDynamic(this,&ThisClass::OnSystemStateOnChange);
	}
	IntiAllRegion();
}

void URegionManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("TrainStationManager::Initialize"));
}

void URegionManager::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("TrainStationManager::Deinitialize"));
	Super::Deinitialize();
}

void URegionManager::IntiAllRegion()
{
	FString JsonStr;
	UJsonToolLibrary::GetJsonString("Region",JsonStr);
	
	TArray<FString> JsonStringArray;
	UJsonToolLibrary::GetJsonStringArrayFromJsonString(JsonStr,"Data",JsonStringArray);
	
	for (auto Item : JsonStringArray)
	{
		FString TrainStationName;
		UJsonToolLibrary::GetStringFromJsonString(Item,"Name",TrainStationName);
		RegionDataMap.Add(TrainStationName,Item);
	}
}

void URegionManager::LoadMaps(TArray<FString> LoadLevels)
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

void URegionManager::UpdateStreamLevel(const FString& LoadLevel,const TArray<FString>&LoadLevels,const TArray<FString>&AllLoadLevels,FTimerHandle & TimerHandle)
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
	}
}

void URegionManager::OnRegionChanging(const FString& Region)
{
	UPrintToolLibrary::Debug(FString::Printf(TEXT("OnRegionChanging::%s"), *Region));
	CurrentRegion = Region;

	//加载该站的数据
	LoadRegionFromJson(Region);
	
	FString CurrentData;
	//获取当前站点数据 能获取 不能获取 分别处理
	if (GetRegionData(CurrentRegion,CurrentData))
	{
		TArray<FString> LoadLevelsDate;
		UJsonToolLibrary::GetJsonStringArrayFromJsonString(CurrentData,"LoadMaps",LoadLevelsDate);
		
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

void URegionManager::OnRegionChangingComplete()
{
	UPrintToolLibrary::Debug(FString::Printf(TEXT("OnRegionChangingComplete::%s"), *CurrentRegion));
}

bool URegionManager::GetRegionData(const FString& RegionName, FString& RegionData)
{
	RegionData = RegionDataMap.FindRef(RegionName);

	return !RegionData.IsEmpty();
}

void URegionManager::GetCurrentRegion(FString& Region)
{
	Region = CurrentRegion;
}

bool URegionManager::LoadRegionFromJson(const FString& Region)
{
	UWorld *World = GetWorld();
	if (!World||Region.IsEmpty()) return false;
	USceneManagerSubsystem* SceneManagerSubsystem = USceneManagerSubsystem::Get(World);
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->SetCurrentSceneName(Region);
		return true;
	}
	return false;
}

void URegionManager::ReturnCurrentRegionView()
{
	
}

void URegionManager::ChangedRegion(const FString& Region)
{
	if (RegionDataMap.Contains(Region))
	{
		OnRegionChangingSignature.Broadcast(Region);
	}
	else
	{
		UPrintToolLibrary::Error(FString::Printf(TEXT("ChangedRegion::%s:失败"), *Region));
	}
}
