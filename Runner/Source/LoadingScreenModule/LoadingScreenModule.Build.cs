using UnrealBuildTool;

/**
 *  Create Loading Screen Module
 * 
 *  Critical: Module must have "PreLoadingScreen" load timing in .uproject
 *            Otherwise module won't initialize early enough to handle loading screen
 */
public class LoadingScreenModule : ModuleRules
{
    public LoadingScreenModule(ReadOnlyTargetRules Target) : base(Target)
    {
        // Specify the primary header file for precompiled headers
        PrivatePCHHeaderFile = "Public/LoadingScreenModule.h";
        
        //PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PCHUsage = PCHUsageMode.UseSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",            // UI framework
                "SlateCore",        // COre Slate functionality
                "MoviePlayer"       // Movie player for loading screens
            }
        );
    }
}