#include "AIEnglishConversationNPCComponent.h"

#include "AIEnglishConversationSettings.h"
#include "AIEnglishConversationSubsystem.h"

#include "Kismet/GameplayStatics.h"

void UAIEnglishConversationNPCComponent::ClearHistory()
{
	History.Reset();
}

void UAIEnglishConversationNPCComponent::TalkToNPC(UObject* WorldContextObject, const FString& PlayerText, FAIEnglishConversationResultDelegate Callback)
{
	if (PlayerText.TrimStartAndEnd().IsEmpty())
	{
		if (Callback.IsBound())
		{
			Callback.Execute(false, TEXT(""), TEXT("PlayerText is empty."));
		}
		return;
	}

	UObject* Context = WorldContextObject ? WorldContextObject : GetWorld();
	UGameInstance* GI = Context ? UGameplayStatics::GetGameInstance(Context) : nullptr;
	UAIEnglishConversationSubsystem* Subsystem = GI ? GI->GetSubsystem<UAIEnglishConversationSubsystem>() : nullptr;
	if (!Subsystem)
	{
		if (Callback.IsBound())
		{
			Callback.Execute(false, TEXT(""), TEXT("AIEnglishConversationSubsystem not available (no GameInstance?)."));
		}
		return;
	}

	const UAIEnglishConversationSettings* Settings = GetDefault<UAIEnglishConversationSettings>();

	FString SystemPrompt;
	if (!SystemPromptOverride.IsEmpty())
	{
		SystemPrompt = SystemPromptOverride;
	}
	else
	{
		if (bIncludeDefaultSystemPrompt && Settings)
		{
			SystemPrompt = Settings->DefaultSystemPrompt;
		}
		if (!NPCPersonaPrompt.IsEmpty())
		{
			if (!SystemPrompt.IsEmpty())
			{
				SystemPrompt += TEXT("\n\n");
			}
			SystemPrompt += NPCPersonaPrompt;
		}
	}

	// Append user message to history and send.
	FAIEnglishConversationMessage UserMsg;
	UserMsg.Role = EAIEnglishConversationRole::User;
	UserMsg.Content = PlayerText;
	History.Add(UserMsg);

	const TArray<FAIEnglishConversationMessage> MessagesToSend = History;
	const TWeakObjectPtr<UAIEnglishConversationNPCComponent> WeakThis(this);
	Subsystem->SendChatCompletionNative(SystemPrompt, MessagesToSend,
		[WeakThis, Callback](bool bSuccess, const FString& AssistantText, const FString& ErrorMessage) mutable
		{
			if (bSuccess && WeakThis.IsValid())
			{
				FAIEnglishConversationMessage AssistantMsg;
				AssistantMsg.Role = EAIEnglishConversationRole::Assistant;
				AssistantMsg.Content = AssistantText;
				WeakThis->History.Add(AssistantMsg);
			}

			if (Callback.IsBound())
			{
				Callback.Execute(bSuccess, AssistantText, ErrorMessage);
			}
		}
	);
}

