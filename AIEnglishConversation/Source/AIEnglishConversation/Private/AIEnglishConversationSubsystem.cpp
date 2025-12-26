#include "AIEnglishConversationSubsystem.h"

#include "AIEnglishConversationSettings.h"

#include "Dom/JsonObject.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

namespace AIEnglishConversation
{
	static FString RoleToString(const EAIEnglishConversationRole Role)
	{
		switch (Role)
		{
		case EAIEnglishConversationRole::System:
			return TEXT("system");
		case EAIEnglishConversationRole::Assistant:
			return TEXT("assistant");
		case EAIEnglishConversationRole::User:
		default:
			return TEXT("user");
		}
	}

	static FString ResolveApiKey(const UAIEnglishConversationSettings* Settings)
	{
		if (!Settings)
		{
			return FString();
		}

		if (!Settings->ApiKey.IsEmpty())
		{
			return Settings->ApiKey;
		}

		if (!Settings->ApiKeyEnvVar.IsEmpty())
		{
			return FPlatformMisc::GetEnvironmentVariable(*Settings->ApiKeyEnvVar);
		}

		return FString();
	}

	static FString NormalizeBaseUrl(const FString& In)
	{
		FString Out = In;
		while (Out.EndsWith(TEXT("/")))
		{
			Out.LeftChopInline(1);
		}
		return Out;
	}
}

void UAIEnglishConversationSubsystem::SendChatCompletion(
	const FString& SystemPrompt,
	const TArray<FAIEnglishConversationMessage>& Messages,
	FAIEnglishConversationResultDelegate Callback
)
{
	// Copy delegate into the async path safely.
	SendChatCompletionNative(SystemPrompt, Messages,
		[Callback](bool bSuccess, const FString& AssistantText, const FString& ErrorMessage) mutable
		{
			if (Callback.IsBound())
			{
				Callback.Execute(bSuccess, AssistantText, ErrorMessage);
			}
		}
	);
}

void UAIEnglishConversationSubsystem::SendChatCompletionNative(
	const FString& SystemPrompt,
	const TArray<FAIEnglishConversationMessage>& Messages,
	TFunction<void(bool, const FString&, const FString&)> Callback
)
{
	const UAIEnglishConversationSettings* Settings = GetDefault<UAIEnglishConversationSettings>();
	const FString ApiKey = AIEnglishConversation::ResolveApiKey(Settings);
	if (ApiKey.IsEmpty())
	{
		Callback(false, TEXT(""), TEXT("Missing API key. Set it in Project Settings -> Plugins -> AI English Conversation, or via environment variable (default: AI_EN_CONV_API_KEY)."));
		return;
	}

	if (!Settings)
	{
		Callback(false, TEXT(""), TEXT("Settings not available."));
		return;
	}

	// Build JSON body: OpenAI-compatible /chat/completions
	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetStringField(TEXT("model"), Settings->Model);
	Root->SetNumberField(TEXT("temperature"), Settings->Temperature);
	Root->SetNumberField(TEXT("max_tokens"), Settings->MaxTokens);

	TArray<TSharedPtr<FJsonValue>> JsonMessages;

	if (!SystemPrompt.IsEmpty())
	{
		TSharedRef<FJsonObject> Sys = MakeShared<FJsonObject>();
		Sys->SetStringField(TEXT("role"), TEXT("system"));
		Sys->SetStringField(TEXT("content"), SystemPrompt);
		JsonMessages.Add(MakeShared<FJsonValueObject>(Sys));
	}

	for (const FAIEnglishConversationMessage& Msg : Messages)
	{
		TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetStringField(TEXT("role"), AIEnglishConversation::RoleToString(Msg.Role));
		Obj->SetStringField(TEXT("content"), Msg.Content);
		JsonMessages.Add(MakeShared<FJsonValueObject>(Obj));
	}

	Root->SetArrayField(TEXT("messages"), JsonMessages);

	FString Body;
	{
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
		FJsonSerializer::Serialize(Root, Writer);
	}

	const FString BaseUrl = AIEnglishConversation::NormalizeBaseUrl(Settings->BaseUrl);
	const FString Url = BaseUrl + TEXT("/chat/completions");

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
	Req->SetURL(Url);
	Req->SetVerb(TEXT("POST"));
	Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Req->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ApiKey));
	Req->SetContentAsString(Body);

	// Capture callback by move (can be executed only once).
	auto CallbackPtr = MakeShared<TFunction<void(bool, const FString&, const FString&)>>(MoveTemp(Callback));

	Req->OnProcessRequestComplete().BindLambda(
		[CallbackPtr](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			auto Finish = [&](bool bOk, const FString& AssistantText, const FString& ErrorMessage)
			{
				if (CallbackPtr && *CallbackPtr)
				{
					(*CallbackPtr)(bOk, AssistantText, ErrorMessage);
				}
			};

			if (!bWasSuccessful || !Response.IsValid())
			{
				Finish(false, TEXT(""), TEXT("HTTP request failed (no response)."));
				return;
			}

			const int32 Code = Response->GetResponseCode();
			const FString RespText = Response->GetContentAsString();

			if (Code < 200 || Code >= 300)
			{
				Finish(false, TEXT(""), FString::Printf(TEXT("HTTP %d: %s"), Code, *RespText));
				return;
			}

			TSharedPtr<FJsonObject> Json;
			{
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(RespText);
				if (!FJsonSerializer::Deserialize(Reader, Json) || !Json.IsValid())
				{
					Finish(false, TEXT(""), TEXT("Failed to parse JSON response."));
					return;
				}
			}

			// OpenAI Chat Completions shape:
			// { choices: [ { message: { content: "..." } } ] }
			const TArray<TSharedPtr<FJsonValue>>* Choices = nullptr;
			if (!Json->TryGetArrayField(TEXT("choices"), Choices) || !Choices || Choices->Num() == 0)
			{
				Finish(false, TEXT(""), TEXT("No choices in response."));
				return;
			}

			const TSharedPtr<FJsonObject> Choice0 = (*Choices)[0].IsValid() ? (*Choices)[0]->AsObject() : nullptr;
			if (!Choice0.IsValid())
			{
				Finish(false, TEXT(""), TEXT("Invalid choice object."));
				return;
			}

			FString Content;
			const TSharedPtr<FJsonObject>* MessageObjPtr = nullptr;
			if (Choice0->TryGetObjectField(TEXT("message"), MessageObjPtr) && MessageObjPtr && MessageObjPtr->IsValid())
			{
				(*MessageObjPtr)->TryGetStringField(TEXT("content"), Content);
			}

			if (Content.IsEmpty())
			{
				// Some providers might return "text".
				Choice0->TryGetStringField(TEXT("text"), Content);
			}

			if (Content.IsEmpty())
			{
				Finish(false, TEXT(""), TEXT("No assistant content in response."));
				return;
			}

			Finish(true, Content, TEXT(""));
		}
	);

	if (!Req->ProcessRequest())
	{
		CallbackPtr->operator()(false, TEXT(""), TEXT("Failed to start HTTP request."));
	}
}

