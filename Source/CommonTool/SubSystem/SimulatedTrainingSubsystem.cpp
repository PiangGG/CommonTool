// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulatedTrainingSubsystem.h"

#include "CommonTool/Library/PrintToolLibrary.h"
#include "CommonTool/Object/SimulatedTrainingQueueStep.h"

USimulatedTrainingSubsystem* USimulatedTrainingSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject && GWorld)
	{
		return UGameInstance::GetSubsystem<USimulatedTrainingSubsystem>(GWorld->GetGameInstance());
	}
	return nullptr;
}

bool USimulatedTrainingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void USimulatedTrainingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString::Printf(TEXT("SimulatedTrainingSubsystem::Initialize")));
}

void USimulatedTrainingSubsystem::Deinitialize()
{
	UPrintToolLibrary::Debug(FString::Printf(TEXT("SimulatedTrainingSubsystem::Initialize")));
	Super::Deinitialize();
}

void USimulatedTrainingSubsystem::ExecuteSimulatedTraining(const TArray<FString>& SimulatedFlows,const FOnSimulatedTrainingQueueComplete& SingleCallBack)
{
	bSimulatedTrainingQueueStep = SimulatedFlows.Num() > 0;
	if (bSimulatedTrainingQueueStep)
	{
		SimulatedTrainingQueueSteps.Empty();

		CurrentOnSimulatedTrainingQueueComplete = SingleCallBack;
		
		OnSimulatedTrainingQueueStateChangeCallBack.AddDynamic(this, &ThisClass::OnSimulatedTrainingQueueStateChange);
		
		for (auto Element : SimulatedFlows)
		{
			USimulatedTrainingQueueStep* SimulatedTrainingQueueStep = NewObject<USimulatedTrainingQueueStep>(
				GetTransientPackage(), USimulatedTrainingQueueStep::StaticClass());
			SimulatedTrainingQueueStep->SimulatedFlowName = Element;
			SimulatedTrainingQueueSteps.Add(SimulatedTrainingQueueStep);
			UPrintToolLibrary::Debug(FString::Printf(TEXT("流程操作：设置当前操作流程(%s)"), *Element));
		}
		StartNextSimulatedTraining();
	}
	else
	{
		UPrintToolLibrary::Error(FString::Printf(TEXT("流程操作设置操作流程为空！")));
	}
}

void USimulatedTrainingSubsystem::StartNextSimulatedTraining()
{
	UPrintToolLibrary::Debug(FString::Printf(TEXT("流程操作：开始绑定当前操作流程")));
	if (SimulatedTrainingQueueSteps.Num() > 0)
	{
		UPrintToolLibrary::Debug(FString::Printf(TEXT("流程操作：绑定当前操作流程成功")));
		GetCurrentSimulatedTrainingQueueStep()->SimulatedTrainingQueue.AddDynamic(
			this, &ThisClass::OnSimulatedTrainingQueue);
	}
	else
	{
		UPrintToolLibrary::Debug(FString::Printf(TEXT("流程操作：绑定当前操作流程失败:没有可绑定流程")));
		EndSimulatedTraining();
	}
}

void USimulatedTrainingSubsystem::RemovePreviousSimulatedTraining()
{
	if (SimulatedTrainingQueueSteps.Num() > 0)
	{
		UPrintToolLibrary::Debug(FString::Printf(TEXT("流程操作：移除上一个流程")));
		SimulatedTrainingQueueSteps.RemoveAt(0);
	}
}

void USimulatedTrainingSubsystem::CompleteCurrentSimulatedTraining(FString SimulatedTrainingNodeName)
{
	if (bSimulatedTrainingQueueStep)
	{
		if (SimulatedTrainingQueueSteps.Num() > 0)
		{
			UPrintToolLibrary::Debug(FString::Printf(TEXT("流程操作：广播当前正在完成的操作")));
			GetCurrentSimulatedTrainingQueueStep()->SimulatedTrainingQueue.Broadcast(SimulatedTrainingNodeName);
		}
		else
		{
			UPrintToolLibrary::Debug(FString::Printf(TEXT("流程操作：当前没有可完成的操作")));
		}
	}
}

void USimulatedTrainingSubsystem::EndSimulatedTraining()
{
	UPrintToolLibrary::Debug(FString::Printf(TEXT("流程操作：结束当前操作流程")));
	SimulatedTrainingQueueSteps.Empty();
	bSimulatedTrainingQueueStep = false;
	CurrentOnSimulatedTrainingQueueComplete.Clear();
}

USimulatedTrainingQueueStep* USimulatedTrainingSubsystem::GetCurrentSimulatedTrainingQueueStep()
{
	if (SimulatedTrainingQueueSteps.Num() > 0)
	{
		return SimulatedTrainingQueueSteps[0];
	}
	return nullptr;
}

void USimulatedTrainingSubsystem::OnSimulatedTrainingQueue(FString SimulatedTrainingNodeName)
{
	if (GetCurrentSimulatedTrainingQueueStep())
	{
		if (SimulatedTrainingNodeName.Equals(GetCurrentSimulatedTrainingQueueStep()->SimulatedFlowName))
		{
			//当完成当前操作不止移除当前步骤，开始下一个步骤
			UPrintToolLibrary::Debug(FString::Printf(TEXT("流程操作：完成当前操作成功")));
			GetCurrentSimulatedTrainingQueueStep()->SimulatedTrainingQueue.RemoveDynamic(
				this, &ThisClass::OnSimulatedTrainingQueue);
			RemovePreviousSimulatedTraining();
			//是否是最后一个步骤 还有后续步骤执行下一步,没有之后完成整个流程
			if (GetCurrentSimulatedTrainingQueueStep())
			{
				StartNextSimulatedTraining();
			}
			else
			{
				OnSimulatedTrainingQueueStateChangeCallBack.Broadcast(SimulatedTrainingNodeName, true,CurrentOnSimulatedTrainingQueueComplete);
			}
		}
		else
		{
			UPrintToolLibrary::Error(FString::Printf(TEXT("流程操作：完成当前操作失败")));
			OnSimulatedTrainingQueueStateChangeCallBack.Broadcast(SimulatedTrainingNodeName, false,CurrentOnSimulatedTrainingQueueComplete);
			EndSimulatedTraining();
		}
	}
}

void USimulatedTrainingSubsystem::OnSimulatedTrainingQueueStateChange(FString SimulatedTrainingNodeName,
                                                                      bool bComplete, const FOnSimulatedTrainingQueueComplete& SingleCallBack)
{
	if (SingleCallBack.IsBound())
	{
		SingleCallBack.Execute(bComplete);
	}
	
	if (bComplete)
	{
		UPrintToolLibrary::Debug(FString::Printf(TEXT("流程操作：流程操作成功")));
	}
	else
	{
		UPrintToolLibrary::Error(FString::Printf(TEXT("流程操作：流程操作失败")));
	}
}
