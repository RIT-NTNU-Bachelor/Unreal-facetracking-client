// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UserLocServer : ModuleRules
{
	public UserLocServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "RenderCore", "InputCore", "Sockets", "Networking" });

		PrivateDependencyModuleNames.AddRange(new string[] { "HTTP" });
	}
}
