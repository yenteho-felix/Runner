// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Runner : ModuleRules
{
	public Runner(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", 
			"EnhancedInput",			// UE enhanced input
			"UMG",						// Used for widget creation
			"LoadingScreenModule"		// Used for loading screens
		});
		
		OptimizeCode = CodeOptimization.Never;  // remove from real game but we should have it in 
	}
}
