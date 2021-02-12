using UnrealBuildTool;

public class UnrealFlecs : ModuleRules
{
	public UnrealFlecs(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"UFlecsQuickstart",
			"FlecsLibrary"
		});

		//The path for the header files
		PublicIncludePaths.AddRange(new string[] {"UnrealFlecs/Public"});
		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] {"UnrealFlecs/Private"});
	}
}