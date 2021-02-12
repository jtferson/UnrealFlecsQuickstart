using UnrealBuildTool;

public class MainGameplay : ModuleRules
{
	public MainGameplay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"UFlecsQuickstart",
			"FlecsLibrary",
			"UnrealFlecs",
		});

		//The path for the header files
		PublicIncludePaths.AddRange(new string[] {"MainGameplay/Public"});
		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] {"MainGameplay/Private"});
	}
}