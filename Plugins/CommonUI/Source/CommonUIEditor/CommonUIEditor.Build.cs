// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CommonUIEditor : ModuleRules
{
	public CommonUIEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new[]
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
				"ToolMenus"
			}
		);

		PrivateIncludePathModuleNames.AddRange(
			new[]
			{
				"DataTableEditor"
			}
		);

		PublicIncludePaths.AddRange(
			new string[]
			{
			}
		);
	}
}