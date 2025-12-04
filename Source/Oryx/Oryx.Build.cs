// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Oryx : ModuleRules
{
	public Oryx(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Oryx",
			"Oryx/Variant_Platforming",
			"Oryx/Variant_Platforming/Animation",
			"Oryx/Variant_Combat",
			"Oryx/Variant_Combat/AI",
			"Oryx/Variant_Combat/Animation",
			"Oryx/Variant_Combat/Gameplay",
			"Oryx/Variant_Combat/Interfaces",
			"Oryx/Variant_Combat/UI",
			"Oryx/Variant_SideScrolling",
			"Oryx/Variant_SideScrolling/AI",
			"Oryx/Variant_SideScrolling/Gameplay",
			"Oryx/Variant_SideScrolling/Interfaces",
			"Oryx/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
