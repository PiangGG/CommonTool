// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceVideoDecal.h"

#include "CommonToolTags.h"
#include "FileMediaSource.h"
#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "MediaTexture.h"
#include "StreamMediaSource.h"
#include "CommonTool/Library/PrintToolLibrary.h"
#include "CommonTool/SubSystem/SceneManagerSubsystem.h"
#include "Engine/DecalActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"

ADeviceVideoDecal::ADeviceVideoDecal()
{
	
	FTransform Result = GetActorTransform();
	FRotator Rotator = Result.GetRotation().Rotator();
	Rotator += FRotator(-89.0f, 0.0f, 0);
	Result.SetRotation(Rotator.Quaternion());
	SetActorTransform(Result);
}

void ADeviceVideoDecal::BeginPlay()
{
	Super::BeginPlay();
	
	GetOnActorInitializationCompletedDelegateChecked().AddDynamic(this, &ThisClass::OnActorInitializationCompleted);
	ActorInitializationCompleted(this);
}

void ADeviceVideoDecal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DecalActor)
	{
		DecalActor->Destroy();
	}
	DecalActor = nullptr;
	Super::EndPlay(EndPlayReason);
}

AActor* ADeviceVideoDecal::GetActor_Implementation()
{
	return DecalActor?Cast<AActor>(DecalActor):this;;
}


TSharedRef<FJsonObject> ADeviceVideoDecal::GetDeviceJsonObjectData(const FString &GUID)
{
	TSharedRef<FJsonObject> ActorJson = Super::GetDeviceJsonObjectData(GUID);
	ActorJson->SetStringField("VideoPath",VideoPath);
	ActorJson->SetStringField("Option",Option);
	return ActorJson;
}

void ADeviceVideoDecal::SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject)
{
	Super::SetDeviceJsonObjectData(JsonObject);

	JsonObject->TryGetStringField(TEXT("VideoPath"),VideoPath);
	JsonObject->TryGetStringField(TEXT("Option"),Option);
}

void ADeviceVideoDecal::SetDeviceLoad_Implementation(const FString& bload)
{
	Super::SetDeviceLoad_Implementation(bload);
	
	if (DecalActor)
	{
		DecalActor->SetActorHiddenInGame(!BIsLoad);
	}
}

void ADeviceVideoDecal::OnActorInitializationCompleted(UObject* Object)
{
	if (!MediaPlayer)
	{
		MediaPlayer = CreateAndSetupMediaPlayer();
	}
	
	if (BIsLoad)
	{
		if (Option.Equals("LocalVideo"))
		{
			PlayLocalVideo(VideoPath);
		}
		else
		{
			PlayStream(VideoPath);
		}
		SetupMediaOutputs(MediaTexture,MediaSound);

		//设置
		if (!MaterialInstanceDynamic)
		{
			if (MaterialInterface)
			{
				MaterialInstanceDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this,MaterialInterface);
				MaterialInstanceDynamic->SetTextureParameterValue(FName("MPTTexture"),GetMediaTexture());
				
				if (!DecalActor)
				{
					DecalActor = Cast<ADecalActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(),ADecalActor::StaticClass(), GetActorTransform(),ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
					UGameplayStatics::FinishSpawningActor(DecalActor, GetActorTransform());
					DecalActor->SetDecalMaterial(MaterialInstanceDynamic);
					DecalActor->SetActorTransform(GetActorTransform());
					DecalActor->SetActorHiddenInGame(false);
				}
				else
				{
					DecalActor->SetDecalMaterial(MaterialInstanceDynamic);
					DecalActor->SetActorTransform(GetActorTransform());
					DecalActor->SetActorHiddenInGame(false);
				}
			}
		}
	}
	else
	{
		MediaPlayer->Pause();
		MediaPlayer->Close();
		MediaTexture = nullptr;
		if (MediaSound)
		{
			MediaSound->DestroyComponent();
			MediaSound = nullptr;
		}
		if (DecalActor)
		{
			DecalActor->SetActorHiddenInGame(true);
		}
	}
}

void ADeviceVideoDecal::ReDraw()
{
	Super::ReDraw();

	if (DecalActor)
	{
		DecalActor->SetActorTransform(GetActorTransform());
	}
}

UMediaPlayer* ADeviceVideoDecal::CreateAndSetupMediaPlayer()
{
	// 创建MediaPlayer对象
	 UMediaPlayer* OutMediaPlayer = NewObject<UMediaPlayer>(GetTransientPackage(), NAME_None, RF_Transient);
    
	if (!OutMediaPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create MediaPlayer!"));
		return nullptr;
	}

	// 设置基本属性
	OutMediaPlayer->SetLooping(false); // 是否循环播放
	OutMediaPlayer->PlayOnOpen = true; // 打开后自动播放
    
	return OutMediaPlayer;
}

void ADeviceVideoDecal::PlayLocalVideo(const FString& FilePath)
{
	if (!MediaPlayer) return;

	// 创建FileMediaSource
	UFileMediaSource* MediaSource = NewObject<UFileMediaSource>();
	MediaSource->FilePath = FilePath; // 如："C:/Videos/Test.mp4" 或 "Game/Videos/Test.mp4"（需放在Content/Videos下）

	// 打开媒体源
	if (!MediaPlayer->OpenSource(MediaSource))
	{
		UPrintToolLibrary::Debug(FString("Failed to open media source!"));
	}
	else
	{
		MediaPlayer->Play();
	}
}

void ADeviceVideoDecal::PlayStream(const FString& StreamUrl)
{
	if (!MediaPlayer) return;

	// 创建StreamMediaSource
	UStreamMediaSource* MediaSource = NewObject<UStreamMediaSource>();
	MediaSource->StreamUrl = StreamUrl; // 如："rtsp://example.com/stream" 或 "http://example.com/video.m3u8"

	// 打开媒体源
	if (!MediaPlayer->OpenSource(MediaSource))
	{
		UPrintToolLibrary::Debug(FString("Failed to open stream!"));
	}else
	{
		MediaPlayer->Play();
	}
}

void ADeviceVideoDecal::SetupMediaOutputs(UMediaTexture*& OutMediaTexture, UMediaSoundComponent*& OutMediaSound)
{
	if (!MediaPlayer) return;

	// 创建MediaTexture用于在材质中显示视频
	OutMediaTexture = NewObject<UMediaTexture>();
	OutMediaTexture->SetMediaPlayer(MediaPlayer);
	OutMediaTexture->UpdateResource();
	
	// 创建MediaSoundComponent用于播放音频
	// OutMediaSound = NewObject<UMediaSoundComponent>();
	// OutMediaSound->SetMediaPlayer(MediaPlayer);
	// OutMediaSound->RegisterComponent();
}

FTransform ADeviceVideoDecal::GetDecalTransform() const
{
	FTransform Result = GetActorTransform();
	FRotator Rotator = Result.GetRotation().Rotator();
	Rotator += FRotator(-89.0f, 0.0f, 0);
	Result.SetRotation(Rotator.Quaternion());
	return Result;
}



UMediaPlayer* ADeviceVideoDecal::GetMediaPlayer() const
{
	return MediaPlayer;
}

UMediaTexture* ADeviceVideoDecal::GetMediaTexture() const
{
	return MediaTexture;
}
