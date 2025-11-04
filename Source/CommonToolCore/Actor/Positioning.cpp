// Fill out your copyright notice in the Description page of Project Settings.


#include "Positioning.h"

#include "CommonToolCore/SubSystem/SceneManagerSubsystem.h"

// Sets default values
APositioning::APositioning()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

}

// Called when the game starts or when spawned
void APositioning::BeginPlay()
{
	Super::BeginPlay();

	//USceneManagerSubsystem::Get(this)->AddPositionings(this);
}

