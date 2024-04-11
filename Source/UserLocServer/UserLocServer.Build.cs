// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UserLocServer : ModuleRules
{
	public UserLocServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"Sockets", 
			"Networking",
			"Slate",
			"SlateCore",
			"HeadMountedDisplay",
            "UMG"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
            "CoreUObject",
            "Engine",
            "RHI",
            "RenderCore",
            "CinematicCamera"
        });
	}
}
