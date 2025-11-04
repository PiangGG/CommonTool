// Fill out your copyright notice in the Description page of Project Settings.


#include "LinePath.h"

#include "CesiumGeoreference.h"
#include "InspectionManager.h"
#include "JsonToolLibrary.h"
#include "PrintToolLibrary.h"
#include "SceneManagerSubsystem.h"
#include "StateSubsystem.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

// Sets default values
ALinePath::ALinePath()
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

void ALinePath::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	PathSpawn_SplineMesh();
}

void ALinePath::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	if (RegisterLineActor)
	{
		if (UInspectionManager*InspectionManager = UInspectionManager::Get(this))
		{
			InspectionManager->RegisterLineActor(this);
		}
	}
	
	if (UStateSubsystem* StateSubsystem = UStateSubsystem::Get(this))
	{
		StateSubsystem->SystemStateOnChange.AddDynamic(this,&ThisClass::OnSystemStateOnChange);
		OnSystemStateOnChange(CommonToolTags::State_Null);
	}
}

void ALinePath::OnSystemStateOnChange(FGameplayTag State)
{
	if (State == showTag)
	{
		SetActorHiddenInGame(false);
		SetActorLocation(StartLocation+StartLocationOffsize);
	}
	else
	{
		SetActorLocation(StartLocation);
		SetActorHiddenInGame(true);
	}
}

void ALinePath::PathSpawn_SplineMesh()
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
			
			SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
			SplineMeshComponent->SetStartScale(FVector2D(Wide));
			SplineMeshComponent->SetEndScale(FVector2D(Wide));
			if (Material!=nullptr)
			{
				float start = SplineComponent->GetDistanceAlongSplineAtSplinePoint(i)/SplineComponent->GetSplineLength();
				float end = SplineComponent->GetDistanceAlongSplineAtSplinePoint(i+1)/SplineComponent->GetSplineLength();
				//SplineMeshComponent->SetMaterial(0,Material);
				UMaterialInstanceDynamic* MaterialInstanceDynamic = SplineMeshComponent->CreateDynamicMaterialInstance(0,Material);
				SplineMeshComponent->SetMaterial(0,MaterialInstanceDynamic);
				// MaterialInstanceDynamic->SetScalarParameterValue(FName("TargetLow"),start);
				// MaterialInstanceDynamic->SetScalarParameterValue(FName("TargetHigh"),end);
				// MaterialInstanceDynamics.AddUnique(MaterialInstanceDynamic);
			}
			SplineMeshComponent->SetStartAndEnd(StartLocation,StartTangent,EndLocation,EndTangent);
			SplineMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
			FinishAndRegisterComponent(SplineMeshComponent);
			SplineMeshComponents.Add(SplineMeshComponent);
		}
	}
}

TArray<USplineMeshComponent*> ALinePath::GetSplineMesh()
{
	return SplineMeshComponents;
}

TArray<UMaterialInstanceDynamic*> ALinePath::GetSplineMeshDynamicMaterial()
{
	return MaterialInstanceDynamics;
}

void ALinePath::Draw(ACesiumGeoreference* CesiumGeoreference)
{
	if (CesiumGeoreference && SplineComponent)
	{
		SplineComponent->ClearSplinePoints();
		
		FString Mark_DataJsonString;
		UJsonToolLibrary::GetJsonString(TEXT("Mark_Data"),Mark_DataJsonString,false,"SceneData");
		TArray<FString> Points;
		UJsonToolLibrary::GetJsonStringArrayFromJsonString(Mark_DataJsonString,"data",Points);
		
		TArray<FVector> SplinePoints;
		for (int32 i=0;i<Points.Num();i++)
		{
			double longitude;
			double latitude;
			UJsonToolLibrary::GetDoubleNumFromJsonString(Points[i],"longitude",longitude);
			UJsonToolLibrary::GetDoubleNumFromJsonString(Points[i],"latitude",latitude);
			FVector SplinePoint = CesiumGeoreference->TransformLongitudeLatitudeHeightPositionToUnreal(FVector(longitude,latitude,40.f));
			SplineComponent->AddSplinePoint(SplinePoint,ESplineCoordinateSpace::World);
			SplineComponent->SetSplinePointType(i,ESplinePointType::Curve);
		}
		PathSpawn_SplineMesh();
	}
}

