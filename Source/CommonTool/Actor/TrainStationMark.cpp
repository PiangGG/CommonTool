// Fill out your copyright notice in the Description page of Project Settings.


#include "TrainStationMark.h"

// Sets default values
ATrainStationMark::ATrainStationMark()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

}

// Called when the game starts or when spawned
void ATrainStationMark::BeginPlay()
{
	Super::BeginPlay();
	
}
