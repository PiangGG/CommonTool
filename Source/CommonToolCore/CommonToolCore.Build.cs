// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class CommonToolCore : ModuleRules
{
	public CommonToolCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Actor/Device"),
				Path.Combine(ModuleDirectory, "SubSystem/"),
				Path.Combine(ModuleDirectory, "Interface/"),
				Path.Combine(ModuleDirectory, "Library/"),
				Path.Combine(ModuleDirectory, "Interface/"),
				Path.Combine(ModuleDirectory, "Settings/"),
				Path.Combine(ModuleDirectory, "Components/"),
			}
		);
  //
		// PrivateIncludePaths.AddRange(
		// 	new string[] {
		// 		Path.Combine(GetModuleDirectory("Renderer"), "Private")
		// 	}
		// );
		//
		// string platform;
  //       string libSearchPattern;
  //       if (Target.Platform == UnrealTargetPlatform.Win64)
  //       {
  //           platform = "Windows-AMD64-";
  //           libSearchPattern = "*.lib";
  //       }
  //       else if (Target.Platform == UnrealTargetPlatform.Mac)
  //       {
  //           platform = "Darwin-universal-";
  //           libSearchPattern = "lib*.a";
  //       }
  //       else if (Target.Platform == UnrealTargetPlatform.Android)
  //       {
  //           platform = "Android-aarch64-";
  //           libSearchPattern = "lib*.a";
  //       }
  //       else if (Target.Platform == UnrealTargetPlatform.Linux)
  //       {
  //           platform = "Linux-x86_64-";
  //           libSearchPattern = "lib*.a";
  //       }
  //       else if(Target.Platform == UnrealTargetPlatform.IOS)
  //       {
  //           platform = "iOS-ARM64-";
  //           libSearchPattern = "lib*.a";
  //       }
  //       else
  //       {
  //           throw new InvalidOperationException("Cesium for Unreal does not support this platform.");
  //       }
  //
  //       string libPathBase = Path.Combine(ModuleDirectory, "../ThirdParty/lib/" + platform);
  //       string libPathDebug = libPathBase + "Debug";
  //       string libPathRelease = libPathBase + "Release";
  //
  //       bool useDebug = false;
  //       if (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.DebugGame)
  //       {
  //           if (Directory.Exists(libPathDebug))
  //           {
  //               useDebug = true;
  //           }
  //       }
  //
  //       string libPath = useDebug ? libPathDebug : libPathRelease;
  //
  //       string[] allLibs = Directory.Exists(libPath) ? Directory.GetFiles(libPath, libSearchPattern) : new string[0];
  //
  //       PublicAdditionalLibraries.AddRange(allLibs);
		
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
				"LidarPointCloudRuntime"
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
				"AIModule",
				"UMG",
				"GameplayTags",
				"GameplayTasks",
				"CommonUI",
				"EnhancedInput",
				"DatasmithRuntime", 
				"ProceduralMeshComponent",
				"GeometryCore",
				"AppFramework",
				"MediaAssets",
				"CesiumRuntime",
				
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