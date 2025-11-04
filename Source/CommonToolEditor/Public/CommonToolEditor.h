#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FCommonToolEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    void RegisterMenus();
protected:
    void OpenGenerateDevice();

    FReply ReSetAllDeviceRegion();
    
    TSharedRef<class SDockTab> OnSpawnDevicePluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
    TSharedPtr<class SButton> RegionButton;
    TSharedPtr<class SEditableText> RegionTag;
    TSharedPtr<class FUICommandList> PluginCommands;
};
