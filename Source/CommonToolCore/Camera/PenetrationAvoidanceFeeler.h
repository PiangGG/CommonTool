// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PenetrationAvoidanceFeeler.generated.h"
/**
 * 
 */
USTRUCT()
struct FPenetrationAvoidanceFeeler
{
	GENERATED_BODY()

	/*描述偏离主射线的旋转体*/
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	FRotator AdjustmentRot;

	/**如果这个触角碰到世界，它会对最终位置产生多大影响*/
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float WorldWeight;

	/**如果触针碰到APawn，它对最终位置的影响有多大（设置为0根本不会尝试与典当碰撞）*/
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float PawnWeight;

	/**追踪此触角时用于碰撞的范围*/
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float Extent;

	/**如果上一帧没有碰到任何东西，则使用此触角测量轨迹之间的最小帧间隔*/
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	int32 TraceInterval;

	/**自使用此探测器以来的帧数*/
	UPROPERTY(transient)
	int32 FramesUntilNextTrace;


	FPenetrationAvoidanceFeeler()
		: AdjustmentRot(ForceInit)
		  , WorldWeight(0)
		  , PawnWeight(0)
		  , Extent(0)
		  , TraceInterval(0)
		  , FramesUntilNextTrace(0)
	{
	}

	FPenetrationAvoidanceFeeler(const FRotator& InAdjustmentRot,
	                            const float& InWorldWeight,
	                            const float& InPawnWeight,
	                            const float& InExtent,
	                            const int32& InTraceInterval = 0,
	                            const int32& InFramesUntilNextTrace = 0)
		: AdjustmentRot(InAdjustmentRot)
		  , WorldWeight(InWorldWeight)
		  , PawnWeight(InPawnWeight)
		  , Extent(InExtent)
		  , TraceInterval(InTraceInterval)
		  , FramesUntilNextTrace(InFramesUntilNextTrace)
	{
	}
};
