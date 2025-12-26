using UnrealBuildTool;

public class AiEnglishConversation : ModuleRules
{
	public AiEnglishConversation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Http",
				"Json",
				"JsonUtilities",
				"Slate",
				"SlateCore",
				"UMG",
				"DeveloperSettings"
			}
		);
	}
}

