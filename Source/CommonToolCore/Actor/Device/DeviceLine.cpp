// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceLine.h"

#include "CommonToolCoreTags.h"
#include "DeviceEditPoint.h"
#include "CommonToolCore/SubSystem/SceneManagerSubsystem.h"
#include "CommonToolCore/SubSystem/StateSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ADeviceLine::ADeviceLine()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

void ADeviceLine::BeginPlay()
{
	Super::BeginPlay();
	
	if (Locations.Num()<1)
	{
		Locations.AddUnique(GetActorLocation());
		AddPointLocation(GetActorLocation());
		ReSetLocations(Locations);
		GetOnActorInitializationCompletedDelegateChecked().AddDynamic(this, &ThisClass::OnActorInitializationCompleted);
	}else
	{
		for (auto Element : Locations)
		{
			AddPointLocation(Element);
		}
		ReSetLocations(Locations);
	}
	UStateSubsystem * StateSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UStateSubsystem>();
	if (StateSubsystem)
	{
		FGameplayTag SystemState;
		StateSubsystem->GetSystemState(SystemState);
		for (auto Element : DeviceEditPoint)
		{
			Element->SetActorHiddenInGame(SystemState != CommonToolTags::State_Edit);
		}
	}
}

void ADeviceLine::AddHitData_Implementation(FHitResult HitResult)
{
	Super::AddHitData_Implementation(HitResult);
	Locations.AddUnique(HitResult.Location);
	AddPointLocation(HitResult.Location);
	ReSetLocations(Locations);
}

void ADeviceLine::OnActorInitializationCompleted(UObject* Object)
{
	
}

void ADeviceLine::AddPointLocation(FVector Location)
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

TSharedRef<FJsonObject> ADeviceLine::GetDeviceJsonObjectData(const FString &GUID)
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
void ADeviceLine::SetDeviceJsonObjectData(TSharedRef<FJsonObject> JsonObject)
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

void ADeviceLine::SetEditing(bool edit)
{
	Super::SetEditing(edit);

	for (auto Element : DeviceEditPoint)
	{
		Element->SetActorHiddenInGame(!edit);
	}
}

void ADeviceLine::ReDraw()
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

void ADeviceLine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto Element : DeviceEditPoint)
	{
		Element->Destroy();
	}
	Super::EndPlay(EndPlayReason);
}

void ADeviceLine::ReSetLocations_Implementation(const TArray<FVector>& locations)
{
	
}
