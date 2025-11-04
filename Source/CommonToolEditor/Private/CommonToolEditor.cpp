#include "CommonToolEditor.h"

#include "CommonToolCommands.h"
#include "PrintToolLibrary.h"

#define LOCTEXT_NAMESPACE "FCommonToolEditorModule"

static const FName TabName(TEXT("打开生成接触网设备面板"));

void FCommonToolEditorModule::StartupModule()
{
	FCommonToolStyle::Initialize();
	FCommonToolStyle::ReloadTextures();

	FCommonToolCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);
	
	PluginCommands->MapAction(
		FCommonToolCommands::Get().GenerateDevice,
		FExecuteAction::CreateRaw(this, &FCommonToolEditorModule::OpenGenerateDevice),
		FCanExecuteAction());
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCommonToolEditorModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabName, FOnSpawnTab::CreateRaw(this, &FCommonToolEditorModule::OnSpawnDevicePluginTab))
		.SetDisplayName(LOCTEXT("FtestTabTitle", "打开生成接触网设备面板"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FCommonToolEditorModule::ShutdownModule()
{
    
}

void FCommonToolEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FCommonToolCommands::Get().GenerateDevice, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FCommonToolCommands::Get().GenerateDevice));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

void FCommonToolEditorModule::OpenGenerateDevice()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TabName);
}

FReply FCommonToolEditorModule::ReSetAllDeviceRegion()
{
	UPrintToolLibrary::Debug(TEXT("ReSetAllDeviceRegion"));
	return FReply::Handled();
}

TSharedRef<class SDockTab> FCommonToolEditorModule::OnSpawnDevicePluginTab(const class FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(LOCTEXT("WindowWidgetText", "{0}"),FText::FromString(TEXT("接触网设备生成面板")));
	return SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		// Put your tab content here!
		SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot() 
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot() 
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SAssignNew(RegionTag,SEditableText)
					.HintText(FText::FromString(TEXT("线路与防火区的定义")))
				]
				+ SHorizontalBox::Slot() 
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SAssignNew(RegionButton,SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Text(FText::Format(
				LOCTEXT("WindowWidgetText", "{0}"),
					FText::FromString(TEXT("提交设置"))
					))
					.OnClicked_Lambda([this]()
					{
						return ReSetAllDeviceRegion();
					})
				]
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FCommonToolEditorModule, CommonToolEditor)