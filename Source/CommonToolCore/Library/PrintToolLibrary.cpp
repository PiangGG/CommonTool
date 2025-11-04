// Fill out your copyright notice in the Description page of Project Settings.


#include "PrintToolLibrary.h"
#include "CommonToolCore.h"
#include "CommonSettings.h"
#include "Kismet/KismetSystemLibrary.h"

void UPrintToolLibrary::Debug(const FString& Msg)
{
#if !UE_BUILD_SHIPPING
	if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
	{
		if (CommonSettings->LogLevel <= 1)
		{
			if (GWorld)
			{
				if (UKismetSystemLibrary::IsDedicatedServer(GWorld))
				{
					UE_LOG(CommonToolCoreLog, Log, TEXT(__FUNCTION__"dedicatedserver:%s:"), *Msg);
				}
				else
				{
					if (UKismetSystemLibrary::IsServer(GWorld))
					{
						UE_LOG(CommonToolCoreLog, Log, TEXT(__FUNCTION__"server:%s"), *Msg);
					}
					else
					{
						UE_LOG(CommonToolCoreLog, Log, TEXT(__FUNCTION__"client:%s:"), *Msg);
					}
				}
			}
		}
	}
#endif
}

void UPrintToolLibrary::Waring(const FString& Msg)
{
#if !UE_BUILD_SHIPPING
	if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
	{
		if (CommonSettings->LogLevel <= 2)
		{
			if (GWorld)
			{
				if (UKismetSystemLibrary::IsDedicatedServer(GWorld))
				{
					UE_LOG(CommonToolCoreLog, Warning, TEXT(__FUNCTION__"dedicatedserver:%s"), *Msg);
				}
				else
				{
					if (UKismetSystemLibrary::IsServer(GWorld))
					{
						UE_LOG(CommonToolCoreLog, Warning, TEXT(__FUNCTION__"server:%s"), *Msg);
					}
					else
					{
						UE_LOG(CommonToolCoreLog, Warning, TEXT(__FUNCTION__"client:%s:"), *Msg);
					}
				}
			}
		}
	}
#endif
}

void UPrintToolLibrary::Error(const FString& Msg)
{
	if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
	{
		if (CommonSettings->LogLevel <= 3)
		{
			if (GWorld)
			{
				if (UKismetSystemLibrary::IsDedicatedServer(GWorld))
				{
					UE_LOG(CommonToolCoreLog, Error, TEXT(__FUNCTION__"dedicatedserver:%s"), *Msg);
				}
				else
				{
					if (UKismetSystemLibrary::IsServer(GWorld))
					{
						UE_LOG(CommonToolCoreLog, Error, TEXT(__FUNCTION__"server:%s"), *Msg);
					}
					else
					{
						UE_LOG(CommonToolCoreLog, Error, TEXT(__FUNCTION__"client:%s"), *Msg);
					}
				}
			}
		}
	}
}

bool UPrintToolLibrary::PrintDebug()
{
	if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
	{
		return CommonSettings->bPrintDebug;	
	}
	return false;
}

bool UPrintToolLibrary::DrawDebug()
{
	if (const UCommonSettings* CommonSettings = GetDefault<UCommonSettings>())
	{
		return CommonSettings->bDrawDebug;
	}
	return false;
}
