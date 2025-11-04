// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
/**
 * 
 */
class COMMONTOOLCORE_API FSceneManagerRunnable : public FRunnable
{
public:
	FSceneManagerRunnable();
	virtual ~FSceneManagerRunnable() override;

	// FRunnable 接口
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	// 线程控制标志
	bool bStopThread;
};
