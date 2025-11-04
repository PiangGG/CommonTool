// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceGenerator.h"

#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "EditorLevelUtils.h"
#include "EngineUtils.h"
#include "FileHelpers.h"
#include "IAssetTools.h"
#include "InspectionManager.h"
#include "JsonToolLibrary.h"
#include "CommonToolCore/Components/DeviceMarkComponent.h"
#include "CommonToolCore/Components/RegionMarkComponent.h"
#include "Factories/WorldFactory.h"

// Sets default values
ADeviceGenerator::ADeviceGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADeviceGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeviceGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#if WITH_EDITOR

bool ADeviceGenerator::GetGenerateDeviceLocation(FVector& Vector)
{
	for (AActor* Actor : TActorRange<AActor>(GWorld))
	{
		if (URegionMarkComponent *RegionMarkComponent = Actor->FindComponentByClass<URegionMarkComponent>())
		{
			if (!SaveLevelName.IsEmpty() && SaveLevelName.Equals(RegionMarkComponent->GetRegionID()))
			{
				 Vector = Actor->GetActorLocation();
				return true;
			}
		}
	}
	return false;
}
void ADeviceGenerator::GenerateDevice()
{
	FVector Location;
	if (GetGenerateDeviceLocation(Location)&&!SaveLevelName.IsEmpty())
	{
		CreateNewLevelAndSpawnActor(SaveLevelName, RoundOffSizeLocation);
	}
}

void ADeviceGenerator::SpawnActorToExistingLevel(const FString& LevelName, const FVector& Location)
{
	FString LevelPath = FString::Printf(TEXT("/Game/%s/%s.%s"),*SavePath, *LevelName, *LevelName);
	FSoftObjectPath LevelRef(LevelPath);
	UWorld* ExistingWorld = Cast<UWorld>(LevelRef.TryLoad());
    
	SpawnActors(LevelName, Location, ExistingWorld);
}

void ADeviceGenerator::CreateNewLevelAndSpawnActor(const FString& LevelName, const FVector& SpawnLocation)
{
	 // 1. 获取AssetTools模块
    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
    IAssetTools& AssetTools = AssetToolsModule.Get();

    // 2. 构建完整关卡路径
    FString PackagePath = FString::Printf(TEXT("/Game/%s/%s"),*SavePath, *LevelName);
    FString PackageName = FString::Printf(TEXT("/Game/%s/%s.%s"),*SavePath, *LevelName, *LevelName);

    // 3. 检查关卡是否已存在
    if (FPackageName::DoesPackageExist(PackagePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("关卡已存在: %s"), *PackageName);
        SpawnActorToExistingLevel(LevelName, SpawnLocation);
        return;
    }

    // 4. 创建新关卡
    UWorldFactory* WorldFactory = NewObject<UWorldFactory>();
    WorldFactory->WorldType = EWorldType::Inactive; // 编辑器关卡
	//TEXT("/Game/SavePath")
	FString Path = FString::Printf(TEXT("/Game/%s"), *SavePath);
    UObject* NewLevelAsset = AssetTools.CreateAsset(
        LevelName,          // 资产名称
        Path, // 包路径
        UWorld::StaticClass(), // 资产类
        WorldFactory        // 工厂
    );

    if (!NewLevelAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("创建关卡失败: %s"), *LevelName);
        return;
    }

    // 5. 加载新创建的关卡到编辑器
    UWorld* NewWorld = Cast<UWorld>(NewLevelAsset);
    if (NewWorld)
    {
    	// 添加到编辑器世界（作为子关卡）
    	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
    	ULevel* TargetLevel = nullptr;
    	if (NewWorld)
    	{
    		TargetLevel = NewWorld->PersistentLevel;
    		EditorWorld->AddLevel(TargetLevel); // 添加到当前编辑器世界
    	}
    	
    	SpawnActors(LevelName, SpawnLocation, NewWorld);
        // if (TargetLevel && DeviceActorClass)
        // {
        //     // 6. 在新关卡中生成Actor
        //     FActorSpawnParameters SpawnParams;
        //     SpawnParams.OverrideLevel = NewWorld->PersistentLevel;
        //
        //     AActor* NewActor = EditorWorld->SpawnActor<AActor>(
        //         DeviceActorClass, // 替换为你的Actor类
        //         SpawnLocation,
        //         FRotator::ZeroRotator,
        //         SpawnParams
        //     );
        //
        //     if (NewActor)
        //     {
        //         // 7. 保存新关卡
        //         TArray<UPackage*> PackagesToSave;
        //         PackagesToSave.Add(NewWorld->GetOutermost());
        //     	FEditorFileUtils::EPromptReturnCode RetValue = FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false);
        //
        //         UE_LOG(LogTemp, Warning, TEXT("新关卡创建成功并生成Actor: %s"), *LevelName);
        //     }
        // }
    }
}

void ADeviceGenerator::SpawnActors(const FString& LevelName, const FVector& SpawnLocation, UWorld* SpawnWorld)
{
	InspectionManager = UInspectionManager::Get(this);

	if (!InspectionManager)
	{
		return;
	}

	// 尝试查找已加载的关卡
	ULevel* TargetLevel = nullptr;
	for (ULevel* Level : SpawnWorld->GetLevels())
	{
		if (Level->GetOuter()->GetName() == LevelName)
		{
			TargetLevel = Level;
			break;
		}
	}
	
	FVector ResultLocation;
	FVector ResultDirection;
	InspectionManager->GetInspectionLineLocationAndDirection(SpawnLocation, ResultLocation, ResultDirection);
	if (TargetLevel && DeviceActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.OverrideLevel = TargetLevel;
		
		for (auto Actor : TargetLevel->Actors)
		{
			if (Actor->IsA(DeviceActorClass))
			{
				Actor->Destroy();
			}
		}
		
		Actors.Empty();
		
		//TODO 需要根据样条线来生成 还是根据LCC的坐标来
		//int32 NeedSpawn = Paragraphs*2 - Actors.Num();
		for (int32 i = 0;i< Paragraphs;i++)
		{
			FVector TargetLocation_Left, TargetLocation_Right;
			FVector ParagraphsOffSize;
			ParagraphsOffSize = FVector(1,0,0)*(Spacing * i);
			
			TargetLocation_Left = TransformLocalOffsetToWorld(SpawnLocation,ResultDirection,ParagraphsOffSize+FVector(PitchOffSizeLocation.X,PitchOffSizeLocation.Y,PitchOffSizeLocation.Z));
			TargetLocation_Right  = TransformLocalOffsetToWorld(SpawnLocation,ResultDirection,ParagraphsOffSize+FVector(PitchOffSizeLocation.X,-PitchOffSizeLocation.Y,PitchOffSizeLocation.Z));

			AActor* EditorWorldSpawnActor_Left =  SpawnWorld->SpawnActor<AActor>(
			DeviceActorClass,
			TargetLocation_Left,
			ResultDirection.Rotation(),
			SpawnParams);
			Actors.Add(EditorWorldSpawnActor_Left);

			AActor* EditorWorldSpawnActor_Right =  SpawnWorld->SpawnActor<AActor>(
			DeviceActorClass,
			TargetLocation_Right,
			ResultDirection.Rotation(),
			SpawnParams);
			Actors.Add(EditorWorldSpawnActor_Right);
		}
			
		if (SpawnWorld&&SpawnWorld->MarkPackageDirty())
		{
			UEditorLoadingAndSavingUtils::SavePackages({ SpawnWorld->GetOutermost() }, true);
			UE_LOG(LogTemp, Warning, TEXT("Actor已生成到关卡 [%s]!"), *LevelName);
		}
	}
}

FVector ADeviceGenerator::TransformLocalOffsetToWorld(const FVector& WorldBaseLocation, const FVector& WorldDirection,
	const FVector& LocalOffset)
{
	// 1. 创建基准变换（位置 + 方向）
	FTransform BaseTransform;
    
	// 设置位置
	BaseTransform.SetLocation(WorldBaseLocation);
    
	// 从方向向量创建旋转（确保方向已归一化）
	if (!WorldDirection.IsNearlyZero())
	{
		FRotator BaseRotation = WorldDirection.Rotation();
		BaseTransform.SetRotation(BaseRotation.Quaternion());
	}
    
	// 2. 将本地偏移变换到世界空间
	FVector WorldPosition = BaseTransform.TransformPosition(LocalOffset);
    
	return WorldPosition;
}

void ADeviceGenerator::SetDeviceData()
{
	TSharedPtr<FJsonObject> JsonObject = UJsonToolLibrary::GetJsonObjectFromFile(SaveLevelName);
	if (JsonObject)
	{
		FString JsonString;
		UJsonToolLibrary:: GetJsonStringFromJsonObject(JsonObject,JsonString);
		TArray<FString>DataList;
		UJsonToolLibrary::GetJsonStringArrayFromJsonString(JsonString,TEXT("Data"),DataList);

		if (Actors.IsEmpty()||(Actors.IsValidIndex(0)&&!Actors[0]))
		{
			Actors.Empty();

			// 尝试查找已加载的关卡
			FString LevelPath = FString::Printf(TEXT("/Game/%s/%s.%s"),*SavePath, *SaveLevelName, *SaveLevelName);
			FSoftObjectPath LevelRef(LevelPath);
			UWorld* ExistingWorld = Cast<UWorld>(LevelRef.TryLoad());

			ULevel* TargetLevel = nullptr;
			for (ULevel* Level : ExistingWorld->GetLevels())
			{
				if (Level->GetOuter()->GetName() == SaveLevelName)
				{
					TargetLevel = Level;
					break;
				}
			}
			for (auto Actor : TargetLevel->Actors)
			{
				if (Actor->IsA(DeviceActorClass))
				{
					Actors.AddUnique(Actor);
				}
			}
		}
		
		for (int32 i = 0;i < DataList.Num();i++)
		{
			if (Actors.IsValidIndex(i)&&Actors[i])
			{
				if (UDeviceMarkComponent * DeviceMarkComponent = Actors[i]->FindComponentByClass<UDeviceMarkComponent>())
				{
					FString ID;
					UJsonToolLibrary::GetStringFromJsonString(DataList[i],TEXT("ID"),ID);
					
					DeviceMarkComponent->DeviceID = ID;
				}
			}
		}
		FString LevelPath = FString::Printf(TEXT("/Game/%s/%s.%s"),*SavePath, *SaveLevelName, *SaveLevelName);
		FSoftObjectPath LevelRef(LevelPath);
		UWorld* ExistingWorld = Cast<UWorld>(LevelRef.TryLoad());
		if (ExistingWorld&&ExistingWorld->MarkPackageDirty())
		{
			UEditorLoadingAndSavingUtils::SavePackages({ ExistingWorld->GetOutermost() }, true);
			UE_LOG(LogTemp, Warning, TEXT("数据已映射 [%s]!"), *SaveLevelName);
		}
	}
}
#endif
