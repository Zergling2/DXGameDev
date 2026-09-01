#include "GameUIManager.h"
#include "Player.h"
#include "Account.h"
#include "ListenServerClient.h"
#include "..\Resource\LSProtocol.h"

using namespace ze;

const wchar_t* const GAME_UI_TEXT_FONT = L"Agency FB";
const FLOAT CHAT_PANEL_ALPHA = 0.25f;

GameUIStateDeactivate GameUIStateDeactivate::s_instance;
GameUIStatePlaying GameUIStatePlaying::s_instance;
GameUIStateScoreboard GameUIStateScoreboard::s_instance;
GameUIStateMenu GameUIStateMenu::s_instance;
GameUIStateChatting GameUIStateChatting::s_instance;

void GameUIStateDeactivate::Enter(GameUIManager* pGameUIManager)
{
	Cursor::SetVisible(true);
	Cursor::SetLockState(CursorLockMode::None);

	pGameUIManager->HideAdapterInfo();
	pGameUIManager->HideScoreboard();
	pGameUIManager->HideMenu();
	pGameUIManager->HideGameUI();
	pGameUIManager->HideChatPanel();
}

void GameUIStateDeactivate::Update(GameUIManager* pGameUIManager, float dt)
{
}

void GameUIStateDeactivate::Exit(GameUIManager* pGameUIManager)
{
}

void GameUIStatePlaying::Enter(GameUIManager* pGameUIManager)
{
	Cursor::SetVisible(false);
	Cursor::SetLockState(CursorLockMode::Locked);

	Player* pScriptPlayer = pGameUIManager->GetPlayerScript();
	if (pScriptPlayer)
		if (!pScriptPlayer->IsDead())
			pScriptPlayer->SetProcessingInput(true);

	pGameUIManager->ShowGameUI();
}

void GameUIStatePlaying::Update(GameUIManager* pGameUIManager, float dt)
{
	if (Input::GetInstance()->GetKeyDown(Keycode::KEY_TAB))
	{
		pGameUIManager->SetState(GameUIStateScoreboard::GetState());
		return;
	}

	if (Input::GetInstance()->GetKeyDown(Keycode::KEY_ESCAPE))
	{
		pGameUIManager->SetState(GameUIStateMenu::GetState());
		return;
	}

	if (Input::GetInstance()->GetKeyDown(Keycode::KEY_RETURN))
	{
		pGameUIManager->SetState(GameUIStateChatting::GetState());
		return;
	}
}

void GameUIStatePlaying::Exit(GameUIManager* pGameUIManager)
{
	pGameUIManager->HideGameUI();
}

void GameUIStateScoreboard::Enter(GameUIManager* pGameUIManager)
{
	pGameUIManager->ShowAdapterInfo();
	pGameUIManager->ShowScoreboard();
}

void GameUIStateScoreboard::Update(GameUIManager* pGameUIManager, float dt)
{
	if (!Input::GetInstance()->GetKey(Keycode::KEY_TAB))
	{
		pGameUIManager->SetState(GameUIStatePlaying::GetState());
		return;
	}
}

void GameUIStateScoreboard::Exit(GameUIManager* pGameUIManager)
{
	pGameUIManager->HideAdapterInfo();
	pGameUIManager->HideScoreboard();
}

void GameUIStateMenu::Enter(GameUIManager* pGameUIManager)
{
	Cursor::SetVisible(true);
	Cursor::SetLockState(CursorLockMode::None);

	Player* pScriptPlayer = pGameUIManager->GetPlayerScript();
	if (pScriptPlayer)
		pScriptPlayer->SetProcessingInput(false);

	pGameUIManager->ShowAdapterInfo();
	pGameUIManager->ShowMenu();
}

void GameUIStateMenu::Update(GameUIManager* pGameUIManager, float dt)
{
	if (Input::GetInstance()->GetKeyDown(Keycode::KEY_ESCAPE))
	{
		pGameUIManager->SetState(GameUIStatePlaying::GetState());
		return;
	}
}

void GameUIStateMenu::Exit(GameUIManager* pGameUIManager)
{
	pGameUIManager->HideAdapterInfo();
	pGameUIManager->HideMenu();
}

void GameUIStateChatting::Enter(GameUIManager* pGameUIManager)
{
	Cursor::SetVisible(true);
	Cursor::SetLockState(CursorLockMode::None);

	Player* pScriptPlayer = pGameUIManager->GetPlayerScript();
	if (pScriptPlayer)
		pScriptPlayer->SetProcessingInput(false);

	pGameUIManager->ShowGameUI();
	pGameUIManager->ShowChatPanel();
}

void GameUIStateChatting::Update(GameUIManager* pGameUIManager, float dt)
{
	if (Input::GetInstance()->GetKeyDown(Keycode::KEY_RETURN))
	{
		pGameUIManager->SendChatMsg();
		pGameUIManager->SetState(GameUIStatePlaying::GetState());
		return;
	}
}

void GameUIStateChatting::Exit(GameUIManager* pGameUIManager)
{
	pGameUIManager->HideChatPanel();
	pGameUIManager->HideGameUI();
}

GameUIManager::GameUIManager(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_pUIState(nullptr)
	, m_activeRespawnUI(false)
	, m_needUpdateChatMsgTransparency(false)
	, m_respawnRemainingTime(0.0f)
	, m_chatMsgCount(0)
	, m_chatMsgTransparencyTimer(0.0f)
{
	m_scoreboardPlayerAccountId[static_cast<size_t>(GameTeam::RedTeam)].reserve(MAX_PLAYERS_PER_TEAM);
	m_scoreboardPlayerAccountId[static_cast<size_t>(GameTeam::BlueTeam)].reserve(MAX_PLAYERS_PER_TEAM);
}

void GameUIManager::Awake()
{
	// ###################################
	// Adapter Info UI
	UIObjectHandle hPanelAdapterInfoRoot = Runtime::GetInstance()->CreatePanel();
	m_hPanelAdapterInfoRoot = hPanelAdapterInfoRoot;
	Panel* pPanelAdapterInfoRoot = static_cast<Panel*>(hPanelAdapterInfoRoot.ToPtr());
	pPanelAdapterInfoRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pPanelAdapterInfoRoot->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pPanelAdapterInfoRoot->m_transform.SetPosition(0, 0);
	pPanelAdapterInfoRoot->SetShape(PanelShape::Rectangle);
	pPanelAdapterInfoRoot->SetColor(ColorsLinear::DimGray);
	pPanelAdapterInfoRoot->SetColorA(0.25f);
	pPanelAdapterInfoRoot->SetSize(100, 70);
	
	{
		UIObjectHandle hText = Runtime::GetInstance()->CreateText();
		Text* pText = static_cast<Text*>(hText.ToPtr());
		pText->m_transform.SetParent(&pPanelAdapterInfoRoot->m_transform);
		pText->SetSize(XMFLOAT2(256, 16));
		std::wstring text = GraphicDevice::GetInstance()->GetAdapterDescription();
		pText->SetText(std::move(text));
		pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pText->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
		pText->m_transform.SetPosition(128 + 2, -(8));
		pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pText->SetColor(ColorsLinear::Orange);
		pText->GetTextFormat().SetSize(12);
		pText->GetTextFormat().SetFontFamilyName(L"Consolas");
		pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pText->ApplyTextFormat();
	}

	{
		UIObjectHandle hText = Runtime::GetInstance()->CreateText();
		Text* pText = static_cast<Text*>(hText.ToPtr());
		pText->m_transform.SetParent(&pPanelAdapterInfoRoot->m_transform);
		pText->SetSize(XMFLOAT2(256, 16));
		std::wstring text = L"DedicatedVideoMemory: ";
		size_t val = GraphicDevice::GetInstance()->GetAdapterDedicatedVideoMemory();
		text += std::to_wstring(val / (1024 * 1024));
		text += L"MB";
		pText->SetText(std::move(text));
		pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pText->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
		pText->m_transform.SetPosition(128 + 2, -(8 + 16 * 1));
		pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pText->SetColor(ColorsLinear::Orange);
		pText->GetTextFormat().SetSize(12);
		pText->GetTextFormat().SetFontFamilyName(L"Consolas");
		pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pText->ApplyTextFormat();
	}

	{
		UIObjectHandle hText = Runtime::GetInstance()->CreateText();
		Text* pText = static_cast<Text*>(hText.ToPtr());
		pText->m_transform.SetParent(&pPanelAdapterInfoRoot->m_transform);
		pText->SetSize(XMFLOAT2(256, 16));
		std::wstring text = L"DedicatedSystemMemory: ";
		size_t val = GraphicDevice::GetInstance()->GetAdapterDedicatedSystemMemory();
		text += std::to_wstring(val / (1024 * 1024));
		text += L"MB";
		pText->SetText(std::move(text));
		pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pText->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
		pText->m_transform.SetPosition(128 + 2, -(8 + 16 * 2));
		pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pText->SetColor(ColorsLinear::Orange);
		pText->GetTextFormat().SetSize(12);
		pText->GetTextFormat().SetFontFamilyName(L"Consolas");
		pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pText->ApplyTextFormat();
	}

	{
		UIObjectHandle hText = Runtime::GetInstance()->CreateText();
		Text* pText = static_cast<Text*>(hText.ToPtr());
		pText->m_transform.SetParent(&pPanelAdapterInfoRoot->m_transform);
		pText->SetSize(XMFLOAT2(256, 16));
		std::wstring text = L"SharedSystemMemory: ";
		size_t val = GraphicDevice::GetInstance()->GetAdapterSharedSystemMemory();
		text += std::to_wstring(val / (1024 * 1024));
		text += L"MB";
		pText->SetText(std::move(text));
		pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pText->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
		pText->m_transform.SetPosition(128 + 2, -(8 + 16 * 3));
		pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pText->SetColor(ColorsLinear::Orange);
		pText->GetTextFormat().SetSize(12);
		pText->GetTextFormat().SetFontFamilyName(L"Consolas");
		pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pText->ApplyTextFormat();
	}
	// ###################################








	// ##### 점수판 UI 생성 #####
	constexpr XMFLOAT2 SCOREBOARD_SIZE(960, 540);
	UIObjectHandle hPanelScoreboardRoot = Runtime::GetInstance()->CreatePanel();
	m_hPanelScoreboardRoot = hPanelScoreboardRoot;		// 핸들 저장
	Panel* pPanelScoreboardRoot = static_cast<Panel*>(hPanelScoreboardRoot.ToPtr());
	pPanelScoreboardRoot->SetSize(SCOREBOARD_SIZE);
	pPanelScoreboardRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelScoreboardRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelScoreboardRoot->SetColor(Colors::Black);
	pPanelScoreboardRoot->SetColorA(0.4f);
	pPanelScoreboardRoot->SetShape(PanelShape::RoundedRectangle);

	UIObjectHandle hTextScoreboardTitle = Runtime::GetInstance()->CreateText();
	Text* pTextScoreboardTitle = static_cast<Text*>(hTextScoreboardTitle.ToPtr());
	pTextScoreboardTitle->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
	pTextScoreboardTitle->SetSize(300, 40);
	pTextScoreboardTitle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextScoreboardTitle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextScoreboardTitle->GetTextFormat().SetSize(32);
	pTextScoreboardTitle->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_BOLD);
	pTextScoreboardTitle->ApplyTextFormat();
	pTextScoreboardTitle->m_transform.SetPosition(0, SCOREBOARD_SIZE.y / 2 - 55);
	pTextScoreboardTitle->SetColor(ColorsLinear::Gold);
	pTextScoreboardTitle->SetText(L"SCOREBOARD");

	constexpr XMFLOAT2 TEAM_PANEL_SIZE(SCOREBOARD_SIZE.x / 2 - 10, 26);
	constexpr XMFLOAT2 TEAM_PANEL_OFFSET[static_cast<size_t>(GameTeam::Count)] =
	{
		XMFLOAT2(-SCOREBOARD_SIZE.x / 2 + TEAM_PANEL_SIZE.x / 2 + 7, +SCOREBOARD_SIZE.y / 2 - 120),
		XMFLOAT2(+SCOREBOARD_SIZE.x / 2 - TEAM_PANEL_SIZE.x / 2 - 7, +SCOREBOARD_SIZE.y / 2 - 120)
	};

	UIObjectHandle hPanelRedTeamPanel = Runtime::GetInstance()->CreatePanel();
	Panel* pPanelRedTeamPanel = static_cast<Panel*>(hPanelRedTeamPanel.ToPtr());
	pPanelRedTeamPanel->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
	pPanelRedTeamPanel->m_transform.SetPosition(TEAM_PANEL_OFFSET[static_cast<size_t>(GameTeam::RedTeam)]);
	pPanelRedTeamPanel->SetSize(TEAM_PANEL_SIZE);
	pPanelRedTeamPanel->SetColor(ColorsLinear::Red);
	pPanelRedTeamPanel->SetColorA(0.4f);
	pPanelRedTeamPanel->SetShape(PanelShape::Rectangle);

	constexpr XMFLOAT2 TEAM_PANEL_TEXT_SIZE(TEAM_PANEL_SIZE.x - 10, TEAM_PANEL_SIZE.y);
	UIObjectHandle hTextRedTeamPanel = Runtime::GetInstance()->CreateText();
	Text* pTextRedTeamPanel = static_cast<Text*>(hTextRedTeamPanel.ToPtr());
	pTextRedTeamPanel->m_transform.SetParent(&pPanelRedTeamPanel->m_transform);
	pTextRedTeamPanel->m_transform.SetPosition(TEAM_PANEL_OFFSET[static_cast<size_t>(GameTeam::RedTeam)]);
	pTextRedTeamPanel->SetSize(TEAM_PANEL_TEXT_SIZE);
	pTextRedTeamPanel->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextRedTeamPanel->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextRedTeamPanel->GetTextFormat().SetSize(16);
	pTextRedTeamPanel->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextRedTeamPanel->ApplyTextFormat();
	pTextRedTeamPanel->SetText(L"RED TEAM");

	
	UIObjectHandle hPanelBlueTeamPanel = Runtime::GetInstance()->CreatePanel();
	Panel* pPanelBlueTeamPanel = static_cast<Panel*>(hPanelBlueTeamPanel.ToPtr());
	pPanelBlueTeamPanel->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
	pPanelBlueTeamPanel->m_transform.SetPosition(TEAM_PANEL_OFFSET[static_cast<size_t>(GameTeam::BlueTeam)]);
	pPanelBlueTeamPanel->SetSize(TEAM_PANEL_SIZE);
	pPanelBlueTeamPanel->SetColor(ColorsLinear::Blue);
	pPanelBlueTeamPanel->SetColorA(0.4f);
	pPanelBlueTeamPanel->SetShape(PanelShape::Rectangle);

	UIObjectHandle hTextBlueTeamPanel = Runtime::GetInstance()->CreateText();
	Text* pTextBlueTeamPanel = static_cast<Text*>(hTextBlueTeamPanel.ToPtr());
	pTextBlueTeamPanel->m_transform.SetParent(&pPanelBlueTeamPanel->m_transform);
	pTextBlueTeamPanel->m_transform.SetPosition(TEAM_PANEL_OFFSET[static_cast<size_t>(GameTeam::BlueTeam)]);
	pTextBlueTeamPanel->SetSize(TEAM_PANEL_TEXT_SIZE);
	pTextBlueTeamPanel->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextBlueTeamPanel->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextBlueTeamPanel->GetTextFormat().SetSize(16);
	pTextBlueTeamPanel->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextBlueTeamPanel->ApplyTextFormat();
	pTextBlueTeamPanel->SetText(L"BLUE TEAM");

	constexpr XMFLOAT2 PLAYER_INFO_COLUMNS_SIZE(TEAM_PANEL_SIZE.x - 10, 20);
	constexpr XMFLOAT2 PLAYER_INFO_COLUMNS_OFFSET[static_cast<size_t>(GameTeam::Count)] =
	{
		XMFLOAT2(TEAM_PANEL_OFFSET[static_cast<size_t>(GameTeam::RedTeam)].x, TEAM_PANEL_OFFSET[static_cast<size_t>(GameTeam::RedTeam)].y - TEAM_PANEL_SIZE.y / 2 - 3 - PLAYER_INFO_COLUMNS_SIZE.y),
		XMFLOAT2(TEAM_PANEL_OFFSET[static_cast<size_t>(GameTeam::BlueTeam)].x, TEAM_PANEL_OFFSET[static_cast<size_t>(GameTeam::BlueTeam)].y - TEAM_PANEL_SIZE.y / 2 - 3 - PLAYER_INFO_COLUMNS_SIZE.y)
	};
	UIObjectHandle hTextScoreboardRedTeamColumns = Runtime::GetInstance()->CreateText();
	Text* pTextScoreboardRedTeamColumns = static_cast<Text*>(hTextScoreboardRedTeamColumns.ToPtr());
	pTextScoreboardRedTeamColumns->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
	pTextScoreboardRedTeamColumns->m_transform.SetPosition(PLAYER_INFO_COLUMNS_OFFSET[static_cast<size_t>(GameTeam::RedTeam)]);
	pTextScoreboardRedTeamColumns->SetSize(PLAYER_INFO_COLUMNS_SIZE);
	pTextScoreboardRedTeamColumns->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextScoreboardRedTeamColumns->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextScoreboardRedTeamColumns->GetTextFormat().SetSize(14);
	pTextScoreboardRedTeamColumns->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextScoreboardRedTeamColumns->ApplyTextFormat();
	pTextScoreboardRedTeamColumns->SetText(L"  Lv.                             닉네임                             킬/데스              지연시간");

	constexpr FLOAT ROW_PITCHES_PER_ITEM = 20;
	constexpr FLOAT ITEM_ROW_SIZE = 20;
	constexpr FLOAT FIRST_ITEM_POS_Y = PLAYER_INFO_COLUMNS_OFFSET[static_cast<size_t>(GameTeam::RedTeam)].y - PLAYER_INFO_COLUMNS_SIZE.y / 2 - 10 - ITEM_ROW_SIZE / 2;
	constexpr XMFLOAT2 SCOREBOARD_LEVEL_TEXT_SIZE(40, 20);
	constexpr XMFLOAT2 SCOREBOARD_NICKNAME_TEXT_SIZE(200, 20);
	constexpr XMFLOAT2 SCOREBOARD_KILLDEATH_TEXT_SIZE(100, 20);
	constexpr XMFLOAT2 SCOREBOARD_PING_TEXT_SIZE(100, 20);
	for (size_t i = 0; i < static_cast<size_t>(GameTeam::Count); ++i)
	{
		for (size_t j = 0; j < MAX_PLAYERS_PER_TEAM; ++j)
		{
			const float yPos = FIRST_ITEM_POS_Y - j * (ITEM_ROW_SIZE + ROW_PITCHES_PER_ITEM);

			UIObjectHandle hTextScoreboardPlayerLevel = Runtime::GetInstance()->CreateText();
			m_hTextScoreboardPlayerLevel[i][j] = hTextScoreboardPlayerLevel;
			Text* pTextScoreboardPlayerLevel = static_cast<Text*>(hTextScoreboardPlayerLevel.ToPtr());
			pTextScoreboardPlayerLevel->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
			pTextScoreboardPlayerLevel->m_transform.SetPosition(
				TEAM_PANEL_OFFSET[i].x - TEAM_PANEL_SIZE.x / 2 + 10 + SCOREBOARD_LEVEL_TEXT_SIZE.x / 2,
				yPos
			);
			pTextScoreboardPlayerLevel->SetSize(SCOREBOARD_LEVEL_TEXT_SIZE);
			pTextScoreboardPlayerLevel->SetColor(ColorsLinear::WhiteSmoke);
			pTextScoreboardPlayerLevel->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pTextScoreboardPlayerLevel->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pTextScoreboardPlayerLevel->GetTextFormat().SetSize(14);
			pTextScoreboardPlayerLevel->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
			pTextScoreboardPlayerLevel->ApplyTextFormat();
			pTextScoreboardPlayerLevel->SetText(L"");



			UIObjectHandle hTextScoreboardPlayerNickname = Runtime::GetInstance()->CreateText();
			m_hTextScoreboardPlayerNickname[i][j] = hTextScoreboardPlayerNickname;
			Text* pTextScoreboardPlayerNickname = static_cast<Text*>(hTextScoreboardPlayerNickname.ToPtr());
			pTextScoreboardPlayerNickname->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
			pTextScoreboardPlayerNickname->m_transform.SetPosition(
				TEAM_PANEL_OFFSET[i].x - TEAM_PANEL_SIZE.x / 2 + 10 + SCOREBOARD_LEVEL_TEXT_SIZE.x + 10 + SCOREBOARD_NICKNAME_TEXT_SIZE.x / 2,
				yPos
			);
			pTextScoreboardPlayerNickname->SetSize(SCOREBOARD_NICKNAME_TEXT_SIZE);
			pTextScoreboardPlayerNickname->SetColor(ColorsLinear::WhiteSmoke);
			pTextScoreboardPlayerNickname->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pTextScoreboardPlayerNickname->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pTextScoreboardPlayerNickname->GetTextFormat().SetSize(14);
			pTextScoreboardPlayerNickname->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
			pTextScoreboardPlayerNickname->ApplyTextFormat();
			pTextScoreboardPlayerNickname->SetText(L"");

			UIObjectHandle hTextScoreboardPlayerKillDeath = Runtime::GetInstance()->CreateText();
			m_hTextScoreboardPlayerKillDeath[i][j] = hTextScoreboardPlayerKillDeath;
			Text* pTextScoreboardPlayerKillDeath = static_cast<Text*>(hTextScoreboardPlayerKillDeath.ToPtr());
			pTextScoreboardPlayerKillDeath->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
			pTextScoreboardPlayerKillDeath->m_transform.SetPosition(
				TEAM_PANEL_OFFSET[i].x - TEAM_PANEL_SIZE.x / 2 + 10 + SCOREBOARD_LEVEL_TEXT_SIZE.x + 10 + SCOREBOARD_NICKNAME_TEXT_SIZE.x + 10 + SCOREBOARD_KILLDEATH_TEXT_SIZE.x / 2,
				yPos
			);
			pTextScoreboardPlayerKillDeath->SetSize(SCOREBOARD_KILLDEATH_TEXT_SIZE);
			pTextScoreboardPlayerKillDeath->SetColor(ColorsLinear::WhiteSmoke);
			pTextScoreboardPlayerKillDeath->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pTextScoreboardPlayerKillDeath->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pTextScoreboardPlayerKillDeath->GetTextFormat().SetSize(14);
			pTextScoreboardPlayerKillDeath->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
			pTextScoreboardPlayerKillDeath->ApplyTextFormat();
			pTextScoreboardPlayerKillDeath->SetText(L"");

			UIObjectHandle hTextScoreboardPlayerPing = Runtime::GetInstance()->CreateText();
			m_hTextScoreboardPlayerPing[i][j] = hTextScoreboardPlayerPing;
			Text* pTextScoreboardPlayerPing = static_cast<Text*>(hTextScoreboardPlayerPing.ToPtr());
			pTextScoreboardPlayerPing->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
			pTextScoreboardPlayerPing->m_transform.SetPosition(
				TEAM_PANEL_OFFSET[i].x - TEAM_PANEL_SIZE.x / 2 + 10 + SCOREBOARD_LEVEL_TEXT_SIZE.x + 10 + SCOREBOARD_NICKNAME_TEXT_SIZE.x + 10 + SCOREBOARD_KILLDEATH_TEXT_SIZE.x + 10 + SCOREBOARD_PING_TEXT_SIZE.x / 2,
				yPos
			);
			pTextScoreboardPlayerPing->SetSize(SCOREBOARD_PING_TEXT_SIZE);
			pTextScoreboardPlayerPing->SetColor(ColorsLinear::WhiteSmoke);
			pTextScoreboardPlayerPing->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pTextScoreboardPlayerPing->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pTextScoreboardPlayerPing->GetTextFormat().SetSize(14);
			pTextScoreboardPlayerPing->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
			pTextScoreboardPlayerPing->ApplyTextFormat();
			pTextScoreboardPlayerPing->SetText(L"");
		}
	}

	UIObjectHandle hTextScoreboardBlueTeamColumns = Runtime::GetInstance()->CreateText();
	Text* pTextScoreboardBlueTeamColumns = static_cast<Text*>(hTextScoreboardBlueTeamColumns.ToPtr());
	pTextScoreboardBlueTeamColumns->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
	pTextScoreboardBlueTeamColumns->m_transform.SetPosition(PLAYER_INFO_COLUMNS_OFFSET[static_cast<size_t>(GameTeam::BlueTeam)]);
	pTextScoreboardBlueTeamColumns->SetSize(PLAYER_INFO_COLUMNS_SIZE);
	pTextScoreboardBlueTeamColumns->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextScoreboardBlueTeamColumns->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextScoreboardBlueTeamColumns->GetTextFormat().SetSize(14);
	pTextScoreboardBlueTeamColumns->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextScoreboardBlueTeamColumns->ApplyTextFormat();
	pTextScoreboardBlueTeamColumns->SetText(L"  Lv.                             닉네임                             킬/데스              지연시간");



	
	// ##### 게임 메뉴 UI 생성 #####
	UIObjectHandle hPanelMenuRoot = Runtime::GetInstance()->CreatePanel();
	m_hPanelMenuRoot = hPanelMenuRoot;
	Panel* pPanelMenuRoot = static_cast<Panel*>(hPanelMenuRoot.ToPtr());
	pPanelMenuRoot->SetSize(200, 300);
	pPanelMenuRoot->SetShape(PanelShape::RoundedRectangle);
	pPanelMenuRoot->SetColor(Colors::DimGray);

	UIObjectHandle hButtonCloseGameMenu = Runtime::GetInstance()->CreateButton();
	Button* pButtonCloseGameMenu = static_cast<Button*>(hButtonCloseGameMenu.ToPtr());
	pButtonCloseGameMenu->m_transform.SetParent(&pPanelMenuRoot->m_transform);
	pButtonCloseGameMenu->SetSize(20, 20);
	pButtonCloseGameMenu->m_transform.SetPosition(
		pPanelMenuRoot->GetHalfSizeX() - pButtonCloseGameMenu->GetHalfSizeX() - 5,
		pPanelMenuRoot->GetHalfSizeY() - pButtonCloseGameMenu->GetHalfSizeY() - 5
	);
	pButtonCloseGameMenu->SetText(L"X");
	pButtonCloseGameMenu->SetButtonColor(ColorsLinear::Red);
	pButtonCloseGameMenu->SetHandlerOnClick(MakeUIHandler(ComponentHandle<GameUIManager>(this->ToHandle()), &GameUIManager::OnClickCloseGameMenu));

	UIObjectHandle hTextSliderControlPlayerFoV = Runtime::GetInstance()->CreateText();
	Text* pTextSliderControlPlayerFoV = static_cast<Text*>(hTextSliderControlPlayerFoV.ToPtr());
	pTextSliderControlPlayerFoV->m_transform.SetParent(&pPanelMenuRoot->m_transform);
	pTextSliderControlPlayerFoV->m_transform.SetPosition(-70, 80);
	pTextSliderControlPlayerFoV->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextSliderControlPlayerFoV->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextSliderControlPlayerFoV->SetText(L"FoV");
	pTextSliderControlPlayerFoV->SetSize(30, 20);

	UIObjectHandle hSliderControlPlayerFoV = Runtime::GetInstance()->CreateSliderControl();
	m_hSliderControlPlayerFoV = hSliderControlPlayerFoV;	// 핸들 저장
	SliderControl* pSliderControlPlayerFoV = static_cast<SliderControl*>(hSliderControlPlayerFoV.ToPtr());
	pSliderControlPlayerFoV->m_transform.SetParent(&pPanelMenuRoot->m_transform);
	// pSliderControlPlayerFoV->SetSliderControlType(SliderControlType::Horizontal);	// 기본값
	pSliderControlPlayerFoV->SetTrackLength(120);
	pSliderControlPlayerFoV->SetRange(70, 99);
	pSliderControlPlayerFoV->SetThumbPos(82);
	pSliderControlPlayerFoV->m_transform.SetPosition(
		pTextSliderControlPlayerFoV->m_transform.GetPositionX() + pTextSliderControlPlayerFoV->GetHalfSizeX() / 2 + 10 + pSliderControlPlayerFoV->GetTrackLength() / 2,
		pTextSliderControlPlayerFoV->m_transform.GetPositionY()
	);
	pSliderControlPlayerFoV->SetHandlerOnPosChange(MakeUIHandler(ComponentHandle<GameUIManager>(this->ToHandle()), &GameUIManager::OnPosChangePlayerFoV));

	UIObjectHandle hCheckboxDrawDebugInfo = Runtime::GetInstance()->CreateCheckbox();
	m_hCheckboxDrawDebugInfo = hCheckboxDrawDebugInfo;
	Checkbox* pCheckboxDrawDebugInfo = static_cast<Checkbox*>(hCheckboxDrawDebugInfo.ToPtr());
	pCheckboxDrawDebugInfo->m_transform.SetParent(&pPanelMenuRoot->m_transform);
	pCheckboxDrawDebugInfo->m_transform.SetPosition(60, 0);
	pCheckboxDrawDebugInfo->SetCheck(Physics::GetInstance()->GetDrawDebugInfo());
	pCheckboxDrawDebugInfo->SetText(L"콜라이더 디버그 그리기");
	pCheckboxDrawDebugInfo->SetTextboxSize(150);
	pCheckboxDrawDebugInfo->SetBoxColor(ColorsLinear::Orange);
	pCheckboxDrawDebugInfo->SetCheckColor(ColorsLinear::Black);
	pCheckboxDrawDebugInfo->SetLeftText(true);
	pCheckboxDrawDebugInfo->SetHandlerOnClick(MakeUIHandler(ComponentHandle<GameUIManager>(this->ToHandle()), &GameUIManager::OnClickDrawDebugInfo));

	UIObjectHandle hButtonExitGame = Runtime::GetInstance()->CreateButton();
	Button* pButtonExitGame = static_cast<Button*>(hButtonExitGame.ToPtr());
	pButtonExitGame->m_transform.SetParent(&pPanelMenuRoot->m_transform);
	pButtonExitGame->m_transform.SetPosition(0, -70);
	pButtonExitGame->SetSize(80, 20);
	pButtonExitGame->SetText(L"나가기");
	pButtonExitGame->SetHandlerOnClick(MakeUIHandler(ComponentHandle<GameUIManager>(this->ToHandle()), &GameUIManager::OnClickCloseGameMenu));




	// ##### 게임 UI 생성 #####
	UIObjectHandle hImageGameUIRoot = Runtime::GetInstance()->CreateImage();
	m_hImageGameUIRoot = hImageGameUIRoot;
	Image* pImageGameUIRoot = static_cast<Image*>(hImageGameUIRoot.ToPtr());

	UIObjectHandle hImageCrosshair = Runtime::GetInstance()->CreateImage();
	m_hImageCrosshair = hImageCrosshair;
	Image* pImageCrosshair = static_cast<Image*>(hImageCrosshair.ToPtr());
	pImageCrosshair->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pImageCrosshair->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\crosshair.dds"));
	pImageCrosshair->SetNativeSize(true);
	pImageCrosshair->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pImageCrosshair->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);

	UIObjectHandle hImageTeamScore = Runtime::GetInstance()->CreateImage();
	Image* pImageTeamScore = static_cast<Image*>(hImageTeamScore.ToPtr());
	pImageTeamScore->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pImageTeamScore->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\team_score_indicator.png"));
	pImageTeamScore->SetNativeSize(true);
	pImageTeamScore->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pImageTeamScore->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pImageTeamScore->m_transform.SetPosition(0, -pImageTeamScore->GetHalfSizeY() - 4);

	UIObjectHandle hImageHPAPBackground = Runtime::GetInstance()->CreateImage();
	m_hImageHPAPBackground = hImageHPAPBackground;
	Image* pImageHPAPBackground = static_cast<Image*>(hImageHPAPBackground.ToPtr());
	pImageHPAPBackground->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pImageHPAPBackground->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\hpap_indicator.png"));
	pImageHPAPBackground->SetNativeSize(true);
	pImageHPAPBackground->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pImageHPAPBackground->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pImageHPAPBackground->m_transform.SetPosition(pImageHPAPBackground->GetHalfSizeX() + 4, pImageHPAPBackground->GetHalfSizeY() + 4);

	UIObjectHandle hImageRBUIBackground = Runtime::GetInstance()->CreateImage();
	m_hImageRBUIBackground = hImageRBUIBackground;
	Image* pImageRBUIBackground = static_cast<Image*>(hImageRBUIBackground.ToPtr());
	pImageRBUIBackground->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pImageRBUIBackground->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapon_indicator.png"));
	pImageRBUIBackground->SetNativeSize(true);
	pImageRBUIBackground->m_transform.SetPosition(-pImageRBUIBackground->GetHalfSizeX() - 4, pImageRBUIBackground->GetHalfSizeY() + 4);
	pImageRBUIBackground->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
	pImageRBUIBackground->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);

	UIObjectHandle hTextGameRemainingTime = Runtime::GetInstance()->CreateText();
	m_hTextGameRemainingTime = hTextGameRemainingTime;
	Text* pTextGameRemainingTime = static_cast<Text*>(hTextGameRemainingTime.ToPtr());
	pTextGameRemainingTime->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pTextGameRemainingTime->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pTextGameRemainingTime->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pTextGameRemainingTime->m_transform.SetPosition(100, -400);
	pTextGameRemainingTime->SetSize(200, 50);
	pTextGameRemainingTime->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameRemainingTime->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameRemainingTime->SetColor(ColorsLinear::WhiteSmoke);
	pTextGameRemainingTime->GetTextFormat().SetSize(28);
	pTextGameRemainingTime->GetTextFormat().SetFontFamilyName(L"Impact");
	pTextGameRemainingTime->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextGameRemainingTime->ApplyTextFormat();
	pTextGameRemainingTime->SetText(L"");

	UIObjectHandle hTextRedTeamScore = Runtime::GetInstance()->CreateText();
	m_hTextRedTeamScore = hTextRedTeamScore;
	Text* pTextRedTeamScore = static_cast<Text*>(hTextRedTeamScore.ToPtr());
	pTextRedTeamScore->m_transform.SetParent(&pImageTeamScore->m_transform);
	pTextRedTeamScore->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextRedTeamScore->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pTextRedTeamScore->m_transform.SetPosition(pImageTeamScore->m_transform.GetPosition());
	pTextRedTeamScore->m_transform.TranslateX(-82);
	pTextRedTeamScore->SetSize(80, 28);
	pTextRedTeamScore->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextRedTeamScore->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextRedTeamScore->SetColor(ColorsLinear::WhiteSmoke);
	pTextRedTeamScore->GetTextFormat().SetSize(20);
	pTextRedTeamScore->GetTextFormat().SetFontFamilyName(L"Impact");
	pTextRedTeamScore->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextRedTeamScore->ApplyTextFormat();
	pTextRedTeamScore->SetText(L"000");

	UIObjectHandle hTextTeamScoreGoal = Runtime::GetInstance()->CreateText();
	m_hTextTeamScoreGoal = hTextTeamScoreGoal;
	Text* pTextTeamScoreGoal = static_cast<Text*>(hTextTeamScoreGoal.ToPtr());
	pTextTeamScoreGoal->m_transform.SetParent(&pImageTeamScore->m_transform);
	pTextTeamScoreGoal->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextTeamScoreGoal->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pTextTeamScoreGoal->m_transform.SetPosition(pImageTeamScore->m_transform.GetPosition());
	pTextTeamScoreGoal->SetSize(80, 28);
	pTextTeamScoreGoal->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextTeamScoreGoal->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextTeamScoreGoal->SetColor(ColorsLinear::WhiteSmoke);
	pTextTeamScoreGoal->GetTextFormat().SetSize(20);
	pTextTeamScoreGoal->GetTextFormat().SetFontFamilyName(L"Impact");
	pTextTeamScoreGoal->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextTeamScoreGoal->ApplyTextFormat();
	pTextTeamScoreGoal->SetText(L"100");

	UIObjectHandle hTextBlueTeamScore = Runtime::GetInstance()->CreateText();
	m_hTextBlueTeamScore = hTextBlueTeamScore;
	Text* pTextBlueTeamScore = static_cast<Text*>(hTextBlueTeamScore.ToPtr());
	pTextBlueTeamScore->m_transform.SetParent(&pImageTeamScore->m_transform);
	pTextBlueTeamScore->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextBlueTeamScore->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pTextBlueTeamScore->m_transform.SetPosition(pImageTeamScore->m_transform.GetPosition());
	pTextBlueTeamScore->m_transform.TranslateX(+82);
	pTextBlueTeamScore->SetSize(80, 28);
	pTextBlueTeamScore->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextBlueTeamScore->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextBlueTeamScore->SetColor(ColorsLinear::WhiteSmoke);
	pTextBlueTeamScore->GetTextFormat().SetSize(20);
	pTextBlueTeamScore->GetTextFormat().SetFontFamilyName(L"Impact");
	pTextBlueTeamScore->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextBlueTeamScore->ApplyTextFormat();
	pTextBlueTeamScore->SetText(L"000");


	UIObjectHandle hTextHP = Runtime::GetInstance()->CreateText();
	m_hTextHP = hTextHP;
	Text* pTextHP = static_cast<Text*>(hTextHP.ToPtr());
	pTextHP->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pTextHP->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pTextHP->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pTextHP->m_transform.SetPosition(pImageHPAPBackground->m_transform.GetPosition());
	pTextHP->m_transform.TranslateX(-56);
	pTextHP->SetSize(128, 48);
	pTextHP->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextHP->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextHP->SetColor(ColorsLinear::WhiteSmoke);
	pTextHP->GetTextFormat().SetSize(28);
	pTextHP->GetTextFormat().SetFontFamilyName(L"Impact");
	pTextHP->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextHP->ApplyTextFormat();

	UIObjectHandle hTextAP = Runtime::GetInstance()->CreateText();
	m_hTextAP = hTextAP;
	Text* pTextAP = static_cast<Text*>(hTextAP.ToPtr());
	pTextAP->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pTextAP->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pTextAP->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pTextAP->m_transform.SetPosition(pImageHPAPBackground->m_transform.GetPosition());
	pTextAP->m_transform.TranslateX(120);
	pTextAP->SetSize(128, 48);
	pTextAP->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextAP->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextAP->SetColor(ColorsLinear::WhiteSmoke);
	pTextAP->GetTextFormat().SetSize(28);
	pTextAP->GetTextFormat().SetFontFamilyName(L"Impact");
	pTextAP->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextAP->ApplyTextFormat();

	UIObjectHandle hTextPoint = Runtime::GetInstance()->CreateText();
	m_hTextPoint = hTextPoint;
	Text* pTextPoint = static_cast<Text*>(hTextPoint.ToPtr());
	pTextPoint->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pTextPoint->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
	pTextPoint->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pTextPoint->m_transform.SetPosition(pImageRBUIBackground->m_transform.GetPosition());
	pTextPoint->m_transform.Translate(-45, 34);
	pTextPoint->SetSize(128, 32);
	pTextPoint->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextPoint->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextPoint->SetColor(ColorsLinear::WhiteSmoke);
	pTextPoint->GetTextFormat().SetSize(22);
	pTextPoint->GetTextFormat().SetFontFamilyName(GAME_UI_TEXT_FONT);
	pTextPoint->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
	pTextPoint->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
	pTextPoint->ApplyTextFormat();
	pTextPoint->SetText(L"-");

	UIObjectHandle hTextWeaponName = Runtime::GetInstance()->CreateText();
	m_hTextWeaponName = hTextWeaponName;
	Text* pTextWeaponName = static_cast<Text*>(hTextWeaponName.ToPtr());
	pTextWeaponName->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pTextWeaponName->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
	pTextWeaponName->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pTextWeaponName->m_transform.SetPosition(pImageRBUIBackground->m_transform.GetPosition());
	pTextWeaponName->m_transform.TranslateY(6);
	pTextWeaponName->SetSize(XMFLOAT2(200, 32));
	pTextWeaponName->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextWeaponName->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextWeaponName->SetColor(ColorsLinear::WhiteSmoke);
	pTextWeaponName->GetTextFormat().SetSize(22);
	pTextWeaponName->GetTextFormat().SetFontFamilyName(GAME_UI_TEXT_FONT);
	pTextWeaponName->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
	pTextWeaponName->ApplyTextFormat();

	UIObjectHandle hTextAmmoState = Runtime::GetInstance()->CreateText();
	m_hTextAmmoState = hTextAmmoState;
	Text* pTextAmmoState = static_cast<Text*>(hTextAmmoState.ToPtr());
	pTextAmmoState->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pTextAmmoState->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
	pTextAmmoState->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pTextAmmoState->m_transform.SetPosition(pImageRBUIBackground->m_transform.GetPosition());
	pTextAmmoState->m_transform.Translate(16, -28);
	pTextAmmoState->SetSize(XMFLOAT2(128, 40));
	pTextAmmoState->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextAmmoState->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextAmmoState->SetColor(ColorsLinear::WhiteSmoke);
	pTextAmmoState->GetTextFormat().SetSize(34);
	pTextAmmoState->GetTextFormat().SetFontFamilyName(GAME_UI_TEXT_FONT);
	pTextAmmoState->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
	pTextAmmoState->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
	pTextAmmoState->ApplyTextFormat();

	UIObjectHandle hTextRespawnIndicator = Runtime::GetInstance()->CreateText();
	m_hTextRespawnIndicator = hTextRespawnIndicator;
	Text* pTextRespawnIndicator = static_cast<Text*>(hTextRespawnIndicator.ToPtr());
	pTextRespawnIndicator->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pTextRespawnIndicator->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pTextRespawnIndicator->m_transform.SetPosition(0, +300);
	pTextRespawnIndicator->SetSize(400, 100);
	pTextRespawnIndicator->GetTextFormat().SetSize(24);
	pTextRespawnIndicator->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
	pTextRespawnIndicator->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_HEAVY);
	pTextRespawnIndicator->ApplyTextFormat();
	pTextRespawnIndicator->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextRespawnIndicator->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextRespawnIndicator->SetColor(ColorsLinear::WhiteSmoke);
	pTextRespawnIndicator->SetText(L"R E S P A W N\n초 남았습니다.");







	// 채팅 패널 UI
	constexpr XMFLOAT2 INGAME_CHAT_PANEL_SIZE(500, 220);
	UIObjectHandle hPanelChatBackground = Runtime::GetInstance()->CreatePanel();
	m_hPanelChatBackground = hPanelChatBackground;
	Panel* pPanelChatBackground = static_cast<Panel*>(hPanelChatBackground.ToPtr());
	pPanelChatBackground->m_transform.SetParent(&pImageGameUIRoot->m_transform);		// 중요 (GameUI의 자식으로 추가)
	pPanelChatBackground->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pPanelChatBackground->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pPanelChatBackground->m_transform.SetPosition(INGAME_CHAT_PANEL_SIZE.x / 2 + 10, INGAME_CHAT_PANEL_SIZE.y / 2 + 200);
	pPanelChatBackground->SetSize(INGAME_CHAT_PANEL_SIZE);
	pPanelChatBackground->SetShape(PanelShape::RoundedRectangle);
	pPanelChatBackground->SetColor(Colors::DimGray);
	pPanelChatBackground->SetColorA(CHAT_PANEL_ALPHA);
	
	const XMFLOAT2 CHAT_MSG_TEXT_SIZE(INGAME_CHAT_PANEL_SIZE.x - 20, 20);
	for (size_t i = 0; i < INGAME_CHAT_MSG_ITEM_ROW_COUNT; ++i)
	{
		m_hTextChatMsg[i] = Runtime::GetInstance()->CreateText();
		Text* pTextChatMsg = static_cast<Text*>(m_hTextChatMsg[i].ToPtr());
		pTextChatMsg->m_transform.SetParent(&pImageGameUIRoot->m_transform);	// 중요 (GameUI의 자식으로 추가)
		pTextChatMsg->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextChatMsg->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pTextChatMsg->m_transform.SetPosition(
			pPanelChatBackground->m_transform.GetPositionX(),
			pPanelChatBackground->m_transform.GetPositionY() + INGAME_CHAT_PANEL_SIZE.y / 2 - CHAT_MSG_TEXT_SIZE.y / 2 - 10 - CHAT_MSG_TEXT_SIZE.y * i
		);
		pTextChatMsg->SetSize(CHAT_MSG_TEXT_SIZE);
		// pTextChatMsg->SetText();
	}

	const XMFLOAT2 CHAT_MSG_INPUT_FIELD_SIZE(INGAME_CHAT_PANEL_SIZE.x - 20, 20);
	m_hInputFieldChatMsg = Runtime::GetInstance()->CreateInputField();
	InputField* pInputFieldChatMsg = static_cast<InputField*>(m_hInputFieldChatMsg.ToPtr());
	pInputFieldChatMsg->m_transform.SetParent(&pPanelChatBackground->m_transform);
	pInputFieldChatMsg->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pInputFieldChatMsg->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pInputFieldChatMsg->m_transform.SetPosition(
		pPanelChatBackground->m_transform.GetPositionX(),
		pPanelChatBackground->m_transform.GetPositionY() - INGAME_CHAT_PANEL_SIZE.y / 2 + CHAT_MSG_INPUT_FIELD_SIZE.y / 2 + 10
	);
	pInputFieldChatMsg->SetSize(CHAT_MSG_INPUT_FIELD_SIZE);
	pInputFieldChatMsg->SetMaxChar(MAX_CHAT_MSG_LEN);

	
	// ###########################################################
	m_hPanelAdapterInfoRoot.ToPtr()->DontDestroyOnLoadRecursively();
	m_hPanelScoreboardRoot.ToPtr()->DontDestroyOnLoadRecursively();
	m_hPanelMenuRoot.ToPtr()->DontDestroyOnLoadRecursively();
	m_hImageGameUIRoot.ToPtr()->DontDestroyOnLoadRecursively();
	// ###########################################################

	this->SetState(GameUIStateDeactivate::GetState());
}

void GameUIManager::Update()
{
	const float dt = Time::GetInstance()->GetDeltaTime();

	// UI 상태머신 업데이트
	if (m_pUIState)
		m_pUIState->Update(this, dt);

	// 리스폰 UI 업데이트
	if (m_activeRespawnUI)
	{
		m_respawnRemainingTime = (std::max)(m_respawnRemainingTime - dt, 0.0f);

		Text* pTextRespawnIndicator = static_cast<Text*>(m_hTextRespawnIndicator.ToPtr());
		wchar_t buf[32];
		StringCchPrintfW(buf, _countof(buf), L"R E S P A W N\n%d초 남았습니다.", static_cast<int>(m_respawnRemainingTime));
		pTextRespawnIndicator->SetText(buf);

		if (m_respawnRemainingTime == 0.0f)
		{
			m_hTextRespawnIndicator.ToPtr()->SetActive(false);
			m_activeRespawnUI = false;
		}
	}

	if (m_needUpdateChatMsgTransparency)
	{
		m_chatMsgTransparencyTimer = (std::max)(m_chatMsgTransparencyTimer - dt, 0.0f);

		constexpr float CHAT_MSG_TRANSPARENCY_START_TIME = 2.5f;
		if (m_chatMsgTransparencyTimer <= CHAT_MSG_TRANSPARENCY_START_TIME)
		{
			for (size_t i = 0; i < _countof(m_hTextChatMsg); ++i)
			{
				Text* pTextChatMsg = static_cast<Text*>(m_hTextChatMsg[i].ToPtr());
				pTextChatMsg->SetColorA(m_chatMsgTransparencyTimer / CHAT_MSG_TRANSPARENCY_START_TIME);
			}
		}

		if (m_chatMsgTransparencyTimer == 0.0f)
		{
			m_needUpdateChatMsgTransparency = false;
		}
	}
}

void GameUIManager::LateUpdate()
{
	// Update 루틴에서 카메라 최종 위치 결정된 상태로 가정
	// -> 카메라 행렬, 투영 행렬, 뷰포트 변환 및 화면 좌표계에서의 좌표를 구해서 캐릭터 위 닉네임에 대한 스크린 좌표 위치 획득 및 Text UI 이동
	// 코드 구현...


}

void GameUIManager::Init()
{
	for (size_t i = 0; i < static_cast<size_t>(GameTeam::Count); ++i)
	{
		for (size_t j = 0; j < MAX_PLAYERS_PER_TEAM; ++j)
		{
			Text* pTextScoreboardPlayerLevel = static_cast<Text*>(m_hTextScoreboardPlayerLevel[i][j].ToPtr());
			if (pTextScoreboardPlayerLevel)
				pTextScoreboardPlayerLevel->GetText().clear();

			Text* pTextScoreboardPlayerNickname = static_cast<Text*>(m_hTextScoreboardPlayerNickname[i][j].ToPtr());
			if (pTextScoreboardPlayerNickname)
				pTextScoreboardPlayerNickname->GetText().clear();

			Text* pTextScoreboardPlayerKillDeath = static_cast<Text*>(m_hTextScoreboardPlayerKillDeath[i][j].ToPtr());
			if (pTextScoreboardPlayerKillDeath)
				pTextScoreboardPlayerKillDeath->GetText().clear();

			Text* pTextScoreboardPlayerPing = static_cast<Text*>(m_hTextScoreboardPlayerPing[i][j].ToPtr());
			if (pTextScoreboardPlayerPing)
				pTextScoreboardPlayerPing->GetText().clear();
		}
	}

	for (size_t i = 0; i < static_cast<size_t>(GameTeam::Count); ++i)
	{
		m_scoreboardPlayerAccountId[i].clear();
	}

	for (size_t i = 0; i < _countof(m_hTextChatMsg); ++i)
	{
		Text* pTextChatMsg = static_cast<Text*>(m_hTextChatMsg[i].ToPtr());
		if (pTextChatMsg)
			pTextChatMsg->GetText().clear();
	}

	InputField* pInputFieldChatMsg = static_cast<InputField*>(m_hInputFieldChatMsg.ToPtr());
	if (pInputFieldChatMsg)
		pInputFieldChatMsg->GetText().clear();
}

void GameUIManager::SetTextGameRemainingTime(float time)
{
	const uint32_t intTime = static_cast<uint32_t>(time);
	const uint32_t m = intTime / 60;
	const uint32_t s = intTime % 60;

	wchar_t buf[32];
	StringCchPrintfW(buf, _countof(buf), L"%02u : %02u", m, s);
	static_cast<Text*>(m_hTextGameRemainingTime.ToPtr())->SetText(buf);
}

void GameUIManager::SetTextHP(uint32_t hp)
{
	wchar_t buf[32];
	StringCchPrintfW(buf, _countof(buf), L"%u", hp);
	Text* pTextHP = static_cast<Text*>(m_hTextHP.ToPtr());
	pTextHP->SetText(buf);

	constexpr int MAX_HP = 100;
	constexpr int MIN_HP = 0;
	const float lerpFactor = static_cast<float>(hp) / static_cast<float>(MAX_HP - MIN_HP);
	XMVECTOR color = XMVectorLerp(ColorsLinear::OrangeRed, ColorsLinear::PaleGreen, lerpFactor);

	pTextHP->SetColor(color);
}

void GameUIManager::SetTextAP(uint32_t ap)
{
	wchar_t buf[32];
	StringCchPrintfW(buf, _countof(buf), L"%u", ap);
	Text* pTextAP = static_cast<Text*>(m_hTextAP.ToPtr());
	pTextAP->SetText(buf);

	constexpr int MAX_AP = 100;
	constexpr int MIN_AP = 0;
	const float lerpFactor = static_cast<float>(ap) / static_cast<float>(MAX_AP - MIN_AP);
	XMVECTOR color = XMVectorLerp(ColorsLinear::OrangeRed, ColorsLinear::DeepSkyBlue, lerpFactor);
	
	pTextAP->SetColor(color);
}

void GameUIManager::SetTextAmmoState(const wchar_t* str)
{
	static_cast<Text*>(m_hTextAmmoState.ToPtr())->SetText(str);
}

void GameUIManager::ClearTextAmmoState()
{
	static_cast<Text*>(m_hTextAmmoState.ToPtr())->GetText().clear();
}

void GameUIManager::SetTextPoint(uint32_t point)
{
	wchar_t buf[32];
	StringCchPrintfW(buf, _countof(buf), L"%u", point);

	static_cast<Text*>(m_hTextPoint.ToPtr())->SetText(buf);
}

void GameUIManager::SetTextWeaponName(const wchar_t* name)
{
	static_cast<Text*>(m_hTextWeaponName.ToPtr())->SetText(name);
}

void GameUIManager::ClearTextWeaponName()
{
	static_cast<Text*>(m_hTextWeaponName.ToPtr())->GetText().clear();
}

void GameUIManager::SetState(IGameUIManagerState* pUIState)
{
	IGameUIManagerState* pOldState = m_pUIState;

	if (pOldState)
		pOldState->Exit(this);

	m_pUIState = pUIState;

	if (m_pUIState)
		m_pUIState->Enter(this);
}

Player* GameUIManager::GetPlayerScript() const
{
	return m_hScriptPlayer.ToPtr();
}

void GameUIManager::ShowAdapterInfo()
{
	m_hPanelAdapterInfoRoot.ToPtr()->SetActive(true);
}

void GameUIManager::HideAdapterInfo()
{
	m_hPanelAdapterInfoRoot.ToPtr()->SetActive(false);
}

void GameUIManager::ShowScoreboard()
{
	m_hPanelScoreboardRoot.ToPtr()->SetActive(true);
}

void GameUIManager::HideScoreboard()
{
	m_hPanelScoreboardRoot.ToPtr()->SetActive(false);
}

void GameUIManager::ShowMenu()
{
	m_hPanelMenuRoot.ToPtr()->SetActive(true);
}

void GameUIManager::HideMenu()
{
	m_hPanelMenuRoot.ToPtr()->SetActive(false);
}

void GameUIManager::ShowGameUI()
{
	m_hImageGameUIRoot.ToPtr()->SetActive(true);

	for (size_t i = 0; i < _countof(m_hTextChatMsg); ++i)
		m_hTextChatMsg[i].ToPtr()->SetActive(true);

	if (m_activeRespawnUI)
		m_hTextRespawnIndicator.ToPtr()->SetActive(true);
}

void GameUIManager::HideGameUI()
{
	m_hImageGameUIRoot.ToPtr()->SetActive(false);

	for (size_t i = 0; i < _countof(m_hTextChatMsg); ++i)
		m_hTextChatMsg[i].ToPtr()->SetActive(false);

	m_hTextRespawnIndicator.ToPtr()->SetActive(false);
}

void GameUIManager::ShowChatPanel()
{
	m_hPanelChatBackground.ToPtr()->SetActive(true);

	for (size_t i = 0; i < _countof(m_hTextChatMsg); ++i)
	{
		Text* pTextChatMsg = static_cast<Text*>(m_hTextChatMsg[i].ToPtr());
		pTextChatMsg->SetColorA(1.0f);
	}

	m_needUpdateChatMsgTransparency = false;

	UIObjectManager::GetInstance()->SetFocusedUI(m_hInputFieldChatMsg.ToPtr());
}

void GameUIManager::HideChatPanel()
{
	m_hPanelChatBackground.ToPtr()->SetActive(false);

	m_chatMsgTransparencyTimer = 6.5f;
	m_needUpdateChatMsgTransparency = true;

	UIObjectManager::GetInstance()->SetFocusedUI(nullptr);
}

void GameUIManager::OnClickCloseGameMenu()
{
	assert(GetState() == GameUIStateMenu::GetState());

	this->SetState(GameUIStatePlaying::GetState());
}

void GameUIManager::ClearAllChatMsgs()
{
	for (size_t i = 0; i < _countof(m_hTextChatMsg); ++i)
	{
		Text* pTextChatMsg = static_cast<Text*>(m_hTextChatMsg[i].ToPtr());
		pTextChatMsg->GetText().clear();
	}

	m_chatMsgCount = 0;
}

void GameUIManager::AddChatMsg(const wchar_t* msg)
{
	m_chatMsgTransparencyTimer = 6.5f;
	m_needUpdateChatMsgTransparency = true;

	for (size_t i = 0; i < _countof(m_hTextChatMsg); ++i)
	{
		Text* pTextChatMsg = static_cast<Text*>(m_hTextChatMsg[i].ToPtr());
		pTextChatMsg->SetColorA(1.0f);
	}

	if (m_chatMsgCount < _countof(m_hTextChatMsg))
	{
		static_cast<Text*>(m_hTextChatMsg[m_chatMsgCount++].ToPtr())->SetText(msg);
	}
	else
	{
		for (size_t i = 0; i < _countof(m_hTextChatMsg) - 1; ++i)
		{
			static_cast<Text*>(m_hTextChatMsg[i].ToPtr())->SetText(
				static_cast<Text*>(m_hTextChatMsg[i + 1].ToPtr())->GetText().c_str()
			);
		}
		static_cast<Text*>(m_hTextChatMsg[_countof(m_hTextChatMsg) - 1].ToPtr())->SetText(msg);
	}
}

void GameUIManager::SendChatMsg()
{
	InputField* pInputFieldChatMsg = static_cast<InputField*>(m_hInputFieldChatMsg.ToPtr());
	uint16_t chatMsgLen = static_cast<uint16_t>(pInputFieldChatMsg->GetText().length());
	
	if (chatMsgLen > 0)
	{
		LSCSReqChat req;
		const Account* pAccount = m_hScriptAccount.ToPtr();

		req.m_protocol = LSProtocol::CS_REQ_CHAT;
		req.m_accountId = pAccount->GetAccountId();
		req.m_chatMsgLen = chatMsgLen;
		wmemcpy(req.m_chatMsg, pInputFieldChatMsg->GetText().c_str(), (std::min)(MAX_CHAT_MSG_LEN, static_cast<size_t>(chatMsgLen)));

		ENetPacket* pPkt = enet_packet_create(&req, sizeof(req), ENET_PACKET_FLAG_RELIABLE);
		ListenServerClient* pScriptListenServerClient = m_hScriptListenServerClient.ToPtr();
		if (!pScriptListenServerClient->SendPacket(pPkt))
		{
			enet_packet_destroy(pPkt);
			pPkt = nullptr;
		}

		pInputFieldChatMsg->GetText().clear();
	}
}

void GameUIManager::StartRespawnUI(float time)
{
	m_activeRespawnUI = true;
	m_respawnRemainingTime = time;

	Text* pTextRespawnIndicator = static_cast<Text*>(m_hTextRespawnIndicator.ToPtr());
	pTextRespawnIndicator->SetActive(true);


	wchar_t buf[32];
	StringCchPrintfW(buf, _countof(buf), L"R E S P A W N\n%d초 남았습니다.", static_cast<int>(m_respawnRemainingTime));
	pTextRespawnIndicator->SetText(buf);
}

void GameUIManager::AddPlayer(uint32_t accountId, GameTeam team, uint16_t level, const wchar_t* nickname, uint32_t kill, uint32_t death, uint32_t ping)
{
	const auto& v = m_scoreboardPlayerAccountId[static_cast<size_t>(team)];

	const size_t playerIndex = v.size();
	assert(playerIndex < MAX_PLAYERS_PER_TEAM);

	m_scoreboardPlayerAccountId[static_cast<size_t>(team)].push_back(accountId);

	wchar_t buf[32];

	// Level 표시
	StringCchPrintfW(buf, _countof(buf), L"%u", static_cast<uint32_t>(level));
	static_cast<Text*>(m_hTextScoreboardPlayerLevel[static_cast<size_t>(team)][playerIndex].ToPtr())->SetText(buf);

	// 닉네임 표시
	static_cast<Text*>(m_hTextScoreboardPlayerNickname[static_cast<size_t>(team)][playerIndex].ToPtr())->SetText(nickname);

	// 킬/데스 표시
	StringCchPrintfW(buf, _countof(buf), L"%u/%u", kill, death);
	static_cast<Text*>(m_hTextScoreboardPlayerKillDeath[static_cast<size_t>(team)][playerIndex].ToPtr())->SetText(buf);

	// 핑 표시
	StringCchPrintfW(buf, _countof(buf), L"%u", ping);
	static_cast<Text*>(m_hTextScoreboardPlayerPing[static_cast<size_t>(team)][playerIndex].ToPtr())->SetText(buf);
}

void GameUIManager::RemovePlayer(uint32_t accountId)
{
	for (size_t i = 0; i < static_cast<size_t>(GameTeam::Count); ++i)
	{
		for (size_t j = 0; j < m_scoreboardPlayerAccountId[i].size(); ++j)
		{
			if (m_scoreboardPlayerAccountId[i][j] == accountId)
			{
				assert(j < MAX_PLAYERS_PER_TEAM);

				if (j < MAX_PLAYERS_PER_TEAM)
				{
					// 한 칸씩 당기기
					for (size_t k = j + 1; k < m_scoreboardPlayerAccountId[i].size(); ++k)
					{
						static_cast<Text*>(m_hTextScoreboardPlayerLevel[i][k - 1].ToPtr())->GetText() =
							static_cast<Text*>(m_hTextScoreboardPlayerLevel[i][k].ToPtr())->GetText();

						static_cast<Text*>(m_hTextScoreboardPlayerNickname[i][k - 1].ToPtr())->GetText() =
							static_cast<Text*>(m_hTextScoreboardPlayerNickname[i][k].ToPtr())->GetText();

						static_cast<Text*>(m_hTextScoreboardPlayerKillDeath[i][k - 1].ToPtr())->GetText() =
							static_cast<Text*>(m_hTextScoreboardPlayerKillDeath[i][k].ToPtr())->GetText();

						static_cast<Text*>(m_hTextScoreboardPlayerPing[i][k - 1].ToPtr())->GetText() =
							static_cast<Text*>(m_hTextScoreboardPlayerPing[i][k].ToPtr())->GetText();

					}
					const size_t lastItemIndex = m_scoreboardPlayerAccountId[i].size() - 1;
					static_cast<Text*>(m_hTextScoreboardPlayerLevel[i][lastItemIndex].ToPtr())->GetText().clear();
					static_cast<Text*>(m_hTextScoreboardPlayerNickname[i][lastItemIndex].ToPtr())->GetText().clear();
					static_cast<Text*>(m_hTextScoreboardPlayerKillDeath[i][lastItemIndex].ToPtr())->GetText().clear();
					static_cast<Text*>(m_hTextScoreboardPlayerPing[i][lastItemIndex].ToPtr())->GetText().clear();
				}

				m_scoreboardPlayerAccountId[i].erase(m_scoreboardPlayerAccountId[i].begin() + j);

				return;
			}
		}
	}
}

void GameUIManager::SetPlayerKillDeath(uint32_t accountId, uint32_t kill, uint32_t death)
{
	for (size_t i = 0; i < static_cast<size_t>(GameTeam::Count); ++i)
	{
		for (size_t j = 0; j < m_scoreboardPlayerAccountId[i].size(); ++j)
		{
			if (m_scoreboardPlayerAccountId[i][j] == accountId)
			{
				assert(j < MAX_PLAYERS_PER_TEAM);

				wchar_t buf[32];
				StringCchPrintfW(buf, _countof(buf), L"%u/%u", kill, death);
				static_cast<Text*>(m_hTextScoreboardPlayerKillDeath[i][j].ToPtr())->SetText(buf);
				
				return;
			}
		}
	}
}

void GameUIManager::SetPlayerPing(uint32_t accountId, uint32_t ping)
{
	for (size_t i = 0; i < static_cast<size_t>(GameTeam::Count); ++i)
	{
		for (size_t j = 0; j < m_scoreboardPlayerAccountId[i].size(); ++j)
		{
			if (m_scoreboardPlayerAccountId[i][j] == accountId)
			{
				assert(j < MAX_PLAYERS_PER_TEAM);

				wchar_t buf[32];
				StringCchPrintfW(buf, _countof(buf), L"%u", ping);
				static_cast<Text*>(m_hTextScoreboardPlayerPing[i][j].ToPtr())->SetText(buf);

				return;
			}
		}
	}
}

void GameUIManager::OnPosChangePlayerFoV()
{
	const SliderControl* pSliderControlPlayerFoV = static_cast<SliderControl*>(m_hSliderControlPlayerFoV.ToPtr());
	if (!pSliderControlPlayerFoV)
		return;

	Player* pScriptPlayer = m_hScriptPlayer.ToPtr();
	if (!pScriptPlayer)
		return;

	wprintf(L"Thumb pos: %d\n", pSliderControlPlayerFoV->GetThumbPos());

	pScriptPlayer->SetFoV(pSliderControlPlayerFoV->GetThumbPos());
}

void GameUIManager::OnClickDrawDebugInfo()
{
	const Checkbox* pCheckboxDrawDebugInfo = static_cast<Checkbox*>(m_hCheckboxDrawDebugInfo.ToPtr());
	if (!pCheckboxDrawDebugInfo)
		return;

	Physics::GetInstance()->SetDrawDebugInfo(pCheckboxDrawDebugInfo->GetCheck());
}
