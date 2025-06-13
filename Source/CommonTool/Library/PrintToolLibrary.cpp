// Fill out your copyright notice in the Description page of Project Settings.


#include "PrintToolLibrary.h"
#include "CommDeveloperSettings.h"
#include "CommonTool.h"
#include "JsonObjectConverter.h"
#include "Kismet/KismetSystemLibrary.h"

void UPrintToolLibrary::Debug(const FString& Msg)
{
#if !UE_BUILD_SHIPPING
	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
	{
		if (Settings->LogLevel <= 1)
		{
			if (GWorld)
			{
				if (UKismetSystemLibrary::IsDedicatedServer(GWorld))
				{
					UE_LOG(CommonToolLog, Log, TEXT(__FUNCTION__"dedicatedserver:%s:"), *Msg);
				}
				else
				{
					if (UKismetSystemLibrary::IsServer(GWorld))
					{
						UE_LOG(CommonToolLog, Log, TEXT(__FUNCTION__"server:%s"), *Msg);
					}
					else
					{
						UE_LOG(CommonToolLog, Log, TEXT(__FUNCTION__"client:%s:"), *Msg);
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
	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
	{
		if (Settings->LogLevel <= 2)
		{
			if (GWorld)
			{
				if (UKismetSystemLibrary::IsDedicatedServer(GWorld))
				{
					UE_LOG(CommonToolLog, Warning, TEXT(__FUNCTION__"dedicatedserver:%s"), *Msg);
				}
				else
				{
					if (UKismetSystemLibrary::IsServer(GWorld))
					{
						UE_LOG(CommonToolLog, Warning, TEXT(__FUNCTION__"server:%s"), *Msg);
					}
					else
					{
						UE_LOG(CommonToolLog, Warning, TEXT(__FUNCTION__"client:%s:"), *Msg);
					}
				}
			}
		}
	}
#endif
}

void UPrintToolLibrary::Error(const FString& Msg)
{
	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
	{
		if (Settings->LogLevel <= 3)
		{
			if (GWorld)
			{
				if (UKismetSystemLibrary::IsDedicatedServer(GWorld))
				{
					UE_LOG(CommonToolLog, Error, TEXT(__FUNCTION__"dedicatedserver:%s"), *Msg);
				}
				else
				{
					if (UKismetSystemLibrary::IsServer(GWorld))
					{
						UE_LOG(CommonToolLog, Error, TEXT(__FUNCTION__"server:%s"), *Msg);
					}
					else
					{
						UE_LOG(CommonToolLog, Error, TEXT(__FUNCTION__"client:%s"), *Msg);
					}
				}
			}
		}
	}
}
