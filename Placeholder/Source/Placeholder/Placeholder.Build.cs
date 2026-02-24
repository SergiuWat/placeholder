// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class Placeholder : ModuleRules
{
	public Placeholder(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate",
            "CableComponent"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Placeholder",
			"Placeholder/Variant_Platforming",
			"Placeholder/Variant_Platforming/Animation",
			"Placeholder/Variant_Combat",
			"Placeholder/Variant_Combat/AI",
			"Placeholder/Variant_Combat/Animation",
			"Placeholder/Variant_Combat/Gameplay",
			"Placeholder/Variant_Combat/Interfaces",
			"Placeholder/Variant_Combat/UI",
			"Placeholder/Variant_SideScrolling",
			"Placeholder/Variant_SideScrolling/AI",
			"Placeholder/Variant_SideScrolling/Gameplay",
			"Placeholder/Variant_SideScrolling/Interfaces",
			"Placeholder/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
