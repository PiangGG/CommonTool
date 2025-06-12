// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceSurface.h"
#include "CommonToolTags.h"
#include "DeviceEditPoint.h"
#include "CommonTool/SubSystem/StateSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ADeviceSurface::ADeviceSurface()
{
	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
}

void ADeviceSurface::BeginPlay()
{
	Super::BeginPlay();
	
	if (Locations.Num()<1)
	{
		Locations.AddUnique(GetActorLocation()+FVector(0,0,1));
		AddPointLocation(GetActorLocation());
		ReSetLocations(Locations);
		GetOnActorInitializationCompletedDelegateChecked().AddDynamic(this, &ThisClass::OnActorInitializationCompleted);
	}
	else
	{
		for (auto Element : Locations)
		{
			AddPointLocation(Element);
		}
		ReSetLocations(Locations);
	}
	
	if (UStateSubsystem * StateSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UStateSubsystem>())
	{
		FGameplayTag SystemState;
		StateSubsystem->GetSystemState(SystemState);
		for (auto Element : DeviceEditPoint)
		{
			Element->SetActorHiddenInGame(SystemState != CommonToolTags::State_Edit);
		}
	} 
}

void ADeviceSurface::AddHitData_Implementation(FHitResult HitResult)
{
	Super::AddHitData_Implementation(HitResult);
	Locations.AddUnique(HitResult.Location);
	AddPointLocation(HitResult.Location);
	ReSetLocations(Locations);
}

void ADeviceSurface::OnActorInitializationCompleted(UObject* Object)
{
	
}

void ADeviceSurface::AddPointLocation(FVector Location)
{
	FTransform TransformLocal = UKismetMathLibrary::MakeTransform(FVector(Location),FRotator(0.0, 0.0, 0.0),FVector(0.5));
	ADeviceEditPoint * SpawndActor = Cast<ADeviceEditPoint>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld->GetWorld(),
										ADeviceEditPoint::StaticClass(), TransformLocal,
										ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	UGameplayStatics::FinishSpawningActor(SpawndActor, TransformLocal);
	//SpawndActor->AttachToActor(this, FAttachmentTransformRules( EAttachmentRule::KeepRelative, true ));
	SpawndActor->ParentActor = this;
	if (SpawndActor)
	{
		DeviceEditPoint.AddUnique(SpawndActor);
	}
}

TSharedRef<FJsonObject> ADeviceSurface::GetDeviceJsonObjectData(const FString &GUID)
{
	TSharedRef<FJsonObject> JsonObject = Super::GetDeviceJsonObjectData(GUID);
	TArray<TSharedPtr<FJsonValue>> ArrayValues;
	Locations.Empty();
	
	for (auto Element : DeviceEditPoint)
	{
		FVector PointLocation = Element->GetActorLocation();
		Locations.AddUnique(PointLocation);
	}
	
	for (auto Location : Locations)
	{
		ArrayValues.Add(MakeShared<FJsonValueString>(Location.ToString()));
	}
	JsonObject->SetArrayField(TEXT("Locations"),ArrayValues);
	return JsonObject;
}

void ADeviceSurface::SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject)
{
	Super::SetDeviceJsonObjectData(JsonObject);
	const TArray<TSharedPtr<FJsonValue>>* ArrayValues;
	
	if (JsonObject->TryGetArrayField(TEXT("Locations"),ArrayValues))
	{
		for (const TSharedPtr<FJsonValue>& JsonValue : *ArrayValues)
		{
			FString ActorTransform;
			FVector Location;
			if (Location.InitFromString(JsonValue->AsString()))
			{
				Locations.AddUnique(Location);
			}
		}
	}
	ReSetLocations(Locations);
}

 void ADeviceSurface::ReDraw()
{
	Super::ReDraw();
	Locations.Empty();
	for (auto ChildActor : DeviceEditPoint)
	{
		FVector PointLocation = ChildActor->GetActorLocation();
		Locations.AddUnique(PointLocation);
	}
	ReSetLocations(Locations);
}

void ADeviceSurface::SetEditing(bool edit)
{
	Super::SetEditing(edit);

	for (auto Element : DeviceEditPoint)
	{
		Element->SetActorHiddenInGame(!edit);
	}
}

void ADeviceSurface::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto Element : DeviceEditPoint)
	{
		Element->Destroy();
	}
	Super::EndPlay(EndPlayReason);
}

void ADeviceSurface::ReSetLocations_Implementation(const TArray<FVector>& locations)
{
	
}