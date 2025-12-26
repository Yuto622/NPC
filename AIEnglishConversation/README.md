# AI English Conversation（Unreal Engine プラグイン）

このプラグインは **OpenAI互換の Chat Completions API** にHTTPで投げて、ゲーム内のNPCと **英会話（テキスト）** をできるようにする最小構成です。

## できること

- NPC（任意のActor）に `UAIEnglishConversationNPCComponent` を付けて会話履歴を保持
- Blueprintから `TalkToNPC` を呼ぶだけで、LLMの返答テキストを受け取れる
- APIキーは **Project Settings** または **環境変数** から取得

## 導入（プロジェクトに取り込み）

- **プロジェクトに配置**: `YourProject/Plugins/AIEnglishConversation/` に、このリポジトリの `AIEnglishConversation/` フォルダを丸ごとコピー
- **UEで有効化**: `Edit -> Plugins` で **AI English Conversation** を有効化 → 再起動

## 設定（APIキー・モデル）

`Edit -> Project Settings -> Plugins -> AI English Conversation` で設定します。

- **BaseUrl**: 例 `https://api.openai.com/v1`（末尾スラッシュ不要）
- **Model**: 例 `gpt-4o-mini`
- **API Key**: 直接入力（推奨はしません）
- **ApiKeyEnvVar**: 環境変数名（デフォルト `AI_EN_CONV_API_KEY`）

### 環境変数でAPIキーを渡す（推奨）

起動前に環境変数を設定します（例）:

```bash
export AI_EN_CONV_API_KEY="YOUR_KEY"
```

## 使い方（NPCと会話）

### 1) NPCにコンポーネントを追加

NPCのBlueprint（またはActor）に **`AIEnglishConversationNPCComponent`** を追加します。

### 2) Persona（任意）

コンポーネントの `NPCPersonaPrompt` 例:

- 「あなたは村の道具屋。短くフレンドリーに答えて、最後にプレイヤー英語の軽い訂正を1〜2個だけ添える」

`SystemPromptOverride` を設定すると、デフォルトのSystemPromptを含めず **完全上書き** になります。

### 3) Blueprintから呼ぶ

プレイヤーが話しかけたタイミングで `TalkToNPC(WorldContextObject, PlayerText, Callback)` を呼びます。

- **bSuccess=true** のとき `AssistantText` がNPCの返答
- **bSuccess=false** のとき `ErrorMessage` に理由（HTTPエラー/JSONパース失敗/キー未設定など）

返ってきた `AssistantText` をUMGのTextに表示すれば、最低限の会話UIが作れます。

## INIでの設定例（任意）

`Config/DefaultGame.ini` に追加:

```ini
[/Script/AIEnglishConversation.AIEnglishConversationSettings]
BaseUrl="https://api.openai.com/v1"
Model="gpt-4o-mini"
Temperature=0.7
MaxTokens=300
ApiKeyEnvVar="AI_EN_CONV_API_KEY"
```

## 注意

- この実装は **テキスト会話の最小核** です（音声入出力/UIは含みません）
- APIキーをプロジェクト設定に直書きする場合は、配布/共有/リポジトリ公開に注意してください

