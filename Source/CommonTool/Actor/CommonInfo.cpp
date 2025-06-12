// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonInfo.h"

#include "CommonToolTags.h"
#include "CommonTool/Library/CommDeveloperSettings.h"
#include "CommonTool/SubSystem/InteractiveSubsystem.h"
#include "CommonTool/SubSystem/StateSubsystem.h"
#include "Components/WidgetComponent.h"

// Sets default values
ACommonInfo::ACommonInfo()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	//改为动态？
	InfoWidget = CreateDefaultSubobject<UWidgetComponent>("InfoWidget");
	InfoWidget->SetupAttachment(Root);
	InfoWidget->SetVisibility(false);
	InfoWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InfoWidget->SetDrawAtDesiredSize(true);
}

// Called when the game starts or when spawned
void ACommonInfo::BeginPlay()
{
	Super::BeginPlay();
	if (const UCommDeveloperSettings* Settings = GetDefault<UCommDeveloperSettings>())
	{
		if (Settings->ActorInfoWidget != nullptr)
		{
			InfoWidget->SetWidgetClass(Settings->ActorInfoWidget);
		}
		OverlayMaterial = Settings->OverlayMaterial.LoadSynchronous();
	}
	
	if (UInteractiveSubsystem* InteractiveSubsystem = UWorld::GetSubsystem<UInteractiveSubsystem>(GWorld))
	{
		InteractiveSubsystem->InteractiveOnClickedSignature.AddDynamic(this, &ThisClass::InteractiveOnClicked);
		InteractiveSubsystem->OnUISectionChange.AddDynamic(this, &ThisClass::SelectedChange);
	}

	if (UStateSubsystem* StateSubsystem = GetGameInstance()->GetSubsystem<UStateSubsystem>())
	{
		StateSubsystem->UserStateOnChange.AddDynamic(this, &ThisClass::OnUserStateOnChange);
		StateSubsystem->SystemStateOnChange.AddDynamic(this, &ThisClass::OnSystemStateOnChange);
	}
}

void ACommonInfo::InteractiveOnClicked(AActor* Actor)
{
	
	if (!OverlayMaterial)
	{
		return;
	}
	if (CurrentInteractiveActor == Actor)
	{
		//通过接口取消对应actor交互
		if (CurrentInteractiveActor != nullptr)
		{
			TArray<UActorComponent*> ActorComponents = CurrentInteractiveActor->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
			for (auto Component : ActorComponents)
			{
				Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(nullptr);
			}
		}
		CurrentInteractiveActor = nullptr;
		InfoWidget->SetVisibility(false);
		return;
	}
	//通过接口取消对应actor交互,执行与新的物品交互逻辑
	if (CurrentInteractiveActor != nullptr)
	{
		TArray<UActorComponent*> ActorComponents = CurrentInteractiveActor->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
		for (auto Component : ActorComponents)
		{
			Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(nullptr);
		}
	}
	CurrentInteractiveActor = Actor;
	if (CurrentInteractiveActor != nullptr)
	{
		TArray<UActorComponent*> ActorComponents = CurrentInteractiveActor->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
		for (auto Component : ActorComponents)
		{
			Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(OverlayMaterial);
		}
	}
	InfoWidget->SetVisibility(true);
}


void ACommonInfo::OnUserStateOnChange(FGameplayTag newState, const FTransform& Transform, float Zoom,bool bblend)
{
	SetActorTransform(Transform);
}

void ACommonInfo::OnSystemStateOnChange(FGameplayTag newState)
{
	if (newState != CommonToolTags::State_Inspection)
	{
		if (CurrentInteractiveActor != nullptr)
		{
			TArray<UActorComponent*> ActorComponents = CurrentInteractiveActor->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
			for (auto Component : ActorComponents)
			{
				Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(nullptr);
			}
		}
		CurrentInteractiveActor = nullptr;
		InfoWidget->SetVisibility(false);
	}
}

void ACommonInfo::SelectedChange(const FEditItem& Item, bool bSelected)
{
	if (CurrentInteractiveActor != nullptr)
	{
		TArray<UActorComponent*> ActorComponents = CurrentInteractiveActor->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
		for (auto Component : ActorComponents)
		{
			Cast<UStaticMeshComponent>(Component)->SetOverlayMaterial(nullptr);
		}
	}
	CurrentInteractiveActor = nullptr;
	InfoWidget->SetVisibility(false);
}