#include "GameUIManager.h"
#include "Player.h"
#include "Account.h"
#include "ListenServerClient.h"
#include "..\Resource\LSProtocol.h"

using namespace ze;

const wchar_t* const GAME_UI_TEXT_FONT = L"Agency FB";

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

	pGameUIManager->ShowChatPanel();
	pGameUIManager->ShowGameUI();
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
}

GameUIManager::GameUIManager(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_pUIState(nullptr)
	, m_activeRespawnUI(false)
	, m_respawnRemainingTime(0.0f)
	, m_redTeamPlayersCount(0)
	, m_blueTeamPlayersCount(0)
	, m_chatMsgCount(0)
{
	ZeroMemory(&m_scoreboardRedTeamPlayersNetId, sizeof(m_scoreboardRedTeamPlayersNetId));
	ZeroMemory(&m_scoreboardBlueTeamPlayersNetId, sizeof(m_scoreboardBlueTeamPlayersNetId));
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
	pTextScoreboardTitle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextScoreboardTitle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextScoreboardTitle->GetTextFormat().SetSize(32);
	pTextScoreboardTitle->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextScoreboardTitle->ApplyTextFormat();
	pTextScoreboardTitle->m_transform.SetPosition(0, SCOREBOARD_SIZE.y / 2 - 80);
	pTextScoreboardTitle->SetColor(ColorsLinear::Gold);
	pTextScoreboardTitle->SetText(L"SCOREBOARD");

	constexpr XMFLOAT2 TEAM_PANEL_SIZE(SCOREBOARD_SIZE.x / 2 - 10, 26);
	constexpr XMFLOAT2 RED_TEAM_PANEL_OFFSET(-SCOREBOARD_SIZE.x / 2 + TEAM_PANEL_SIZE.x / 2 + 7, +SCOREBOARD_SIZE.y / 2 - 120);
	UIObjectHandle hPanelRedTeamPanel = Runtime::GetInstance()->CreatePanel();
	Panel* pPanelRedTeamPanel = static_cast<Panel*>(hPanelRedTeamPanel.ToPtr());
	pPanelRedTeamPanel->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
	pPanelRedTeamPanel->m_transform.SetPosition(RED_TEAM_PANEL_OFFSET);
	pPanelRedTeamPanel->SetSize(TEAM_PANEL_SIZE);
	pPanelRedTeamPanel->SetColor(ColorsLinear::Red);
	pPanelRedTeamPanel->SetColorA(0.4f);
	pPanelRedTeamPanel->SetShape(PanelShape::Rectangle);

	constexpr XMFLOAT2 TEAM_PANEL_TEXT_SIZE(TEAM_PANEL_SIZE.x - 10, TEAM_PANEL_SIZE.y);
	UIObjectHandle hTextRedTeamPanel = Runtime::GetInstance()->CreateText();
	Text* pTextRedTeamPanel = static_cast<Text*>(hTextRedTeamPanel.ToPtr());
	pTextRedTeamPanel->m_transform.SetParent(&pPanelRedTeamPanel->m_transform);
	pTextRedTeamPanel->m_transform.SetPosition(RED_TEAM_PANEL_OFFSET);
	pTextRedTeamPanel->SetSize(TEAM_PANEL_TEXT_SIZE);
	pTextRedTeamPanel->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextRedTeamPanel->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextRedTeamPanel->GetTextFormat().SetSize(16);
	pTextRedTeamPanel->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextRedTeamPanel->ApplyTextFormat();
	pTextRedTeamPanel->SetText(L"RED TEAM");

	constexpr XMFLOAT2 BLUE_TEAM_PANEL_OFFSET(+SCOREBOARD_SIZE.x / 2 - TEAM_PANEL_SIZE.x / 2 - 7, RED_TEAM_PANEL_OFFSET.y);
	UIObjectHandle hPanelBlueTeamPanel = Runtime::GetInstance()->CreatePanel();
	Panel* pPanelBlueTeamPanel = static_cast<Panel*>(hPanelBlueTeamPanel.ToPtr());
	pPanelBlueTeamPanel->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
	pPanelBlueTeamPanel->m_transform.SetPosition(BLUE_TEAM_PANEL_OFFSET);
	pPanelBlueTeamPanel->SetSize(TEAM_PANEL_SIZE);
	pPanelBlueTeamPanel->SetColor(ColorsLinear::Blue);
	pPanelBlueTeamPanel->SetColorA(0.4f);
	pPanelBlueTeamPanel->SetShape(PanelShape::Rectangle);

	UIObjectHandle hTextBlueTeamPanel = Runtime::GetInstance()->CreateText();
	Text* pTextBlueTeamPanel = static_cast<Text*>(hTextBlueTeamPanel.ToPtr());
	pTextBlueTeamPanel->m_transform.SetParent(&pPanelBlueTeamPanel->m_transform);
	pTextBlueTeamPanel->m_transform.SetPosition(BLUE_TEAM_PANEL_OFFSET);
	pTextBlueTeamPanel->SetSize(TEAM_PANEL_TEXT_SIZE);
	pTextBlueTeamPanel->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextBlueTeamPanel->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextBlueTeamPanel->GetTextFormat().SetSize(16);
	pTextBlueTeamPanel->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextBlueTeamPanel->ApplyTextFormat();
	pTextBlueTeamPanel->SetText(L"BLUE TEAM");

	constexpr XMFLOAT2 PLAYER_INFO_COLUMNS_SIZE(TEAM_PANEL_SIZE.x - 10, 20);
	constexpr XMFLOAT2 RED_TEAM_PLAYER_INFO_COLUMNS_OFFSET(RED_TEAM_PANEL_OFFSET.x, RED_TEAM_PANEL_OFFSET.y - TEAM_PANEL_SIZE.y / 2 - 3 - PLAYER_INFO_COLUMNS_SIZE.y);
	UIObjectHandle hTextScoreboardRedTeamColumns = Runtime::GetInstance()->CreateText();
	Text* pTextScoreboardRedTeamColumns = static_cast<Text*>(hTextScoreboardRedTeamColumns.ToPtr());
	pTextScoreboardRedTeamColumns->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
	pTextScoreboardRedTeamColumns->m_transform.SetPosition(RED_TEAM_PLAYER_INFO_COLUMNS_OFFSET);
	pTextScoreboardRedTeamColumns->SetSize(PLAYER_INFO_COLUMNS_SIZE);
	pTextScoreboardRedTeamColumns->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextScoreboardRedTeamColumns->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextScoreboardRedTeamColumns->GetTextFormat().SetSize(14);
	pTextScoreboardRedTeamColumns->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextScoreboardRedTeamColumns->ApplyTextFormat();
	pTextScoreboardRedTeamColumns->SetText(L"Lv.\t닉네임\t\t\t\t킬\t데스\t지연시간");

	constexpr FLOAT ROW_PITCHES_PER_ITEM = 20;
	constexpr FLOAT ITEM_ROW_SIZE = 20;
	constexpr FLOAT FIRST_ITEM_POS_Y = RED_TEAM_PLAYER_INFO_COLUMNS_OFFSET.y - PLAYER_INFO_COLUMNS_SIZE.y / 2 - 10 - ITEM_ROW_SIZE / 2;
	constexpr XMFLOAT2 SCOREBOARD_LEVEL_TEXT_SIZE(60, 20);
	for (size_t i = 0; i < MAX_PLAYERS_PER_TEAM; ++i)
	{
		UIObjectHandle hTextScoreboardRedTeamPlayerLevel = Runtime::GetInstance()->CreateText();
		m_hTextScoreboardRedTeamPlayerLevel[i] = hTextScoreboardRedTeamPlayerLevel;
		Text* pTextScoreboardRedTeamPlayerLevel = static_cast<Text*>(hTextScoreboardRedTeamPlayerLevel.ToPtr());
		pTextScoreboardRedTeamPlayerLevel->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
		pTextScoreboardRedTeamPlayerLevel->m_transform.SetPosition(RED_TEAM_PANEL_OFFSET.x - TEAM_PANEL_SIZE.x / 2 + SCOREBOARD_LEVEL_TEXT_SIZE.x / 2, FIRST_ITEM_POS_Y - i * (ITEM_ROW_SIZE + ROW_PITCHES_PER_ITEM));
		pTextScoreboardRedTeamPlayerLevel->SetSize(SCOREBOARD_LEVEL_TEXT_SIZE);
		pTextScoreboardRedTeamPlayerLevel->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pTextScoreboardRedTeamPlayerLevel->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextScoreboardRedTeamPlayerLevel->GetTextFormat().SetSize(14);
		pTextScoreboardRedTeamPlayerLevel->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
		pTextScoreboardRedTeamPlayerLevel->ApplyTextFormat();
		pTextScoreboardRedTeamPlayerLevel->SetText(L"999");
	}

	constexpr XMFLOAT2 BLUE_TEAM_PLAYER_INFO_COLUMNS_OFFSET(RED_TEAM_PANEL_OFFSET.x, RED_TEAM_PANEL_OFFSET.y - TEAM_PANEL_SIZE.y / 2 - 3 - PLAYER_INFO_COLUMNS_SIZE.y);
	UIObjectHandle hTextScoreboardBlueTeamColumns = Runtime::GetInstance()->CreateText();
	Text* pTextScoreboardBlueTeamColumns = static_cast<Text*>(hTextScoreboardBlueTeamColumns.ToPtr());
	pTextScoreboardBlueTeamColumns->m_transform.SetParent(&pPanelScoreboardRoot->m_transform);
	pTextScoreboardBlueTeamColumns->m_transform.SetPosition(BLUE_TEAM_PLAYER_INFO_COLUMNS_OFFSET);
	pTextScoreboardBlueTeamColumns->SetSize(PLAYER_INFO_COLUMNS_SIZE);
	pTextScoreboardBlueTeamColumns->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextScoreboardBlueTeamColumns->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextScoreboardBlueTeamColumns->GetTextFormat().SetSize(14);
	pTextScoreboardBlueTeamColumns->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextScoreboardBlueTeamColumns->ApplyTextFormat();
	pTextScoreboardBlueTeamColumns->SetText(L"Lv.\t닉네임\t\t\t\t킬\t데스\t지연시간");



	
	// ##### 게임 메뉴 UI 생성 #####
	UIObjectHandle hPanelMenuRoot = Runtime::GetInstance()->CreatePanel();
	m_hPanelMenuRoot = hPanelMenuRoot;
	Panel* pPanelMenuRoot = static_cast<Panel*>(hPanelMenuRoot.ToPtr());
	pPanelMenuRoot->SetSize(140, 200);
	pPanelMenuRoot->SetShape(PanelShape::Rectangle);
	pPanelMenuRoot->SetColor(Colors::DimGray);

	UIObjectHandle hButtonCloseGameMenu = Runtime::GetInstance()->CreateButton();
	Button* pButtonCloseGameMenu = static_cast<Button*>(hButtonCloseGameMenu.ToPtr());
	pButtonCloseGameMenu->m_transform.SetParent(&pPanelMenuRoot->m_transform);
	pButtonCloseGameMenu->m_transform.SetPosition(55, 85);
	pButtonCloseGameMenu->SetSize(20, 20);
	pButtonCloseGameMenu->SetText(L"X");
	pButtonCloseGameMenu->SetButtonColor(ColorsLinear::Red);
	pButtonCloseGameMenu->SetHandlerOnClick(MakeUIHandler(ComponentHandle<GameUIManager>(this->ToHandle()), &GameUIManager::OnClickCloseGameMenu));

	UIObjectHandle hButtonGameSettings = Runtime::GetInstance()->CreateButton();
	Button* pButtonGameSettings = static_cast<Button*>(hButtonGameSettings.ToPtr());
	pButtonGameSettings->m_transform.SetParent(&pPanelMenuRoot->m_transform);
	pButtonGameSettings->m_transform.SetPosition(0, +40);
	pButtonGameSettings->SetSize(80, 20);
	pButtonGameSettings->SetText(L"옵션");
	// pButtonGameSettings->SetHandlerOnClick(MakeUIHandler(ComponentHandle<GameUIManager>(this->ToHandle()), &GameUIManager::OnClickCloseGameMenu));

	UIObjectHandle hButtonExitGame = Runtime::GetInstance()->CreateButton();
	Button* pButtonExitGame = static_cast<Button*>(hButtonExitGame.ToPtr());
	pButtonExitGame->m_transform.SetParent(&pPanelMenuRoot->m_transform);
	pButtonExitGame->m_transform.SetPosition(0, -60);
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

	UIObjectHandle hImageHealthBackground = Runtime::GetInstance()->CreateImage();
	m_hImageHealthBackground = hImageHealthBackground;
	Image* pImageHealthBackground = static_cast<Image*>(hImageHealthBackground.ToPtr());
	pImageHealthBackground->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pImageHealthBackground->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\health.png"));
	pImageHealthBackground->SetNativeSize(true);
	pImageHealthBackground->m_transform.SetPosition(pImageHealthBackground->GetHalfSizeX() + 4, pImageHealthBackground->GetHalfSizeY() + 4);
	pImageHealthBackground->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pImageHealthBackground->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);

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

	UIObjectHandle hTextHP = Runtime::GetInstance()->CreateText();
	m_hTextHP = hTextHP;
	Text* pTextHP = static_cast<Text*>(hTextHP.ToPtr());
	pTextHP->m_transform.SetParent(&pImageGameUIRoot->m_transform);
	pTextHP->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pTextHP->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pTextHP->m_transform.SetPosition(pImageHealthBackground->m_transform.GetPosition());
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
	pTextAP->m_transform.SetPosition(pImageHealthBackground->m_transform.GetPosition());
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
	pTextRespawnIndicator->SetSize(400, 100);
	pTextRespawnIndicator->GetTextFormat().SetSize(24);
	pTextRespawnIndicator->ApplyTextFormat();
	pTextRespawnIndicator->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextRespawnIndicator->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextRespawnIndicator->SetColor(Colors::WhiteSmoke);
	pTextRespawnIndicator->SetText(L"R E S P A W N\n초 남았습니다.");







	// 채팅 패널 UI
	constexpr XMFLOAT2 INGAME_CHAT_PANEL_SIZE(500, 220);
	UIObjectHandle hPanelChatRoot = Runtime::GetInstance()->CreatePanel();
	m_hPanelChatRoot = hPanelChatRoot;
	Panel* pPanelChatRoot = static_cast<Panel*>(hPanelChatRoot.ToPtr());
	pPanelChatRoot->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pPanelChatRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pPanelChatRoot->m_transform.SetPosition(INGAME_CHAT_PANEL_SIZE.x / 2 + 10, INGAME_CHAT_PANEL_SIZE.y / 2 + 200);
	pPanelChatRoot->SetSize(INGAME_CHAT_PANEL_SIZE);
	pPanelChatRoot->SetShape(PanelShape::RoundedRectangle);
	pPanelChatRoot->SetColor(Colors::DimGray);
	pPanelChatRoot->SetColorA(0.25f);
	
	const XMFLOAT2 CHAT_MSG_TEXT_SIZE(INGAME_CHAT_PANEL_SIZE.x - 20, 20);
	for (size_t i = 0; i < INGAME_CHAT_MSG_ITEM_ROW_COUNT; ++i)
	{
		m_hTextChatMsg[i] = Runtime::GetInstance()->CreateText();
		Text* pTextChatMsg = static_cast<Text*>(m_hTextChatMsg[i].ToPtr());
		pTextChatMsg->m_transform.SetParent(&pPanelChatRoot->m_transform);
		pTextChatMsg->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextChatMsg->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pTextChatMsg->m_transform.SetPosition(
			pPanelChatRoot->m_transform.GetPositionX(),
			pPanelChatRoot->m_transform.GetPositionY() + INGAME_CHAT_PANEL_SIZE.y / 2 - CHAT_MSG_TEXT_SIZE.y / 2 - 10 - CHAT_MSG_TEXT_SIZE.y * i
		);
		pTextChatMsg->SetSize(CHAT_MSG_TEXT_SIZE);
		// pTextChatMsg->SetText();
	}

	const XMFLOAT2 CHAT_MSG_INPUT_FIELD_SIZE(INGAME_CHAT_PANEL_SIZE.x - 20, 20);
	m_hInputFieldChatMsg = Runtime::GetInstance()->CreateInputField();
	InputField* pInputFieldChatMsg = static_cast<InputField*>(m_hInputFieldChatMsg.ToPtr());
	pInputFieldChatMsg->m_transform.SetParent(&pPanelChatRoot->m_transform);
	pInputFieldChatMsg->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pInputFieldChatMsg->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pInputFieldChatMsg->m_transform.SetPosition(
		pPanelChatRoot->m_transform.GetPositionX(),
		pPanelChatRoot->m_transform.GetPositionY() - INGAME_CHAT_PANEL_SIZE.y / 2 + CHAT_MSG_INPUT_FIELD_SIZE.y / 2 + 10
	);
	pInputFieldChatMsg->SetSize(CHAT_MSG_INPUT_FIELD_SIZE);
	pInputFieldChatMsg->SetMaxChar(MAX_CHAT_MSG_LEN);

	
	// ###########################################################
	m_hPanelAdapterInfoRoot.ToPtr()->DontDestroyOnLoadRecursively();
	m_hPanelScoreboardRoot.ToPtr()->DontDestroyOnLoadRecursively();
	m_hPanelMenuRoot.ToPtr()->DontDestroyOnLoadRecursively();
	m_hImageGameUIRoot.ToPtr()->DontDestroyOnLoadRecursively();
	m_hPanelChatRoot.ToPtr()->DontDestroyOnLoadRecursively();
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
}

void GameUIManager::LateUpdate()
{
	// Update 루틴에서 카메라 최종 위치 결정된 상태로 가정
	// -> 카메라 행렬, 투영 행렬, 뷰포트 변환 및 화면 좌표계에서의 좌표를 구해서 캐릭터 위 닉네임에 대한 스크린 좌표 위치 획득 및 Text UI 이동
	// 코드 구현...


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
	static_cast<Text*>(m_hTextHP.ToPtr())->SetText(buf);
}

void GameUIManager::SetTextAP(uint32_t ap)
{
	wchar_t buf[32];
	StringCchPrintfW(buf, _countof(buf), L"%u", ap);
	static_cast<Text*>(m_hTextAP.ToPtr())->SetText(buf);
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

	if (m_activeRespawnUI)
		m_hTextRespawnIndicator.ToPtr()->SetActive(true);
}

void GameUIManager::HideGameUI()
{
	m_hImageGameUIRoot.ToPtr()->SetActive(false);
	m_hTextRespawnIndicator.ToPtr()->SetActive(false);
}

void GameUIManager::ShowChatPanel()
{
	m_hPanelChatRoot.ToPtr()->SetActive(true);
}

void GameUIManager::HideChatPanel()
{
	m_hPanelChatRoot.ToPtr()->SetActive(false);
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
	if (m_chatMsgCount < _countof(m_hTextChatMsg))
	{
		static_cast<Text*>(m_hTextChatMsg[m_chatMsgCount].ToPtr())->SetText(msg);
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
