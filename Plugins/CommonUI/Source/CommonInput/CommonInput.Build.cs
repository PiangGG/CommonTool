// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CommonInput : ModuleRules
{
	public CommonInput(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"EnhancedInput",
				"DeveloperSettings"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"SlateCore",
				"Slate",
				"ApplicationCore",
				"EngineSettings"
			}
		);

		if (Target.Type == TargetType.Editor)
			PublicDependencyModuleNames.AddRange(
				new[]
				{
					"EditorFramework",
					"UnrealEd"
				}
			);

		var CommonUIPlatform = ToCommonUIPlatform(Target.Platform);
		if (!string.IsNullOrEmpty(CommonUIPlatform))
			PublicDefinitions.Add("UE_COMMONINPUT_PLATFORM_TYPE = " + CommonUIPlatform);

		PrivateDefinitions.Add("UE_COMMONINPUT_PLATFORM_KBM_REQUIRES_ATTACHED_MOUSE=" +
		                       (bPlatformKBMRequiresAttachedMouse ? "1" : "0"));
		PrivateDefinitions.Add(
			"UE_COMMONINPUT_FORCE_TOUCH_SUPPORT_DISABLED=" + (bForceTouchSupportDisabled ? "1" : "0"));
	}

	protected virtual bool bPlatformKBMRequiresAttachedMouse => false;
	protected virtual bool bForceTouchSupportDisabled => false;

	public static string ToCommonUIPlatform(UnrealTargetPlatform TargetPlatform)
	{
		if (TargetPlatform.IsInGroup(UnrealPlatformGroup.Windows)) return "PC";

		if (TargetPlatform == UnrealTargetPlatform.Mac) return "Mac";

		if (TargetPlatform == UnrealTargetPlatform.Linux) return "PC";

		if (TargetPlatform == UnrealTargetPlatform.IOS) return "IOS";

		if (TargetPlatform == UnrealTargetPlatform.Android) return "Android";

		return string.Empty;
	}
}