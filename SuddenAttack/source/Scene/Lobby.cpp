#include "Lobby.h"
#include "../Script/GameResources.h"
#include "../Script/LobbyHandler.h"
#include "../Script/Network.h"
#include "../Script/Account.h"
#include "Constants.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(Lobby);

static bool g_singleton = false;
const wchar_t* GO_GAME_RESOURCES_NAME = L"%GameResources";
const wchar_t* GO_LOBBY_HANDLER_NAME = L"%LobbyHandler";
const wchar_t* GO_ACCOUNT_NAME = L"%Account";
const wchar_t* GO_NETWORK_NAME = L"%Network";

const wchar_t* BTN_TEXT_CREATE_ACCOUNT = L"계정 생성";
const wchar_t* BTN_TEXT_LOGIN = L"로그인";
const wchar_t* BTN_TEXT_EXIT = L"나가기";

constexpr uint32_t STATIC_TEXT_SIZE = 16;
constexpr uint32_t CHAT_MSG_TEXT_SIZE = 12;
constexpr uint32_t LOGIN_BUTTON_TEXT_SIZE = 12;
constexpr uint32_t GAME_LIST_TEXT_SIZE = 18;

void Lobby::OnLoadScene()
{
	ComponentHandle<LobbyHandler> hScriptLobbyHandler;

	if (!g_singleton)
	{
		// 1. 게임 리소스 매니저 게임오브젝트 생성
		GameObjectHandle hGameObjectGameResources = CreateGameObject(GO_GAME_RESOURCES_NAME);
		GameObject* pGameObjectGameResources = hGameObjectGameResources.ToPtr();
		pGameObjectGameResources->DontDestroyOnLoad();	// DontDestroyOnLoad
		ComponentHandle<GameResources> hScriptGameResources = pGameObjectGameResources->AddComponent<GameResources>();
		GameResources* pScriptGameResources = hScriptGameResources.ToPtr();
		pScriptGameResources->m_texLoginBgr = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\login_bgr.png");
		pScriptGameResources->m_texGameListBgr = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\gamelist_bgr.png");

		// 2. 로비 핸들러 게임오브젝트 생성
		GameObjectHandle hGameObjectLobbyHandler = CreateGameObject(GO_LOBBY_HANDLER_NAME);
		GameObject* pGameObjectLobbyHandler = hGameObjectLobbyHandler.ToPtr();
		pGameObjectLobbyHandler->DontDestroyOnLoad();	// DontDestroyOnLoad
		hScriptLobbyHandler = pGameObjectLobbyHandler->AddComponent<LobbyHandler>();
		LobbyHandler* pScriptLobbyHandler = hScriptLobbyHandler.ToPtr();
		pScriptLobbyHandler->m_hScriptGameResources = hScriptGameResources;	// 핸들 저장

		// 3. 계정 정보 게임오브젝트 생성
		GameObjectHandle hGameObjectAccount = CreateGameObject(GO_ACCOUNT_NAME);
		GameObject* pGameObjectAccount = hGameObjectAccount.ToPtr();
		pGameObjectAccount->DontDestroyOnLoad();	// DontDestroyOnLoad
		ComponentHandle<Account> hScriptAccount = pGameObjectAccount->AddComponent<Account>();

		// 4. 네트워크 게임오브젝트 생성
		GameObjectHandle hGameObjectNetwork = CreateGameObject(GO_NETWORK_NAME);
		GameObject* pGameObjectNetwork = hGameObjectNetwork.ToPtr();
		pGameObjectNetwork->DontDestroyOnLoad();	// DontDestroyOnLoad
		ComponentHandle<Network> hScriptNetwork = pGameObjectNetwork->AddComponent<Network>();
		pScriptLobbyHandler->m_hScriptNetwork = hScriptNetwork;	// 핸들 저장
		Network* pScriptNetwork = hScriptNetwork.ToPtr();
		pScriptNetwork->m_hScriptAccount = hScriptAccount;
		pScriptNetwork->m_hScriptLobbyHandler = hScriptLobbyHandler;

		g_singleton = true;
	}
	else
	{
		// 1. 게임 리소스 스크립트 핸들 획득
		// GameObjectHandle hGameObjectGameResources = GameObject::Find(GO_GAME_RESOURCES_NAME);
		// GameObject* pGameObjectGameResources = hGameObjectGameResources.ToPtr();
		// assert(pGameObjectGameResources);
		// hScriptGameResources = pGameObjectGameResources->GetComponent<GameResources>();

		// 2. 로비 핸들러 스크립트 핸들 획득
		GameObjectHandle hGameObjectLobbyHandler = GameObject::Find(GO_LOBBY_HANDLER_NAME);
		GameObject* pGameObjectLobbyHandler = hGameObjectLobbyHandler.ToPtr();
		assert(pGameObjectLobbyHandler);
		hScriptLobbyHandler = pGameObjectLobbyHandler->GetComponent<LobbyHandler>();

		// 3. 계정 정보 스크립트 핸들 획득
		// GameObjectHandle hGameObjectAccount = GameObject::Find(GO_ACCOUNT_NAME);
		// GameObject* pGameObjectAccount = hGameObjectAccount.ToPtr();
		// assert(pGameObjectAccount);
		// hScriptAccount = pGameObjectAccount->GetComponent<Account>();

		// 4. 네트워크 스크립트 핸들 획득
		// GameObjectHandle hGameObjectNetwork = GameObject::Find(GO_NETWORK_NAME);
		// GameObject* pGameObjectNetwork = hGameObjectNetwork.ToPtr();
		// assert(pGameObjectNetwork);
		// hScriptNetwork = pGameObjectNetwork->GetComponent<Network>();
	}

	LobbyHandler* pScriptLobbyHandler = hScriptLobbyHandler.ToPtr();

	UIObjectHandle hImageLobbyBgr = CreateImage();
	pScriptLobbyHandler->m_hImageLobbyBgr = hImageLobbyBgr;
	Image* pImageLobbyBgr = static_cast<Image*>(hImageLobbyBgr.ToPtr());
	pImageLobbyBgr->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pImageLobbyBgr->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pImageLobbyBgr->SetNativeSize(true);


	UIObjectHandle hPanelLoginUIRoot = CreatePanel();
	pScriptLobbyHandler->m_hPanelLoginUIRoot = hPanelLoginUIRoot;
	Panel* pPanelLoginWindowRoot = static_cast<Panel*>(hPanelLoginUIRoot.ToPtr());
	pPanelLoginWindowRoot->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pPanelLoginWindowRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelLoginWindowRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelLoginWindowRoot->m_transform.SetPosition(0, 0);
	pPanelLoginWindowRoot->SetSize(360, 160);
	pPanelLoginWindowRoot->SetColor(ColorsLinear::DimGray);
	pPanelLoginWindowRoot->SetShape(PanelShape::RoundedRectangle);
	pPanelLoginWindowRoot->SetRadius(4.0f, 4.0f);


	UIObjectHandle hPanelIDPWFrame = CreatePanel();
	Panel* pPanelIdPwFrame = static_cast<Panel*>(hPanelIDPWFrame.ToPtr());
	pPanelIdPwFrame->m_transform.SetParent(&pPanelLoginWindowRoot->m_transform);
	pPanelIdPwFrame->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelIdPwFrame->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelIdPwFrame->m_transform.SetPosition(0, 20);
	pPanelIdPwFrame->SetSize(pPanelLoginWindowRoot->GetSizeX() - 12, pPanelLoginWindowRoot->GetSizeY() - 50);
	pPanelIdPwFrame->SetColor(0.25f, 0.25f, 0.25f, 1.0f);
	pPanelIdPwFrame->SetShape(PanelShape::RoundedRectangle);
	pPanelIdPwFrame->SetRadius(4.0f, 4.0f);


	UIObjectHandle hTextId = CreateText();
	Text* pTextId = static_cast<Text*>(hTextId.ToPtr());
	pTextId->m_transform.SetParent(&pPanelIdPwFrame->m_transform);
	pTextId->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextId->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextId->m_transform.SetPosition(-110, +46);
	pTextId->SetText(L"ID");
	pTextId->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextId->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextId->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextId->GetTextFormat().SetSize(STATIC_TEXT_SIZE);
	pTextId->ApplyTextFormat();
	pTextId->SetSize(40, 20);


	UIObjectHandle hTextPw = CreateText();
	Text* pTextPw = static_cast<Text*>(hTextPw.ToPtr());
	pTextPw->m_transform.SetParent(&pPanelIdPwFrame->m_transform);
	pTextPw->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextPw->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextPw->m_transform.SetPosition(-110, +0);
	pTextPw->SetText(L"PASSWORD");
	pTextPw->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextPw->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextPw->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextPw->GetTextFormat().SetSize(STATIC_TEXT_SIZE);
	pTextPw->ApplyTextFormat();
	pTextPw->SetSize(100, 20);

	constexpr XMFLOAT2 IDPW_INPUT_FIELD_SIZE(210, 24);
	UIObjectHandle hInputFieldId = CreateInputField();
	pScriptLobbyHandler->m_hInputFieldId = hInputFieldId;
	InputField* pInputFieldId = static_cast<InputField*>(hInputFieldId.ToPtr());
	pInputFieldId->m_transform.SetParent(&pPanelIdPwFrame->m_transform);
	pInputFieldId->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pInputFieldId->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pInputFieldId->m_transform.SetPosition(+55, pTextId->m_transform.GetPositionY());
	pInputFieldId->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pInputFieldId->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pInputFieldId->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pInputFieldId->GetTextFormat().SetSize(CHAT_MSG_TEXT_SIZE);
	pInputFieldId->ApplyTextFormat();
	pInputFieldId->SetSize(IDPW_INPUT_FIELD_SIZE);
	pInputFieldId->SetBkColor(ColorsLinear::DimGray);
	pInputFieldId->SetTextColor(Colors::Black);
	pInputFieldId->AllowReturn(false);
	pInputFieldId->AllowSpace(false);
	pInputFieldId->SetMaxChar(16);

	UIObjectHandle hInputFieldPw = CreateInputField();
	pScriptLobbyHandler->m_hInputFieldPw = hInputFieldPw;
	InputField* pInputFieldPw = static_cast<InputField*>(hInputFieldPw.ToPtr());
	pInputFieldPw->m_transform.SetParent(&pPanelIdPwFrame->m_transform);
	pInputFieldPw->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pInputFieldPw->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pInputFieldPw->m_transform.SetPosition(+55, pTextPw->m_transform.GetPositionY());
	pInputFieldPw->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pInputFieldPw->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pInputFieldPw->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pInputFieldPw->GetTextFormat().SetSize(CHAT_MSG_TEXT_SIZE);
	pInputFieldPw->ApplyTextFormat();
	pInputFieldPw->SetSize(IDPW_INPUT_FIELD_SIZE);
	pInputFieldPw->SetBkColor(ColorsLinear::DimGray);
	pInputFieldPw->SetTextColor(Colors::Black);
	pInputFieldPw->AllowReturn(false);
	pInputFieldPw->AllowSpace(false);
	pInputFieldPw->SetPassword(true);
	pInputFieldPw->SetMaxChar(16);

	UIObjectHandle hTextIdPwInputFieldHelpMsg = CreateText();
	pScriptLobbyHandler->m_hTextIdPwInputFieldHelpMsg = hTextIdPwInputFieldHelpMsg;
	Text* pTextIdPwInputFieldHelpMsg = static_cast<Text*>(hTextIdPwInputFieldHelpMsg.ToPtr());
	pTextIdPwInputFieldHelpMsg->SetActive(false);
	pTextIdPwInputFieldHelpMsg->m_transform.SetParent(&pPanelIdPwFrame->m_transform);
	pTextIdPwInputFieldHelpMsg->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextIdPwInputFieldHelpMsg->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextIdPwInputFieldHelpMsg->m_transform.SetPosition(pInputFieldPw->m_transform.GetPositionX() - 24, pInputFieldPw->m_transform.GetPositionY() - 24);
	pTextIdPwInputFieldHelpMsg->SetText(L"아이디 또는 비밀번호가 올바른 형식이 아닙니다.");
	pTextIdPwInputFieldHelpMsg->SetColor(Colors::Red);
	pTextIdPwInputFieldHelpMsg->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextIdPwInputFieldHelpMsg->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextIdPwInputFieldHelpMsg->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pTextIdPwInputFieldHelpMsg->GetTextFormat().SetSize(CHAT_MSG_TEXT_SIZE);
	pTextIdPwInputFieldHelpMsg->ApplyTextFormat();
	pTextIdPwInputFieldHelpMsg->SetSize(300, 20);



	constexpr XMFLOAT2 LOGIN_BUTTON_SIZE = XMFLOAT2(80, 24);
	UIObjectHandle hButtonCreateAccount = CreateButton();
	Button* pButtonCreateAccount = static_cast<Button*>(hButtonCreateAccount.ToPtr());
	pButtonCreateAccount->m_transform.SetParent(&pPanelLoginWindowRoot->m_transform);
	pButtonCreateAccount->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonCreateAccount->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonCreateAccount->m_transform.SetPosition(-120, -56);
	pButtonCreateAccount->SetSize(LOGIN_BUTTON_SIZE);
	pButtonCreateAccount->SetButtonColor(ColorsLinear::Orange);
	pButtonCreateAccount->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonCreateAccount->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonCreateAccount->SetText(BTN_TEXT_CREATE_ACCOUNT);
	pButtonCreateAccount->SetTextColor(ColorsLinear::Black);
	pButtonCreateAccount->GetTextFormat().SetSize(LOGIN_BUTTON_TEXT_SIZE);
	pButtonCreateAccount->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonCreateAccount->ApplyTextFormat();


	UIObjectHandle hButtonLogin = CreateButton();
	Button* pButtonLogin = static_cast<Button*>(hButtonLogin.ToPtr());
	pButtonLogin->m_transform.SetParent(&pPanelLoginWindowRoot->m_transform);
	pButtonLogin->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonLogin->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonLogin->m_transform.SetPosition(30, -56);
	pButtonLogin->SetSize(LOGIN_BUTTON_SIZE);
	// pButtonLogin->SetButtonColor(Colors::Gray);
	pButtonLogin->SetTextColor(Colors::Gold);
	pButtonLogin->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonLogin->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonLogin->SetText(BTN_TEXT_LOGIN);
	pButtonLogin->GetTextFormat().SetSize(LOGIN_BUTTON_TEXT_SIZE);
	pButtonLogin->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonLogin->ApplyTextFormat();
	pButtonLogin->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickLogin));


	UIObjectHandle hButtonExit = CreateButton();
	Button* pButtonExit = static_cast<Button*>(hButtonExit.ToPtr());
	pButtonExit->m_transform.SetParent(&pPanelLoginWindowRoot->m_transform);
	pButtonExit->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonExit->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonExit->m_transform.SetPosition(120, -56);
	pButtonExit->SetSize(LOGIN_BUTTON_SIZE);
	pButtonExit->SetButtonColor(Colors::DodgerBlue);
	pButtonExit->SetTextColor(Colors::Black);
	pButtonExit->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonExit->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonExit->SetText(BTN_TEXT_EXIT);
	pButtonExit->GetTextFormat().SetSize(LOGIN_BUTTON_TEXT_SIZE);
	pButtonExit->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonExit->ApplyTextFormat();
	pButtonExit->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickExitGame));


	// ############################
	// 방목록 브라우저 UI
	constexpr XMFLOAT2 GAME_BROWSER_PANEL_SIZE(1280, 700);
	constexpr XMFLOAT2 GAME_BROWSER_PANEL_OFFSET(0, +40);
	UIObjectHandle hPanelGameBrowserRoot = CreatePanel();
	pScriptLobbyHandler->m_hPanelGameBrowserRoot = hPanelGameBrowserRoot;
	Panel* pPanelGameBrowserRoot = static_cast<Panel*>(hPanelGameBrowserRoot.ToPtr());
	pPanelGameBrowserRoot->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pPanelGameBrowserRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelGameBrowserRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelGameBrowserRoot->m_transform.SetPosition(GAME_BROWSER_PANEL_OFFSET);
	pPanelGameBrowserRoot->SetSize(GAME_BROWSER_PANEL_SIZE);
	pPanelGameBrowserRoot->SetColor(XMVectorSetW(Colors::DimGray, 0.6f));
	pPanelGameBrowserRoot->SetShape(PanelShape::RoundedRectangle);
	// pPanelGameBrowserRoot->SetRadius(4.0f, 4.0f);

	constexpr XMFLOAT2 GAME_BROWSER_HEAD_TEXT_OFFSET(-GAME_BROWSER_PANEL_SIZE.x / 2 + 80 + GAME_BROWSER_PANEL_OFFSET.x, GAME_BROWSER_PANEL_SIZE.y / 2 - 40 + GAME_BROWSER_PANEL_OFFSET.y);
	UIObjectHandle hTextGameBrowserHead = CreateText();
	Text* pTextGameBrowserHead = static_cast<Text*>(hTextGameBrowserHead.ToPtr());
	pTextGameBrowserHead->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pTextGameBrowserHead->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameBrowserHead->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameBrowserHead->m_transform.SetPosition(GAME_BROWSER_HEAD_TEXT_OFFSET);
	pTextGameBrowserHead->SetText(L"방목록");
	pTextGameBrowserHead->SetColor(Colors::Goldenrod);
	pTextGameBrowserHead->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameBrowserHead->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameBrowserHead->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_HEAVY);
	pTextGameBrowserHead->GetTextFormat().SetSize(22);
	pTextGameBrowserHead->ApplyTextFormat();
	pTextGameBrowserHead->SetSize(100, 30);

	constexpr XMFLOAT2 GAME_LIST_BROWSER_TAB_SIZE(1240, 30);
	constexpr XMFLOAT2 GAME_LIST_BROWSER_TAB_OFFSET(0, +300);
	UIObjectHandle hPanelGameListBrowserTab = CreatePanel();
	Panel* pPanelGameListBrowserTab = static_cast<Panel*>(hPanelGameListBrowserTab.ToPtr());
	pPanelGameListBrowserTab->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pPanelGameListBrowserTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelGameListBrowserTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelGameListBrowserTab->m_transform.SetPosition(GAME_LIST_BROWSER_TAB_OFFSET);
	pPanelGameListBrowserTab->SetSize(GAME_LIST_BROWSER_TAB_SIZE);
	pPanelGameListBrowserTab->SetColor(XMVectorSetW(ColorsLinear::DarkOliveGreen, 0.4f));
	pPanelGameListBrowserTab->SetShape(PanelShape::Rectangle);

	UIObjectHandle hPanelGameSelectedIndicator = CreatePanel();
	pScriptLobbyHandler->m_hPanelGameSelectedIndicator = hPanelGameSelectedIndicator;
	Panel* pPanelGameSelectedIndicator = static_cast<Panel*>(hPanelGameSelectedIndicator.ToPtr());
	pPanelGameSelectedIndicator->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pPanelGameSelectedIndicator->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelGameSelectedIndicator->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelGameSelectedIndicator->m_transform.SetPosition(0, 0);
	pPanelGameSelectedIndicator->SetSize(GAME_LIST_BROWSER_TAB_SIZE);
	pPanelGameSelectedIndicator->SetColor(XMVectorSetW(ColorsLinear::Black, 0.25f));
	pPanelGameSelectedIndicator->SetShape(PanelShape::Rectangle);

	constexpr XMFLOAT2 GAME_NO_TAB_OFFSET(-500, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_NO_TAB_SIZE(+40, +20);
	UIObjectHandle hTextGameNoTab = CreateText();
	Text* pTextGameNoTab = static_cast<Text*>(hTextGameNoTab.ToPtr());
	pTextGameNoTab->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pTextGameNoTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameNoTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameNoTab->m_transform.SetPosition(GAME_NO_TAB_OFFSET);
	pTextGameNoTab->SetText(L"No");
	pTextGameNoTab->SetColor(Colors::Goldenrod);
	pTextGameNoTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameNoTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameNoTab->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_HEAVY);
	pTextGameNoTab->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
	pTextGameNoTab->ApplyTextFormat();
	pTextGameNoTab->SetSize(GAME_NO_TAB_SIZE);

	constexpr XMFLOAT2 GAME_NAME_TAB_OFFSET(-220, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_NAME_TAB_SIZE(+420, +20);
	UIObjectHandle hTextGameNameTab = CreateText();
	Text* pTextGameNameTab = static_cast<Text*>(hTextGameNameTab.ToPtr());
	pTextGameNameTab->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pTextGameNameTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameNameTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameNameTab->m_transform.SetPosition(GAME_NAME_TAB_OFFSET);
	pTextGameNameTab->SetText(L"방제목");
	pTextGameNameTab->SetColor(Colors::Goldenrod);
	pTextGameNameTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameNameTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameNameTab->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_HEAVY);
	pTextGameNameTab->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
	pTextGameNameTab->ApplyTextFormat();
	pTextGameNameTab->SetSize(GAME_NAME_TAB_SIZE);

	constexpr XMFLOAT2 GAME_MAP_TAB_OFFSET(+100, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_MAP_TAB_SIZE(+120, +20);
	UIObjectHandle hTextGameMapTab = CreateText();
	Text* pTextGameMapTab = static_cast<Text*>(hTextGameMapTab.ToPtr());
	pTextGameMapTab->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pTextGameMapTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameMapTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameMapTab->m_transform.SetPosition(GAME_MAP_TAB_OFFSET);
	pTextGameMapTab->SetText(L"맵");
	pTextGameMapTab->SetColor(Colors::Goldenrod);
	pTextGameMapTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameMapTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameMapTab->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_HEAVY);
	pTextGameMapTab->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
	pTextGameMapTab->ApplyTextFormat();
	pTextGameMapTab->SetSize(GAME_MAP_TAB_SIZE);

	constexpr XMFLOAT2 GAME_HEADCOUNT_TAB_OFFSET(+240, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_HEADCOUNT_TAB_SIZE(+100, +20);
	UIObjectHandle hTextGameHeadcountTab = CreateText();
	Text* pTextGameHeadcountTab = static_cast<Text*>(hTextGameHeadcountTab.ToPtr());
	pTextGameHeadcountTab->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pTextGameHeadcountTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameHeadcountTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameHeadcountTab->m_transform.SetPosition(GAME_HEADCOUNT_TAB_OFFSET);
	pTextGameHeadcountTab->SetText(L"인원수");
	pTextGameHeadcountTab->SetColor(Colors::Goldenrod);
	pTextGameHeadcountTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameHeadcountTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameHeadcountTab->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_HEAVY);
	pTextGameHeadcountTab->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
	pTextGameHeadcountTab->ApplyTextFormat();
	pTextGameHeadcountTab->SetSize(GAME_HEADCOUNT_TAB_SIZE);

	constexpr XMFLOAT2 GAME_MODE_TAB_POS(+340, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_MODE_TAB_SIZE(+120, +20);
	UIObjectHandle hTextGameModeTab = CreateText();
	Text* pTextGameModeTab = static_cast<Text*>(hTextGameModeTab.ToPtr());
	pTextGameModeTab->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pTextGameModeTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameModeTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameModeTab->m_transform.SetPosition(GAME_MODE_TAB_POS);
	pTextGameModeTab->SetText(L"게임모드");
	pTextGameModeTab->SetColor(Colors::Goldenrod);
	pTextGameModeTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameModeTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameModeTab->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_HEAVY);
	pTextGameModeTab->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
	pTextGameModeTab->ApplyTextFormat();
	pTextGameModeTab->SetSize(GAME_MODE_TAB_SIZE);

	constexpr XMFLOAT2 GAME_STATE_TAB_OFFSET(+440, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_STATE_TAB_SIZE(+80, +20);
	UIObjectHandle hTextGameStateTab = CreateText();
	Text* pTextGameStateTab = static_cast<Text*>(hTextGameStateTab.ToPtr());
	pTextGameStateTab->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pTextGameStateTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameStateTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameStateTab->m_transform.SetPosition(GAME_STATE_TAB_OFFSET);
	pTextGameStateTab->SetText(L"방상태");
	pTextGameStateTab->SetColor(Colors::Goldenrod);
	pTextGameStateTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameStateTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameStateTab->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_HEAVY);
	pTextGameStateTab->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
	pTextGameStateTab->ApplyTextFormat();
	pTextGameStateTab->SetSize(GAME_STATE_TAB_SIZE);


	constexpr float UI_TAB_ITEM_VERTICAL_DISTANCE = 60;
	constexpr float LIST_ITEM_VERTICAL_DISTANCE = 40;
	for (size_t i = 0; i < MAX_GAME_PER_LIST_PAGE; ++i)
	{
		UIObjectHandle hTextGameNo = CreateText();
		pScriptLobbyHandler->m_hTextGameNo[i] = hTextGameNo;
		Text* pTextGameNo = static_cast<Text*>(hTextGameNo.ToPtr());
		pTextGameNo->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
		pTextGameNo->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameNo->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameNo->m_transform.SetPosition(GAME_NO_TAB_OFFSET.x, GAME_NO_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameNo->SetText(L"123");
		pTextGameNo->SetColor(Colors::WhiteSmoke);
		pTextGameNo->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameNo->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameNo->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
		pTextGameNo->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
		pTextGameNo->ApplyTextFormat();
		pTextGameNo->SetSize(GAME_NO_TAB_SIZE);

		UIObjectHandle hTextGameName = CreateText();
		pScriptLobbyHandler->m_hTextGameName[i] = hTextGameName;
		Text* pTextGameName = static_cast<Text*>(hTextGameName.ToPtr());
		pTextGameName->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
		pTextGameName->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameName->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameName->m_transform.SetPosition(GAME_NAME_TAB_OFFSET.x, GAME_NAME_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameName->SetText(L"Only the best there is!");
		pTextGameName->SetColor(Colors::WhiteSmoke);
		pTextGameName->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameName->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameName->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
		pTextGameName->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
		pTextGameName->ApplyTextFormat();
		pTextGameName->SetSize(GAME_NAME_TAB_SIZE);

		UIObjectHandle hTextGameMap = CreateText();
		pScriptLobbyHandler->m_hTextGameMap[i] = hTextGameMap;
		Text* pTextGameMap = static_cast<Text*>(hTextGameMap.ToPtr());
		pTextGameMap->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
		pTextGameMap->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameMap->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameMap->m_transform.SetPosition(GAME_MAP_TAB_OFFSET.x, GAME_MAP_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameMap->SetText(L"웨어하우스");
		pTextGameMap->SetColor(Colors::WhiteSmoke);
		pTextGameMap->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameMap->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameMap->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
		pTextGameMap->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
		pTextGameMap->ApplyTextFormat();
		pTextGameMap->SetSize(GAME_MAP_TAB_SIZE);

		UIObjectHandle hTextGameHeadcount = CreateText();
		pScriptLobbyHandler->m_hTextGameHeadcount[i] = hTextGameHeadcount;
		Text* pTextGameHeadcount = static_cast<Text*>(hTextGameHeadcount.ToPtr());
		pTextGameHeadcount->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
		pTextGameHeadcount->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameHeadcount->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameHeadcount->m_transform.SetPosition(GAME_HEADCOUNT_TAB_OFFSET.x, GAME_HEADCOUNT_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameHeadcount->SetText(L"3 / 8");
		pTextGameHeadcount->SetColor(Colors::WhiteSmoke);
		pTextGameHeadcount->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameHeadcount->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameHeadcount->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
		pTextGameHeadcount->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
		pTextGameHeadcount->ApplyTextFormat();
		pTextGameHeadcount->SetSize(GAME_HEADCOUNT_TAB_SIZE);

		UIObjectHandle hTextGameMode = CreateText();
		pScriptLobbyHandler->m_hTextGameMode[i] = hTextGameMode;
		Text* pTextGameMode = static_cast<Text*>(hTextGameMode.ToPtr());
		pTextGameMode->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
		pTextGameMode->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameMode->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameMode->m_transform.SetPosition(GAME_MODE_TAB_POS.x, GAME_MODE_TAB_POS.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameMode->SetText(L"팀데스매치");
		pTextGameMode->SetColor(Colors::WhiteSmoke);
		pTextGameMode->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameMode->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameMode->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
		pTextGameMode->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
		pTextGameMode->ApplyTextFormat();
		pTextGameMode->SetSize(GAME_MODE_TAB_SIZE);

		UIObjectHandle hTextGameState = CreateText();
		pScriptLobbyHandler->m_hTextGameState[i] = hTextGameState;
		Text* pTextGameState = static_cast<Text*>(hTextGameState.ToPtr());
		pTextGameState->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
		pTextGameState->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameState->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameState->m_transform.SetPosition(GAME_STATE_TAB_OFFSET.x, GAME_STATE_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameState->SetText(L"대기중");
		pTextGameState->SetColor(Colors::GreenYellow);
		pTextGameState->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameState->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameState->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
		pTextGameState->GetTextFormat().SetSize(GAME_LIST_TEXT_SIZE);
		pTextGameState->ApplyTextFormat();
		pTextGameState->SetSize(GAME_STATE_TAB_SIZE);

		UIObjectHandle hButtonEnterGame = CreateButton();		// 행 항목 전체를 덮는 투명한 버튼
		pScriptLobbyHandler->m_hButtonEnterGame[i] = hButtonEnterGame;
		Button* pButtonEnterGame = static_cast<Button*>(hButtonEnterGame.ToPtr());
		pButtonEnterGame->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
		pButtonEnterGame->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pButtonEnterGame->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pButtonEnterGame->m_transform.SetPosition(0, GAME_STATE_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pButtonEnterGame->SetSize(GAME_LIST_BROWSER_TAB_SIZE);
		pButtonEnterGame->SetButtonColorA(0.0f);
		pButtonEnterGame->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pButtonEnterGame->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pButtonEnterGame->GetTextFormat().SetSize(STATIC_TEXT_SIZE);
		pButtonEnterGame->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
		pButtonEnterGame->ApplyTextFormat();
	}

	constexpr float BROWSER_BOTTOM_BUTTON_COMMON_Y_OFFSET = +30;
	constexpr XMFLOAT2 GAME_LIST_PREV_BUTTON_OFFSET(-40, -GAME_BROWSER_PANEL_SIZE.y / 2 + BROWSER_BOTTOM_BUTTON_COMMON_Y_OFFSET + GAME_BROWSER_PANEL_OFFSET.y);
	UIObjectHandle hButtonGameListPrev = CreateButton();
	Button* pButtonGameListPrev = static_cast<Button*>(hButtonGameListPrev.ToPtr());
	pButtonGameListPrev->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pButtonGameListPrev->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonGameListPrev->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonGameListPrev->m_transform.SetPosition(GAME_LIST_PREV_BUTTON_OFFSET);
	pButtonGameListPrev->SetSize(40, 26);
	pButtonGameListPrev->SetTextColor(Colors::Gold);
	pButtonGameListPrev->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonGameListPrev->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonGameListPrev->SetText(L"◀");
	pButtonGameListPrev->GetTextFormat().SetSize(STATIC_TEXT_SIZE);
	pButtonGameListPrev->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonGameListPrev->ApplyTextFormat();

	constexpr XMFLOAT2 GAME_LIST_NEXT_BUTTON_OFFSET(+40, -GAME_BROWSER_PANEL_SIZE.y / 2 + BROWSER_BOTTOM_BUTTON_COMMON_Y_OFFSET + GAME_BROWSER_PANEL_OFFSET.y);
	UIObjectHandle hButtonGameListNext = CreateButton();
	Button* pButtonGameListNext = static_cast<Button*>(hButtonGameListNext.ToPtr());
	pButtonGameListNext->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pButtonGameListNext->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonGameListNext->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonGameListNext->m_transform.SetPosition(GAME_LIST_NEXT_BUTTON_OFFSET);
	pButtonGameListNext->SetSize(40, 26);
	pButtonGameListNext->SetTextColor(Colors::Gold);
	pButtonGameListNext->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonGameListNext->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonGameListNext->SetText(L"▶");
	pButtonGameListNext->GetTextFormat().SetSize(STATIC_TEXT_SIZE);
	pButtonGameListNext->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonGameListNext->ApplyTextFormat();

	constexpr XMFLOAT2 GAME_LIST_REFRESH_BUTTON_OFFSET(+540, -GAME_BROWSER_PANEL_SIZE.y / 2 + BROWSER_BOTTOM_BUTTON_COMMON_Y_OFFSET + GAME_BROWSER_PANEL_OFFSET.y);
	UIObjectHandle hButtonRefreshGameList = CreateButton();
	Button* pButtonRefreshGameList = static_cast<Button*>(hButtonRefreshGameList.ToPtr());
	pButtonRefreshGameList->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pButtonRefreshGameList->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonRefreshGameList->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonRefreshGameList->m_transform.SetPosition(GAME_LIST_REFRESH_BUTTON_OFFSET);
	pButtonRefreshGameList->SetSize(100, 26);
	pButtonRefreshGameList->SetTextColor(Colors::Gold);
	pButtonRefreshGameList->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonRefreshGameList->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonRefreshGameList->SetText(L"새로고침");
	pButtonRefreshGameList->GetTextFormat().SetSize(STATIC_TEXT_SIZE);
	pButtonRefreshGameList->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonRefreshGameList->ApplyTextFormat();






	// ############################
	// 채팅 UI
	constexpr XMFLOAT2 CHAT_PANEL_SIZE(840, 200);
	constexpr XMFLOAT2 CHAT_PANEL_OFFSET(-GAME_BROWSER_PANEL_SIZE.x / 2 + CHAT_PANEL_SIZE.x / 2, CHAT_PANEL_SIZE.y / 2 + 20);
	UIObjectHandle hPanelChat = CreatePanel();
	pScriptLobbyHandler->m_hPanelChat = hPanelChat;
	Panel* pPanelChat = static_cast<Panel*>(hPanelChat.ToPtr());
	pPanelChat->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pPanelChat->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelChat->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pPanelChat->m_transform.SetPosition(CHAT_PANEL_OFFSET);
	pPanelChat->SetSize(CHAT_PANEL_SIZE);
	pPanelChat->SetColor(XMVectorSetW(Colors::DimGray, 0.6f));
	pPanelChat->SetShape(PanelShape::RoundedRectangle);
	// pPanelGameBrowserRoot->SetRadius(4.0f, 4.0f);

	constexpr XMFLOAT2 SEND_CHAT_MSG_BUTTON_SIZE(80, 22);
	constexpr XMFLOAT2 SEND_CHAT_MSG_BUTTON_OFFSET(CHAT_PANEL_OFFSET.x + CHAT_PANEL_SIZE.x / 2 - SEND_CHAT_MSG_BUTTON_SIZE.x / 2 - 10, CHAT_PANEL_OFFSET.y - CHAT_PANEL_SIZE.y / 2 + SEND_CHAT_MSG_BUTTON_SIZE.y / 2 + 10);
	UIObjectHandle hButtonSendChatMsg = CreateButton();
	Button* pButtonSendChatMsg = static_cast<Button*>(hButtonSendChatMsg.ToPtr());
	pButtonSendChatMsg->m_transform.SetParent(&pPanelChat->m_transform);
	pButtonSendChatMsg->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonSendChatMsg->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pButtonSendChatMsg->m_transform.SetPosition(SEND_CHAT_MSG_BUTTON_OFFSET);
	pButtonSendChatMsg->SetSize(SEND_CHAT_MSG_BUTTON_SIZE);
	pButtonSendChatMsg->SetTextColor(Colors::Gold);
	pButtonSendChatMsg->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonSendChatMsg->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonSendChatMsg->SetText(L"전송");
	pButtonSendChatMsg->GetTextFormat().SetSize(STATIC_TEXT_SIZE);
	pButtonSendChatMsg->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonSendChatMsg->ApplyTextFormat();
	pButtonSendChatMsg->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickSendChatMsg));

	constexpr XMFLOAT2 CHAT_INPUT_FIELD_SIZE(CHAT_PANEL_SIZE.x - SEND_CHAT_MSG_BUTTON_SIZE.x - 30, 22);
	constexpr XMFLOAT2 CHAT_INPUT_FIELD_OFFSET(CHAT_PANEL_OFFSET.x - CHAT_PANEL_SIZE.x / 2 + CHAT_INPUT_FIELD_SIZE.x / 2 + 10, SEND_CHAT_MSG_BUTTON_OFFSET.y);
	UIObjectHandle hInputFieldChatMsg = CreateInputField();
	pScriptLobbyHandler->m_hInputFieldChatMsg = hInputFieldChatMsg;
	InputField* pInputFieldChatInput = static_cast<InputField*>(hInputFieldChatMsg.ToPtr());
	pInputFieldChatInput->m_transform.SetParent(&pPanelChat->m_transform);
	pInputFieldChatInput->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pInputFieldChatInput->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pInputFieldChatInput->m_transform.SetPosition(CHAT_INPUT_FIELD_OFFSET);
	pInputFieldChatInput->SetSize(CHAT_INPUT_FIELD_SIZE);
	pInputFieldChatInput->SetMaxChar(MAX_CHAT_MSG_LEN);
	pInputFieldChatInput->SetTextColor(Colors::Black);
	pInputFieldChatInput->SetBkColor(ColorsLinear::Orange);
	pInputFieldChatInput->GetTextFormat().SetSize(CHAT_MSG_TEXT_SIZE);
	pInputFieldChatInput->ApplyTextFormat();

	constexpr XMFLOAT2 CHAT_MSG_SIZE(CHAT_INPUT_FIELD_SIZE.x + SEND_CHAT_MSG_BUTTON_SIZE.x, CHAT_INPUT_FIELD_SIZE.y);
	constexpr XMFLOAT2 FIRST_CHAT_MSG_OFFSET(CHAT_PANEL_OFFSET.x, CHAT_PANEL_OFFSET.y + CHAT_PANEL_SIZE.y / 2 - CHAT_MSG_SIZE.y / 2 - 10);
	for (size_t i = 0; i < CHAT_MSG_ITEM_ROW_COUNT; ++i)
	{
		UIObjectHandle hTextLobbyChatMsg = CreateText();
		pScriptLobbyHandler->m_hTextLobbyChatMsg[i] = hTextLobbyChatMsg;
		Text* pTextLobbyChatMsg = static_cast<Text*>(hTextLobbyChatMsg.ToPtr());
		pTextLobbyChatMsg->m_transform.SetParent(&pPanelChat->m_transform);
		pTextLobbyChatMsg->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextLobbyChatMsg->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextLobbyChatMsg->m_transform.SetPosition(FIRST_CHAT_MSG_OFFSET.x, FIRST_CHAT_MSG_OFFSET.y - i * CHAT_MSG_SIZE.y);
		pTextLobbyChatMsg->SetSize(CHAT_MSG_SIZE);
		pTextLobbyChatMsg->SetColor(Colors::WhiteSmoke);
		pTextLobbyChatMsg->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pTextLobbyChatMsg->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextLobbyChatMsg->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
		pTextLobbyChatMsg->GetTextFormat().SetSize(CHAT_MSG_TEXT_SIZE);
		pTextLobbyChatMsg->ApplyTextFormat();
	}





	// ########################################
	// 로비 기타 UI

	UIObjectHandle hButtonOpenShop = CreateButton();
	pScriptLobbyHandler->m_hButtonOpenShop = hButtonOpenShop;
	Button* pButtonOpenShop = static_cast<Button*>(hButtonOpenShop.ToPtr());
	pButtonOpenShop->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pButtonOpenShop->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pButtonOpenShop->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pButtonOpenShop->m_transform.SetPosition(+55, -18);
	pButtonOpenShop->SetSize(100, 26);
	pButtonOpenShop->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonOpenShop->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonOpenShop->SetText(L"상점/정비");
	pButtonOpenShop->GetTextFormat().SetSize(STATIC_TEXT_SIZE);
	pButtonOpenShop->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonOpenShop->ApplyTextFormat();


	UIObjectHandle hButtonUserInfo = CreateButton();
	pScriptLobbyHandler->m_hButtonUserInfo = hButtonUserInfo;
	Button* pButtonUserInfo = static_cast<Button*>(hButtonUserInfo.ToPtr());
	pButtonUserInfo->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pButtonUserInfo->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pButtonUserInfo->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pButtonUserInfo->m_transform.SetPosition(+55, -18 - 31);
	pButtonUserInfo->SetSize(100, 26);
	pButtonUserInfo->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonUserInfo->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonUserInfo->SetText(L"내 정보");
	pButtonUserInfo->GetTextFormat().SetSize(STATIC_TEXT_SIZE);
	pButtonUserInfo->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonUserInfo->ApplyTextFormat();


	UIObjectHandle hButtonExitGame = CreateButton();
	Button* pButtonExitGame = static_cast<Button*>(hButtonExitGame.ToPtr());
	pButtonExitGame->m_transform.SetParent(&pPanelGameBrowserRoot->m_transform);
	pButtonExitGame->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pButtonExitGame->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pButtonExitGame->m_transform.SetPosition(+45, +18);
	pButtonExitGame->SetSize(80, 26);
	pButtonExitGame->SetButtonColor(Colors::Red);
	pButtonExitGame->SetTextColor(Colors::White);
	pButtonExitGame->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonExitGame->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonExitGame->SetText(L"게임종료");
	pButtonExitGame->GetTextFormat().SetSize(STATIC_TEXT_SIZE);
	pButtonExitGame->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonExitGame->ApplyTextFormat();
	pButtonExitGame->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickExitGame));



}
