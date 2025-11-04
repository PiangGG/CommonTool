// Fill out your copyright notice in the Description page of Project Settings.


#include "EmergencyLinePath.h"

#include "InspectionManager.h"
#include "SceneManagerSubsystem.h"
#include "StateSubsystem.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

// Sets default values
AEmergencyLinePath::AEmergencyLinePath()
{
	//Set default values
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;

	//Set root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Movable);
	RootComponent = Root;

	//Set SplineComponent
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetMobility(EComponentMobility::Movable);
	SplineComponent->SetupAttachment(RootComponent);
}

void AEmergencyLinePath::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	PathSpawn_SplineMesh();
}

void AEmergencyLinePath::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	
	if (UStateSubsystem* StateSubsystem = UStateSubsystem::Get(this))
	{
		StateSubsystem->SystemStateOnChange.AddDynamic(this,&ThisClass::OnSystemStateOnChange);
		OnSystemStateOnChange(CommonToolTags::State_Null);
	}
}

void AEmergencyLinePath::OnSystemStateOnChange(FGameplayTag State)
{
	if (State == CommonToolTags::State_Emergency)
	{
		SetActorHiddenInGame(false);
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

void AEmergencyLinePath::PathSpawn_SplineMesh()
{
	for (auto Element : SplineMeshComponents)
	{
		if (Element)
		{
			Element->DestroyComponent();
		}
	}
	
	SplineMeshComponents.Empty();
	MaterialInstanceDynamics.Empty();
	
	if (SplineComponent && StaticMesh)
	{
		int32 SpawnNum = SplineComponent->GetNumberOfSplinePoints()-1;
		for (int32 i= 0;i< SpawnNum;i++)
		{
			FVector StartLocation,StartTangent;
			SplineComponent->GetLocationAndTangentAtSplinePoint(i,StartLocation,StartTangent,ESplineCoordinateSpace::Local);
			FVector EndLocation,EndTangent;
			SplineComponent->GetLocationAndTangentAtSplinePoint(i+1,EndLocation,EndTangent,ESplineCoordinateSpace::Local);
			
			USplineMeshComponent* SplineMeshComponent;
			SplineMeshComponent = NewObject<USplineMeshComponent>(this);
			SplineMeshComponent->CreationMethod = EComponentCreationMethod::SimpleConstructionScript;
			SplineMeshComponent->bAutoRegister = true;
			SplineMeshComponent->SetupAttachment(RootComponent);
			SplineMeshComponent->SetNetAddressable();
			if (StaticMesh)
			{
				SplineMeshComponent->SetStaticMesh(StaticMesh);
			}
			SplineMeshComponent->SetMobility(EComponentMobility::Movable);
			//Element.bInteractive = true;
			
			SplineMeshComponent->SetCollisionProfileName(FName(TEXT("Interactive")));
			
			SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
			SplineMeshComponent->SetStartScale(FVector2D(Wide));
			SplineMeshComponent->SetEndScale(FVector2D(Wide));
			if (Material!=nullptr)
			{
				float start = SplineComponent->GetDistanceAlongSplineAtSplinePoint(i)/SplineComponent->GetSplineLength();
				float end = SplineComponent->GetDistanceAlongSplineAtSplinePoint(i+1)/SplineComponent->GetSplineLength();
				
				UMaterialInstanceDynamic* MaterialInstanceDynamic = SplineMeshComponent->CreateDynamicMaterialInstance(0,Material);
				MaterialInstanceDynamic->SetScalarParameterValue(FName("TargetLow"),start);
				MaterialInstanceDynamic->SetScalarParameterValue(FName("TargetHigh"),end);
				MaterialInstanceDynamics.AddUnique(MaterialInstanceDynamic);
			}
			SplineMeshComponent->SetStartAndEnd(StartLocation,StartTangent,EndLocation,EndTangent);
			SplineMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
			FinishAndRegisterComponent(SplineMeshComponent);
			SplineMeshComponents.Add(SplineMeshComponent);
		}
	}
}

TArray<USplineMeshComponent*> AEmergencyLinePath::GetSplineMesh()
{
	return SplineMeshComponents;
}

TArray<UMaterialInstanceDynamic*> AEmergencyLinePath::GetSplineMeshDynamicMaterial()
{
	return MaterialInstanceDynamics;
}

