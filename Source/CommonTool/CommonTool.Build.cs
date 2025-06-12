// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class CommonTool : ModuleRules
{
	public CommonTool(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"InputCore",
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"HTTP",
				"Sockets",
				"HTTPServer",
				"Json",
				"JsonUtilities",
				"JsonBlueprintUtilities",
				"DeveloperSettings",
				"GameplayTags",
				"AIModule",
				"UMG",
				"GameplayTags",
				"GameplayTasks",
				"CommonUI",
				"EnhancedInput",
				"ProceduralMeshComponent",
				"GeometryCore",
				"AppFramework",
				"MediaAssets"
				
				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
		ShadowVariableWarningLevel = WarningLevel.Off;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;
		bEnableExceptions = true;
	}
}