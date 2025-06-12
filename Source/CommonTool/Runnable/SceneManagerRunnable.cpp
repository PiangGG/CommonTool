// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneManagerRunnable.h"
#include "CommonTool/Library/ToolFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

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
	UToolFunctionLibrary::Debug(FString::Printf(TEXT("SceneManagerRunnable::Init")));
	return true;
}

uint32 FSceneManagerRunnable::Run()
{
	while (!bStopThread)
	{
		UToolFunctionLibrary::Debug(FString::Printf(TEXT("SceneManagerRunnable::Run")));
		// 在游戏线程中创建 Actor
	}
	return 0;
}

void FSceneManagerRunnable::Stop()
{
	bStopThread = true;
	UToolFunctionLibrary::Debug(FString::Printf(TEXT("SceneManagerRunnable::Stop")));
}

void FSceneManagerRunnable::Exit()
{
	UToolFunctionLibrary::Debug(FString::Printf(TEXT("SceneManagerRunnable::Exit")));
}
