// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherManager.h"

#include "HttpSubSystem.h"
#include "PrintToolLibrary.h"

UWeatherManager* UWeatherManager::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return UWorld::GetSubsystem<UWeatherManager>(WorldContextObject->GetWorld());
	}
	return nullptr;
}

bool UWeatherManager::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UWeatherManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("UWeatherManager::Initialize"));
}

void UWeatherManager::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("UWeatherManager::Deinitialize"));
	Super::Deinitialize();
	
}

void UWeatherManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	SimulateCheckSignature.AddDynamic(this,&ThisClass::OnSimulateCheck);

	if (UHttpSubSystem* HttpSubSystem = UHttpSubSystem::Get(this))
	{
		FString WeatherURL = "https://restapi.amap.com/v3/weather/weatherInfo?city=420100&key=7eff8b66bdcb8c275912e09b3a882854";
		//UJsonToolLibrary::GetStringFromJsonString(ConfigSubSystem->GetCfgData(),"UnitPersonData",UnitPersonDataURL);
		FString Data;
		FHttpSingleCallBack SingleCallBack;
		// SingleCallBack.BindDynamic(this,&ThisClass::GetWeatherDataResult);
		// HttpSubSystem->HttpGetCallBack(WeatherURL,Data,SingleCallBack);
	}
}

void UWeatherManager::ChangeWeatherState(const FGameplayTag& NewState)
{
	if (NewState != CurrentWeatherStateTag)
	{
		CurrentWeatherStateTag = NewState;

		UPrintToolLibrary::Debug(FString("UWeatherManager::ChangeWeatherState: ") + CurrentWeatherStateTag.ToString());
		WeatherChangeSignature.Broadcast(CurrentWeatherStateTag);
	}
}

void UWeatherManager::GetWeatherState(FGameplayTag& CurState)
{
	CurState = CurrentWeatherStateTag;
}

void UWeatherManager::ChangeTime(const float& time)
{
	CurrentTime = time;
	TimeChangeSignature.Broadcast(CurrentTime);
}

float UWeatherManager::GetTime()
{
	return CurrentTime;
}

void UWeatherManager::SetSimulateCheck(const bool& Simulate)
{
	bSimulate = Simulate;

	SimulateCheckSignature.Broadcast(bSimulate);
}

bool UWeatherManager::GetSimulateCheck()
{
	return bSimulate;
}

void UWeatherManager::OnSimulateCheck(bool SimulateCheck)
{
	if (SimulateCheck)
	{
		
	}
	else
	{
		WeatherChangeSignature.Broadcast(CurrentWeatherStateTag);
		TimeChangeSignature.Broadcast(CurrentTime);
	}
}

void UWeatherManager::GetWeatherDataResult(const FString& JsonStr)
{
	WeatherData = JsonStr;
}

void UWeatherManager::GetWeatherData(FString& JsonStr)
{
	JsonStr = WeatherData;
}
