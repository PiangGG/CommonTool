// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneManagerRunnable.h"
#include "PrintToolLibrary.h"

FSceneManagerRunnable::FSceneManagerRunnable()
{
	bStopThread = false;
}

FSceneManagerRunnable::~FSceneManagerRunnable()
{
	Stop();
}

bool FSceneManagerRunnable::Init()
{
	UPrintToolLibrary::Debug(FString::Printf(TEXT("SceneManagerRunnable::Init")));
	return true;
}

uint32 FSceneManagerRunnable::Run()
{
	while (!bStopThread)
	{
		UPrintToolLibrary::Debug(FString::Printf(TEXT("SceneManagerRunnable::Run")));
		// 在游戏线程中创建 Actor
	}
	return 0;
}

void FSceneManagerRunnable::Stop()
{
	bStopThread = true;
	UPrintToolLibrary::Debug(FString::Printf(TEXT("SceneManagerRunnable::Stop")));
}

void FSceneManagerRunnable::Exit()
{
	UPrintToolLibrary::Debug(FString::Printf(TEXT("SceneManagerRunnable::Exit")));
}
