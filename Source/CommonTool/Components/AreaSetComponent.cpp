// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaSetComponent.h"

#include "CommonTool/SubSystem/SceneManagerSubsystem.h"

// Sets default values for this component's properties
UAreaSetComponent::UAreaSetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...
	
}


// Called when the game starts
void UAreaSetComponent::BeginPlay()
{
	Super::BeginPlay();

	USceneManagerSubsystem::Get(this)->OnAreaSet.AddDynamic(this,&ThisClass::OnAreaSet);
	
}

void UAreaSetComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (GetOwner())
	{
		GetOwner()->SetActorHiddenInGame(true);
	}
}

void UAreaSetComponent::OnAreaSet(const FString& area)
{
	GetOwner()->SetActorHiddenInGame(!(area.Equals(AreaSet)&&!area.IsEmpty()));
}
