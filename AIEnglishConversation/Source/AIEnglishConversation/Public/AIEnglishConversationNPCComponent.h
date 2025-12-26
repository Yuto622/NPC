#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AIEnglishConversationTypes.h"

#include "AIEnglishConversationNPCComponent.generated.h"

/**
 * Attach to an NPC actor. Call TalkToNPC() from Blueprints to send the player's text,
 * store history, and receive the assistant reply.
 */
UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class AIENGLISHCONVERSATION_API UAIEnglishConversationNPCComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	/** If set, overrides all system prompts (recommended for per-NPC persona). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI English Conversation", meta=(MultiLine="true"))
	FString SystemPromptOverride;

	/** Extra NPC persona text appended after the plugin default system prompt. Ignored if SystemPromptOverride is set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI English Conversation", meta=(MultiLine="true"))
	FString NPCPersonaPrompt;

	/** If true, include the plugin default system prompt from settings. Ignored if SystemPromptOverride is set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI English Conversation")
	bool bIncludeDefaultSystemPrompt = true;

	/** Stored conversation history (User/Assistant). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI English Conversation")
	TArray<FAIEnglishConversationMessage> History;

	/** Clear the conversation history. */
	UFUNCTION(BlueprintCallable, Category="AI English Conversation")
	void ClearHistory();

	/**
	 * Send the player's message and receive the NPC reply.
	 * Callback receives (bSuccess, AssistantText, ErrorMessage).
	 */
	UFUNCTION(BlueprintCallable, Category="AI English Conversation", meta=(WorldContext="WorldContextObject"))
	void TalkToNPC(UObject* WorldContextObject, const FString& PlayerText, FAIEnglishConversationResultDelegate Callback);
};

