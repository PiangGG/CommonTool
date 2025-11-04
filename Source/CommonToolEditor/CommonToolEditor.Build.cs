using UnrealBuildTool;

public class CommonToolEditor : ModuleRules
{
    public CommonToolEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "ApplicationCore",
                "Engine",
                "PropertyEditor",
                "InputCore",
                "Slate",
                "UMG",
                "SlateCore",
                "CommonUI",
                "EditorWidgets",
                "UnrealEd",
                "GameplayTags",
                "GameplayTagsEditor",
                "AssetTools",
                "AssetRegistry",
                "ToolMenus",
                "EditorFramework",
                "UnrealEd",
                "ToolMenus",
                "CommonToolCore",
                "Json",
                "JsonUtilities",
                "JsonBlueprintUtilities",
                "Projects"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}