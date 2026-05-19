#include "GameUIManager.h"
#include "Player.h"

using namespace ze;

UIStateNone UIStateNone::s_instance;
UIStateScoreboard UIStateScoreboard::s_instance;
UIStateGameMenu UIStateGameMenu::s_instance;

void UIStateNone::Enter(GameUIManager* pGameUIManager)
{
}

void UIStateNone::Update(GameUIManager* pGameUIManager)
{
	if (Input::GetInstance()->GetKey(Keycode::KEY_TAB))
	{
		pGameUIManager->SetState(UIStateScoreboard::GetState());
		return;
	}

	if (Input::GetInstance()->GetKeyDown(Keycode::KEY_ESCAPE))
	{
		pGameUIManager->SetState(UIStateGameMenu::GetState());
		return;
	}
}

void UIStateNone::Exit(GameUIManager* pGameUIManager)
{
}

void UIStateScoreboard::Enter(GameUIManager* pGameUIManager)
{
	IUIObject* pPanelScoreboardRoot = pGameUIManager->m_hPanelScoreboardRoot.ToPtr();
	assert(pPanelScoreboardRoot);

	pPanelScoreboardRoot->SetActive(true);
}

void UIStateScoreboard::Update(GameUIManager* pGameUIManager)
{
	if (!Input::GetInstance()->GetKey(Keycode::KEY_TAB))
	{
		pGameUIManager->SetState(UIStateNone::GetState());
		return;
	}
}

void UIStateScoreboard::Exit(GameUIManager* pGameUIManager)
{
	IUIObject* pPanelScoreboardRoot = pGameUIManager->m_hPanelScoreboardRoot.ToPtr();
	assert(pPanelScoreboardRoot);

	pPanelScoreboardRoot->SetActive(false);
}

void UIStateGameMenu::Enter(GameUIManager* pGameUIManager)
{
	Player* pScriptPlayer = pGameUIManager->m_hScriptPlayer.ToPtr();
	assert(pScriptPlayer);
	pScriptPlayer->SetProcessingInput(false);

	IUIObject* pPanelMenuRoot = pGameUIManager->m_hPanelGameMenuRoot.ToPtr();
	assert(pPanelMenuRoot);
	pPanelMenuRoot->SetActive(true);

	Cursor::SetVisible(true);
	Cursor::SetLockState(CursorLockMode::None);
}

void UIStateGameMenu::Update(GameUIManager* pGameUIManager)
{
}

void UIStateGameMenu::Exit(GameUIManager* pGameUIManager)
{
	IUIObject* pPanelMenuRoot = pGameUIManager->m_hPanelGameMenuRoot.ToPtr();
	assert(pPanelMenuRoot);
	pPanelMenuRoot->SetActive(false);

	Player* pScriptPlayer = pGameUIManager->m_hScriptPlayer.ToPtr();
	assert(pScriptPlayer);
	pScriptPlayer->SetProcessingInput(true);

	Cursor::SetVisible(false);
	Cursor::SetLockState(CursorLockMode::Locked);
}

GameUIManager::GameUIManager(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_pUIState(UIStateNone::GetState())
	, m_redTeamPlayersCount(0)
	, m_blueTeamPlayersCount(0)
{
	ZeroMemory(&m_scoreboardRedTeamPlayersNetId, sizeof(m_scoreboardRedTeamPlayersNetId));
	ZeroMemory(&m_scoreboardBlueTeamPlayersNetId, sizeof(m_scoreboardBlueTeamPlayersNetId));
}

void GameUIManager::Awake()
{
	Cursor::SetVisible(false);
	Cursor::SetLockState(CursorLockMode::Locked);



	// ## 점수판 UI 생성
	constexpr XMFLOAT2 SCOREBOARD_SIZE(960, 540);
	UIObjectHandle hPanelScoreboardRoot = Runtime::GetInstance()->CreatePanel();
	m_hPanelScoreboardRoot = hPanelScoreboardRoot;		// 핸들 저장
	Panel* pPanelScoreboardRoot = static_cast<Panel*>(hPanelScoreboardRoot.ToPtr());
	pPanelScoreboardRoot->SetActive(false);
	pPanelScoreboardRoot->SetSize(SCOREBOARD_SIZE);
	pPanelScoreboardRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelScoreboardRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelScoreboardRoot->SetColor(Colors::Black);
	pPanelScoreboardRoot->SetColorA(0.4f);
	pPanelScoreboardRoot->SetShape(PanelShape::RoundedRectangle);

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



	
	// ## 게임 메뉴 UI 생성
	UIObjectHandle hPanelGameMenuRoot = Runtime::GetInstance()->CreatePanel();
	m_hPanelGameMenuRoot = hPanelGameMenuRoot;
	Panel* pPanelGameMenuRoot = static_cast<Panel*>(hPanelGameMenuRoot.ToPtr());
	pPanelGameMenuRoot->SetActive(false);
	pPanelGameMenuRoot->SetSize(140, 200);
	pPanelGameMenuRoot->SetShape(PanelShape::Rectangle);
	pPanelGameMenuRoot->SetColor(Colors::DimGray);

	UIObjectHandle hButtonCloseGameMenu = Runtime::GetInstance()->CreateButton();
	Button* pButtonCloseGameMenu = static_cast<Button*>(hButtonCloseGameMenu.ToPtr());
	pButtonCloseGameMenu->m_transform.SetParent(&pPanelGameMenuRoot->m_transform);
	pButtonCloseGameMenu->m_transform.SetPosition(55, 85);
	pButtonCloseGameMenu->SetSize(20, 20);
	pButtonCloseGameMenu->SetText(L"X");
	pButtonCloseGameMenu->SetButtonColor(ColorsLinear::Red);
	pButtonCloseGameMenu->SetHandlerOnClick(MakeUIHandler(ComponentHandle<GameUIManager>(this->ToHandle()), &GameUIManager::OnClickCloseGameMenu));

	UIObjectHandle hButtonGameSettings = Runtime::GetInstance()->CreateButton();
	Button* pButtonGameSettings = static_cast<Button*>(hButtonGameSettings.ToPtr());
	pButtonGameSettings->m_transform.SetParent(&pPanelGameMenuRoot->m_transform);
	pButtonGameSettings->m_transform.SetPosition(0, +40);
	pButtonGameSettings->SetSize(80, 20);
	pButtonGameSettings->SetText(L"옵션");
	// pButtonGameSettings->SetHandlerOnClick();

	UIObjectHandle hButtonExitGame = Runtime::GetInstance()->CreateButton();
	Button* pButtonExitGame = static_cast<Button*>(hButtonExitGame.ToPtr());
	pButtonExitGame->m_transform.SetParent(&pPanelGameMenuRoot->m_transform);
	pButtonExitGame->m_transform.SetPosition(0, -60);
	pButtonExitGame->SetSize(80, 20);
	pButtonExitGame->SetText(L"나가기");
	// pButtonExitGame->SetHandlerOnClick();

}

void GameUIManager::Update()
{
	if (m_pUIState)
		m_pUIState->Update(this);
}

void GameUIManager::LateUpdate()
{
	// Update 루틴에서 카메라 최종 위치 결정된 상태로 가정
	// -> 카메라 행렬을 이용해서 캐릭터 위 닉네임에 대한 스크린 좌표 위치 획득 및 Text UI 이동
	// 코드 구현...


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

void GameUIManager::SetState(IUIState* pUIState)
{
	IUIState* pOldState = m_pUIState;

	if (pOldState)
		pOldState->Exit(this);

	m_pUIState = pUIState;

	if (m_pUIState)
		m_pUIState->Enter(this);
}

void GameUIManager::ShowAliveUI()
{
	m_hImageCrosshair.ToPtr()->SetActive(true);
	m_hImageHealthBackground.ToPtr()->SetActive(true);
	m_hImageRBUIBackground.ToPtr()->SetActive(true);
}

void GameUIManager::HideAliveUI()
{
	m_hImageCrosshair.ToPtr()->SetActive(false);
	m_hImageHealthBackground.ToPtr()->SetActive(false);
	m_hImageRBUIBackground.ToPtr()->SetActive(false);
}

void GameUIManager::OnClickCloseGameMenu()
{
	assert(GetState() == UIStateGameMenu::GetState());

	SetState(UIStateNone::GetState());
}
