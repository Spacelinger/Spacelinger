// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Spacelinger : ModuleRules
{
	public Spacelinger(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"EnhancedInput",
			"UMG",
			"AIModule",
			"CableComponent",
			"NavigationSystem",
            "MCVGASFramework",
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			"Niagara"
        });
		PublicIncludePaths.Add("Spacelinger");

		PrivateDependencyModuleNames.AddRange(new string[] { "MCVGASFramework", "GameplayAbilities" });

    }
}
