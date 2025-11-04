// Fill out your copyright notice in the Description page of Project Settings.


#include "POICanvas.h"

#include "Device.h"
#include "DeviceManager.h"
#include "DocumentManager.h"
#include "SceneManagerSubsystem.h"
#include "StateSubsystem.h"
#include "UIInterface.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"

bool UPOICanvas::Initialize()
{
	if (!Super::Initialize())
		return false;

	// // 创建 Canvas Panel
	// CanvasPanel = NewObject<UCanvasPanel>(this, UCanvasPanel::StaticClass(), TEXT("DynamicCanvas"));
	//    
	// // 设置为根组件
	// if (CanvasPanel)
	// {
	// 	WidgetTree->RootWidget = CanvasPanel;
	// }
	return true;
}

void UPOICanvas::NativeConstruct()
{
	Super::NativeConstruct();
	
	POIMap.Empty();
	
	DeviceManager = UDeviceManager::Get(this);
	
	if (DeviceManager)
	{
		DeviceManager->OnDevicePOIInit.AddDynamic(this,&ThisClass::OnDevicePOIInit);
		DeviceManager->OnDeviceDisassemble.AddDynamic(this,&ThisClass::OnDeviceDisassemble);
	}

	StateSubsystem = UStateSubsystem::Get(this);
	if (StateSubsystem)
	{
		StateSubsystem->DeviceStateOnChange.AddDynamic(this,&ThisClass::OnDeviceStateOnChange);
	}

	SceneManagerSubsystem = USceneManagerSubsystem::Get(this);
	if (SceneManagerSubsystem)
	{
		SceneManagerSubsystem->OnScenePOIInit.AddDynamic(this,&ThisClass::OnScenePOIInit);
	}

	DocumentManager = UDocumentManager::Get(this);
	if (true)
	{
		DocumentManager->OnDocumentPOIInit.AddDynamic(this,&ThisClass::OnDocumentPOIInit);
	}
}

void UPOICanvas::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	TArray<FString> AllActorID;
	POIMap.GetKeys(AllActorID);
	FVector2D Location2D;
	for (auto Element : AllActorID)
	{
		if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(),POILocationMap.FindRef(Element),Location2D))
		{
			Location2D = (Location2D/UWidgetLayoutLibrary::GetViewportScale(this));
			if (POIMap.FindRef(Element))
			{
				if (UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(POIMap.FindRef(Element)))
				{
					CanvasPanelSlot->SetPosition(Location2D);
				}
			}
		}
	}
	if (DisassembleDevice)
	{
		TArray<UStaticMeshComponent*> Meshs;
		DeviceComponentTagMap.GetKeys(Meshs);
		for (auto Element : Meshs)
		{
			FVector Origin,BoxExtent;
			float SphereRadius;
			UKismetSystemLibrary::GetComponentBounds(Element,Origin,BoxExtent,SphereRadius);
			if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(),Origin,Location2D))
			{
				Location2D = (Location2D/UWidgetLayoutLibrary::GetViewportScale(this));
				if (DeviceComponentTagMap.FindRef(Element))
				{
					if (UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(DeviceComponentTagMap.FindRef(Element)))
					{
						CanvasPanelSlot->SetPosition(Location2D);
					}
				}
			}
		}
	}
}

void UPOICanvas::OnDevicePOIInit(const FString& ID,const FVector& HitLocation,const bool bShow)
{
	if (DeviceManager)
	{
		UUserWidget* UserWidget = DeviceManager->GetDevicePOI(ID);
		if (!UserWidget)
		{
			return;
		}
		if (bShow)
		{
			if (UserWidget)
			{
				POIMap.Add(ID,UserWidget);
				POILocationMap.Add(ID,HitLocation);
				if (CanvasPanel)
				{
					CanvasPanel->AddChildToCanvas(UserWidget);
				}
			
				UWidgetLayoutLibrary::SlotAsCanvasSlot(UserWidget)->SetAutoSize(true);
				if (UserWidget->GetClass()->ImplementsInterface(UUIInterface::StaticClass()))
				{
					IUIInterface::Execute_Init(UserWidget,ID);
				}
			}
		}
		else
		{
			if (CanvasPanel)
			{
				CanvasPanel->RemoveChild(UserWidget);
			}
			if (POILocationMap.Contains(ID))
			{
				POILocationMap.Remove(ID);
			}
			if (POIMap.Contains(ID))
			{
				POIMap.Remove(ID);
			}
		}	
	}
}

void UPOICanvas::OnDeviceDisassemble(ADevice* Device,const FString& Type)
{
	DisassembleDevice = Device;
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this,Device,Type]()
	{
		for (auto Element : DeviceComponentTagMap)
		{
			CanvasPanel->RemoveChild(Element.Value);
		}

		if (DisassembleDevicePOIClass->IsValidLowLevel())
		{
			for (auto Element : DisassembleDevice->DeviceComponentMap)
			{
				if (Type.IsEmpty() && !Device->DisassembleTypes.IsEmpty())
				{
					if (Element.Key && Element.Value.bShowPOI && Element.Value.Name == Element.Value.Type)
					{
						UUserWidget *UserWidget = CreateWidget<UUserWidget>(this,DisassembleDevicePOIClass);
						DeviceComponentTagMap.Add(Element.Key,UserWidget);
						CanvasPanel->AddChildToCanvas(UserWidget);
						UWidgetLayoutLibrary::SlotAsCanvasSlot(UserWidget)->SetAutoSize(true);
						if (UserWidget->GetClass()->ImplementsInterface(UUIInterface::StaticClass()))
						{
							IUIInterface::Execute_Init(UserWidget,Element.Value.Name);
							IUIInterface::Execute_SetDevice(UserWidget,DisassembleDevice,Element.Key);
						}
					}
				}
				else
				{
					if (Device->DisassembleTypes.IsEmpty() && Element.Value.bShowPOI && Element.Key)
					{
						UUserWidget *UserWidget = CreateWidget<UUserWidget>(this,DisassembleDevicePOIClass);
						DeviceComponentTagMap.Add(Element.Key,UserWidget);
						CanvasPanel->AddChildToCanvas(UserWidget);
						UWidgetLayoutLibrary::SlotAsCanvasSlot(UserWidget)->SetAutoSize(true);
						if (UserWidget->GetClass()->ImplementsInterface(UUIInterface::StaticClass()))
						{
							IUIInterface::Execute_Init(UserWidget,Element.Value.Name);
							IUIInterface::Execute_SetDevice(UserWidget,DisassembleDevice,Element.Key);
						}
					}
					else if (Type.Equals(Element.Value.Type) && Element.Key && Element.Value.bShowPOI && Element.Value.Name != Element.Value.Type)
					{
						UUserWidget *UserWidget = CreateWidget<UUserWidget>(this,DisassembleDevicePOIClass);
						DeviceComponentTagMap.Add(Element.Key,UserWidget);
						CanvasPanel->AddChildToCanvas(UserWidget);
						UWidgetLayoutLibrary::SlotAsCanvasSlot(UserWidget)->SetAutoSize(true);
						if (UserWidget->GetClass()->ImplementsInterface(UUIInterface::StaticClass()))
						{
							IUIInterface::Execute_Init(UserWidget,Element.Value.Name);
							IUIInterface::Execute_SetDevice(UserWidget,DisassembleDevice,Element.Key);
						}
					}
				}
			}
		}
	}));
}

void UPOICanvas::OnDeviceStateOnChange(FGameplayTag newState,const FString& ID)
{
	if (newState==CommonToolTags::DeviceState_Disassemble)
	{
		for (auto Element : DeviceComponentTagMap)
		{
			Element.Value->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		for (auto Element : POIMap)
		{
			Element.Value->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		for (auto Element : DeviceComponentTagMap)
		{
			Element.Value->SetVisibility(ESlateVisibility::Collapsed);
			CanvasPanel->RemoveChild(Element.Value);
		}
		DeviceComponentTagMap.Empty();
		
		for (auto Element : POIMap)
		{
			Element.Value->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

void UPOICanvas::OnScenePOIInit(const FString& ID, const FVector& HitLocation, const bool bShow,
	UUserWidget* UserWidget)
{
	if (bShow)
	{
		if (UserWidget)
		{
			POIMap.Add(ID,UserWidget);
			POILocationMap.Add(ID,HitLocation);
			if (CanvasPanel)
			{
				CanvasPanel->AddChildToCanvas(UserWidget);
			}

			if (UWidgetLayoutLibrary::SlotAsCanvasSlot(UserWidget))
			{
				UWidgetLayoutLibrary::SlotAsCanvasSlot(UserWidget)->SetAutoSize(true);
			}
			
			if (UserWidget->GetClass()->ImplementsInterface(UUIInterface::StaticClass()))
			{
				IUIInterface::Execute_Init(UserWidget,ID);
			}
		}
	}
	else
	{
		if (CanvasPanel)
		{
			CanvasPanel->RemoveChild(UserWidget);
		}
		if (POILocationMap.Contains(ID))
		{
			POILocationMap.Remove(ID);
		}
		if (POIMap.Contains(ID))
		{
			POIMap.Remove(ID);
		}
	}
}

void UPOICanvas::OnDocumentPOIInit(const FString& ID, const FVector& HitLocation, const bool bShow,
	UUserWidget* UserWidget)
{
	if (bShow)
	{
		if (UserWidget)
		{
			POIMap.Add(ID,UserWidget);
			POILocationMap.Add(ID,HitLocation);
			if (CanvasPanel)
			{
				CanvasPanel->AddChildToCanvas(UserWidget);
			}

			if (UWidgetLayoutLibrary::SlotAsCanvasSlot(UserWidget))
			{
				UWidgetLayoutLibrary::SlotAsCanvasSlot(UserWidget)->SetAutoSize(true);
			}
			
			if (UserWidget->GetClass()->ImplementsInterface(UUIInterface::StaticClass()))
			{
				IUIInterface::Execute_Init(UserWidget,ID);
			}
		}
	}
	else
	{
		if (CanvasPanel)
		{
			CanvasPanel->RemoveChild(UserWidget);
		}
		if (POILocationMap.Contains(ID))
		{
			POILocationMap.Remove(ID);
		}
		if (POIMap.Contains(ID))
		{
			POIMap.Remove(ID);
		}
	}
}
