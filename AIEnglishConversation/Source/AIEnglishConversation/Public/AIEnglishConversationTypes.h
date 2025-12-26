#pragma once

#include "CoreMinimal.h"
#include "AIEnglishConversationTypes.generated.h"

UENUM(BlueprintType)
enum class EAIEnglishConversationRole : uint8
{
	System UMETA(DisplayName="System"),
	User UMETA(DisplayName="User"),
	Assistant UMETA(DisplayName="Assistant")
};

USTRUCT(BlueprintType)
struct FAIEnglishConversationMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI English Conversation")
	EAIEnglishConversationRole Role = EAIEnglishConversationRole::User;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI English Conversation", meta=(MultiLine="true"))
	FString Content;
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(
	FAIEnglishConversationResultDelegate,
	bool, bSuccess,
	const FString&, AssistantText,
	const FString&, ErrorMessage
);

