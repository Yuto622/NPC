#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AIEnglishConversationSettings.generated.h"

/**
 * Project Settings: Plugins -> AI English Conversation
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="AI English Conversation"))
class AIENGLISHCONVERSATION_API UAIEnglishConversationSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** OpenAI-compatible base URL (no trailing slash), e.g. https://api.openai.com/v1 */
	UPROPERTY(Config, EditAnywhere, Category="API")
	FString BaseUrl = TEXT("https://api.openai.com/v1");

	/** Model name for /chat/completions, e.g. gpt-4o-mini */
	UPROPERTY(Config, EditAnywhere, Category="API")
	FString Model = TEXT("gpt-4o-mini");

	/**
	 * API key for Authorization: Bearer.
	 * Prefer leaving this empty and setting environment variable: AI_EN_CONV_API_KEY
	 */
	UPROPERTY(Config, EditAnywhere, Category="API", meta=(DisplayName="API Key", PasswordField="true"))
	FString ApiKey;

	/** Optional: environment variable name to read the API key from. */
	UPROPERTY(Config, EditAnywhere, Category="API")
	FString ApiKeyEnvVar = TEXT("AI_EN_CONV_API_KEY");

	/** Request timeout in seconds */
	UPROPERTY(Config, EditAnywhere, Category="API", meta=(ClampMin="1.0", ClampMax="120.0"))
	float TimeoutSeconds = 30.0f;

	/** Temperature for the model (0..2) */
	UPROPERTY(Config, EditAnywhere, Category="Generation", meta=(ClampMin="0.0", ClampMax="2.0"))
	float Temperature = 0.7f;

	/** Max tokens for the assistant reply (soft limit). */
	UPROPERTY(Config, EditAnywhere, Category="Generation", meta=(ClampMin="16", ClampMax="2048"))
	int32 MaxTokens = 300;

	/** Default system prompt for "English conversation NPC". */
	UPROPERTY(Config, EditAnywhere, Category="Prompt", meta=(MultiLine="true"))
	FString DefaultSystemPrompt = TEXT(
		"You are an NPC in a game, chatting with the player in English. "
		"Keep replies natural and friendly. "
		"After answering, provide brief corrections of the player's English (if needed) with 1-2 suggestions. "
		"Keep it short (2-5 sentences total). "
	);

	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
};

