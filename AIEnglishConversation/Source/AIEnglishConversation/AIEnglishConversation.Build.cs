using UnrealBuildTool;

public class AIEnglishConversation : ModuleRules
{
	public AIEnglishConversation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"DeveloperSettings",
				"HTTP",
				"Json",
				"JsonUtilities"
			}
		);
	}
}

