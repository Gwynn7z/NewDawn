// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NewDawn : ModuleRules
{
	public NewDawn(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "HeadMountedDisplay", "Niagara", "UMG" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "InputCore"});
	}
}
