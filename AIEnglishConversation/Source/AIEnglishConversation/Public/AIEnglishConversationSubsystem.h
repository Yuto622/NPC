#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "AIEnglishConversationTypes.h"

#include "AIEnglishConversationSubsystem.generated.h"

/**
 * Sends OpenAI-compatible Chat Completions requests.
 * Blueprint-friendly entry point for conversation features.
 */
UCLASS()
class AIENGLISHCONVERSATION_API UAIEnglishConversationSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Send a chat completion request.
	 * - **SystemPrompt**: If empty, settings' DefaultSystemPrompt will be used by callers (component), but this function sends exactly what you pass.
	 * - **Messages**: History messages (User/Assistant). System messages should generally be passed via SystemPrompt.
	 */
	UFUNCTION(BlueprintCallable, Category="AI English Conversation")
	void SendChatCompletion(const FString& SystemPrompt, const TArray<FAIEnglishConversationMessage>& Messages, FAIEnglishConversationResultDelegate Callback);

	// Native-friendly variant (used by the NPC component).
	void SendChatCompletionNative(
		const FString& SystemPrompt,
		const TArray<FAIEnglishConversationMessage>& Messages,
		TFunction<void(bool /*bSuccess*/, const FString& /*AssistantText*/, const FString& /*ErrorMessage*/)> Callback
	);
};

