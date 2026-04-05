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

constexpr uint32_t STATIC_TEXT_SIZE_MEDIUM = 16;
constexpr uint32_t STATIC_TEXT_SIZE_SMALL = 12;
constexpr uint32_t CHAT_MSG_TEXT_SIZE = 12;
constexpr uint32_t MEDIUM_TEXT_SIZE = 18;
constexpr DWRITE_FONT_WEIGHT MEDIUM_TEXT_WEIGHT = DWRITE_FONT_WEIGHT_BOLD;

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
	pTextId->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
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
	pTextPw->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
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
	pButtonCreateAccount->GetTextFormat().SetSize(STATIC_TEXT_SIZE_SMALL);
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
	pButtonLogin->GetTextFormat().SetSize(STATIC_TEXT_SIZE_SMALL);
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
	pButtonExit->GetTextFormat().SetSize(STATIC_TEXT_SIZE_SMALL);
	pButtonExit->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonExit->ApplyTextFormat();
	pButtonExit->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickExitGame));

	// ############################
	// Modal 메시지 박스
	UIObjectHandle hPanelOkMsgBoxRoot = CreatePanel();
	pScriptLobbyHandler->m_hPanelOkMsgBoxRoot = hPanelOkMsgBoxRoot;
	Panel* pPanelOkMsgBoxRoot = static_cast<Panel*>(hPanelOkMsgBoxRoot.ToPtr());
	// pPanelOkMsgBoxRoot->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pPanelOkMsgBoxRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelOkMsgBoxRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelOkMsgBoxRoot->m_transform.SetPosition(0, 0);
	pPanelOkMsgBoxRoot->SetSize(4096, 4096);
	pPanelOkMsgBoxRoot->SetColor(Colors::Black);
	pPanelOkMsgBoxRoot->SetColorA(0.5f);
	pPanelOkMsgBoxRoot->SetShape(PanelShape::Rectangle);

	constexpr XMFLOAT2 OK_MSGBOX_PANEL_SIZE(400, 200);
	UIObjectHandle hPanelOkMsgBoxWindow = CreatePanel();
	Panel* pPanelOkMsgBoxWindow = static_cast<Panel*>(hPanelOkMsgBoxWindow.ToPtr());
	pPanelOkMsgBoxWindow->m_transform.SetParent(&pPanelOkMsgBoxRoot->m_transform);
	pPanelOkMsgBoxWindow->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelOkMsgBoxWindow->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelOkMsgBoxWindow->m_transform.SetPosition(0, 0);
	pPanelOkMsgBoxWindow->SetSize(OK_MSGBOX_PANEL_SIZE);
	pPanelOkMsgBoxWindow->SetColor(Colors::DimGray);
	pPanelOkMsgBoxWindow->SetShape(PanelShape::RoundedRectangle);
	pPanelOkMsgBoxWindow->SetRadius(8.0f, 8.0f);

	constexpr XMFLOAT2 OK_MSGBOX_HEAD_TEXT_SIZE(OK_MSGBOX_PANEL_SIZE.x - 40, 30);
	constexpr XMFLOAT2 OK_MSGBOX_HEAD_TEXT_OFFSET(0, +OK_MSGBOX_PANEL_SIZE.y / 2 - 5 - OK_MSGBOX_HEAD_TEXT_SIZE.y / 2);
	UIObjectHandle hTextOkMsgBoxHead = CreateText();
	Text* pTextOkMsgBoxHead = static_cast<Text*>(hTextOkMsgBoxHead.ToPtr());
	pTextOkMsgBoxHead->m_transform.SetParent(&pPanelOkMsgBoxWindow->m_transform);
	pTextOkMsgBoxHead->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextOkMsgBoxHead->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextOkMsgBoxHead->m_transform.SetPosition(OK_MSGBOX_HEAD_TEXT_OFFSET);
	pTextOkMsgBoxHead->SetSize(OK_MSGBOX_HEAD_TEXT_SIZE);
	pTextOkMsgBoxHead->SetText(L"알림");
	pTextOkMsgBoxHead->SetColor(Colors::WhiteSmoke);
	pTextOkMsgBoxHead->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextOkMsgBoxHead->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextOkMsgBoxHead->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextOkMsgBoxHead->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextOkMsgBoxHead->ApplyTextFormat();

	constexpr XMFLOAT2 OK_MSGBOX_MSG_TEXT_SIZE(OK_MSGBOX_PANEL_SIZE.x - 40, OK_MSGBOX_PANEL_SIZE.y - OK_MSGBOX_HEAD_TEXT_SIZE.y - 100);
	constexpr XMFLOAT2 OK_MSGBOX_MSG_TEXT_OFFSET(0, OK_MSGBOX_HEAD_TEXT_OFFSET.y - OK_MSGBOX_HEAD_TEXT_SIZE.y / 2 - 10 - OK_MSGBOX_MSG_TEXT_SIZE.y / 2);
	UIObjectHandle hTextOkMsgBoxMsg = CreateText();
	pScriptLobbyHandler->m_hTextOkMsgBoxMsg = hTextOkMsgBoxMsg;
	Text* pTextOkMsgBoxMsg = static_cast<Text*>(hTextOkMsgBoxMsg.ToPtr());
	pTextOkMsgBoxMsg->m_transform.SetParent(&pPanelOkMsgBoxWindow->m_transform);
	pTextOkMsgBoxMsg->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextOkMsgBoxMsg->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextOkMsgBoxMsg->m_transform.SetPosition(OK_MSGBOX_MSG_TEXT_OFFSET);
	pTextOkMsgBoxMsg->SetSize(OK_MSGBOX_MSG_TEXT_SIZE);
	pTextOkMsgBoxMsg->SetText(L"알림 메시지입니다.");
	pTextOkMsgBoxMsg->SetColor(Colors::Goldenrod);
	pTextOkMsgBoxMsg->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextOkMsgBoxMsg->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextOkMsgBoxMsg->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pTextOkMsgBoxMsg->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pTextOkMsgBoxMsg->ApplyTextFormat();

	constexpr XMFLOAT2 OK_MSGBOX_OK_BUTTON_SIZE(80, 22);
	constexpr XMFLOAT2 OK_MSGBOX_OK_BUTTON_OFFSET(OK_MSGBOX_PANEL_SIZE.x / 2 - 10 - OK_MSGBOX_OK_BUTTON_SIZE.x / 2, -OK_MSGBOX_PANEL_SIZE.y / 2 + 10 + OK_MSGBOX_OK_BUTTON_SIZE.y / 2);
	UIObjectHandle hButtonOkMsgBoxOkButton = CreateButton();
	Button* pButtonOkMsgBoxOkButton = static_cast<Button*>(hButtonOkMsgBoxOkButton.ToPtr());
	pButtonOkMsgBoxOkButton->m_transform.SetParent(&pPanelOkMsgBoxWindow->m_transform);
	pButtonOkMsgBoxOkButton->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonOkMsgBoxOkButton->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonOkMsgBoxOkButton->m_transform.SetPosition(OK_MSGBOX_OK_BUTTON_OFFSET);
	pButtonOkMsgBoxOkButton->SetSize(OK_MSGBOX_OK_BUTTON_SIZE);
	pButtonOkMsgBoxOkButton->SetTextColor(Colors::Gold);
	pButtonOkMsgBoxOkButton->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonOkMsgBoxOkButton->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonOkMsgBoxOkButton->SetText(L"확인");
	pButtonOkMsgBoxOkButton->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonOkMsgBoxOkButton->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonOkMsgBoxOkButton->ApplyTextFormat();
	pButtonOkMsgBoxOkButton->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickOkMsgBoxOk));



	// ############################
	// 채널 브라우저 UI
	constexpr XMFLOAT2 CHANNEL_BROWSER_PANEL_SIZE(1280, 700);
	constexpr XMFLOAT2 CHANNEL_BROWSER_PANEL_OFFSET(0, +40);
	UIObjectHandle hPanelChannelBrowserRoot = CreatePanel();
	pScriptLobbyHandler->m_hPanelChannelBrowserRoot = hPanelChannelBrowserRoot;
	Panel* pPanelChannelBrowserRoot = static_cast<Panel*>(hPanelChannelBrowserRoot.ToPtr());
	pPanelChannelBrowserRoot->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pPanelChannelBrowserRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelChannelBrowserRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelChannelBrowserRoot->m_transform.SetPosition(CHANNEL_BROWSER_PANEL_OFFSET);
	pPanelChannelBrowserRoot->SetSize(CHANNEL_BROWSER_PANEL_SIZE);
	pPanelChannelBrowserRoot->SetColor(XMVectorSetW(Colors::DimGray, 0.6f));
	pPanelChannelBrowserRoot->SetShape(PanelShape::RoundedRectangle);
	pPanelChannelBrowserRoot->SetRadius(8.0f, 8.0f);

	UIObjectHandle hButtonExitGame = CreateButton();
	Button* pButtonExitGame = static_cast<Button*>(hButtonExitGame.ToPtr());
	pButtonExitGame->m_transform.SetParent(&pPanelChannelBrowserRoot->m_transform);
	pButtonExitGame->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
	pButtonExitGame->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pButtonExitGame->m_transform.SetPosition(-55, -18);
	pButtonExitGame->SetSize(100, 26);
	// pButtonExitGame->SetButtonColor(Colors::Red);
	pButtonExitGame->SetTextColor(Colors::Gold);
	pButtonExitGame->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonExitGame->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonExitGame->SetText(L"게임종료");
	pButtonExitGame->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonExitGame->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonExitGame->ApplyTextFormat();
	pButtonExitGame->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickExitGame));

	constexpr float BROWSER_BOTTOM_BUTTON_COMMON_Y_OFFSET = +30;
	constexpr XMFLOAT2 CHANNEL_LIST_REFRESH_BUTTON_OFFSET(+540, -CHANNEL_BROWSER_PANEL_SIZE.y / 2 + BROWSER_BOTTOM_BUTTON_COMMON_Y_OFFSET + CHANNEL_BROWSER_PANEL_OFFSET.y);
	UIObjectHandle hButtonRefreshChannelList = CreateButton();
	Button* pButtonRefreshChannelList = static_cast<Button*>(hButtonRefreshChannelList.ToPtr());
	pButtonRefreshChannelList->m_transform.SetParent(&pPanelChannelBrowserRoot->m_transform);
	pButtonRefreshChannelList->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonRefreshChannelList->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonRefreshChannelList->m_transform.SetPosition(CHANNEL_LIST_REFRESH_BUTTON_OFFSET);
	pButtonRefreshChannelList->SetSize(100, 26);
	pButtonRefreshChannelList->SetTextColor(Colors::Gold);
	pButtonRefreshChannelList->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonRefreshChannelList->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonRefreshChannelList->SetText(L"새로고침");
	pButtonRefreshChannelList->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonRefreshChannelList->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonRefreshChannelList->ApplyTextFormat();
	pButtonRefreshChannelList->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickRefreshChannelList));


	constexpr XMFLOAT2 CHANNEL_BROWSER_HEAD_TEXT_OFFSET(-CHANNEL_BROWSER_PANEL_SIZE.x / 2 + 80 + CHANNEL_BROWSER_PANEL_OFFSET.x, CHANNEL_BROWSER_PANEL_SIZE.y / 2 - 40 + CHANNEL_BROWSER_PANEL_OFFSET.y);
	UIObjectHandle hTextChannelBrowserHead = CreateText();
	Text* pTextChannelBrowserHead = static_cast<Text*>(hTextChannelBrowserHead.ToPtr());
	pTextChannelBrowserHead->m_transform.SetParent(&pPanelChannelBrowserRoot->m_transform);
	pTextChannelBrowserHead->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextChannelBrowserHead->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextChannelBrowserHead->m_transform.SetPosition(CHANNEL_BROWSER_HEAD_TEXT_OFFSET);
	pTextChannelBrowserHead->SetSize(100, 30);
	pTextChannelBrowserHead->SetText(L"채널목록");
	pTextChannelBrowserHead->SetColor(Colors::Goldenrod);
	pTextChannelBrowserHead->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextChannelBrowserHead->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextChannelBrowserHead->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextChannelBrowserHead->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextChannelBrowserHead->ApplyTextFormat();

	constexpr XMFLOAT2 CHANNEL_LIST_BROWSER_TAB_SIZE(1240, 30);
	constexpr XMFLOAT2 CHANNEL_LIST_BROWSER_TAB_OFFSET(0, +300);
	UIObjectHandle hPanelChannelListBrowserTab = CreatePanel();
	Panel* pPanelChannelListBrowserTab = static_cast<Panel*>(hPanelChannelListBrowserTab.ToPtr());
	pPanelChannelListBrowserTab->m_transform.SetParent(&pPanelChannelBrowserRoot->m_transform);
	pPanelChannelListBrowserTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelChannelListBrowserTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelChannelListBrowserTab->m_transform.SetPosition(CHANNEL_LIST_BROWSER_TAB_OFFSET);
	pPanelChannelListBrowserTab->SetSize(CHANNEL_LIST_BROWSER_TAB_SIZE);
	pPanelChannelListBrowserTab->SetColor(XMVectorSetW(ColorsLinear::DarkOliveGreen, 0.4f));
	pPanelChannelListBrowserTab->SetShape(PanelShape::Rectangle);

	constexpr XMFLOAT2 CHANNEL_NAME_TAB_OFFSET(-300, CHANNEL_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 CHANNEL_NAME_TAB_SIZE(400, +20);
	UIObjectHandle hTextChannelNameTab = CreateText();
	Text* pTextChannelNameTab = static_cast<Text*>(hTextChannelNameTab.ToPtr());
	pTextChannelNameTab->m_transform.SetParent(&pPanelChannelBrowserRoot->m_transform);
	pTextChannelNameTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextChannelNameTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextChannelNameTab->m_transform.SetPosition(CHANNEL_NAME_TAB_OFFSET);
	pTextChannelNameTab->SetText(L"채널명");
	pTextChannelNameTab->SetColor(Colors::Goldenrod);
	pTextChannelNameTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextChannelNameTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextChannelNameTab->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextChannelNameTab->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextChannelNameTab->ApplyTextFormat();
	pTextChannelNameTab->SetSize(CHANNEL_NAME_TAB_SIZE);

	constexpr XMFLOAT2 CHANNEL_HEADCOUNT_TAB_OFFSET(+200, CHANNEL_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 CHANNEL_HEADCOUNT_TAB_SIZE(+200, +20);
	UIObjectHandle hTextChannelHeadcountTab = CreateText();
	Text* pTextChannelHeadcountTab = static_cast<Text*>(hTextChannelHeadcountTab.ToPtr());
	pTextChannelHeadcountTab->m_transform.SetParent(&pPanelChannelBrowserRoot->m_transform);
	pTextChannelHeadcountTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextChannelHeadcountTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextChannelHeadcountTab->m_transform.SetPosition(CHANNEL_HEADCOUNT_TAB_OFFSET);
	pTextChannelHeadcountTab->SetText(L"인원수");
	pTextChannelHeadcountTab->SetColor(Colors::Goldenrod);
	pTextChannelHeadcountTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextChannelHeadcountTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextChannelHeadcountTab->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextChannelHeadcountTab->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextChannelHeadcountTab->ApplyTextFormat();
	pTextChannelHeadcountTab->SetSize(CHANNEL_HEADCOUNT_TAB_SIZE);

	constexpr float UI_TAB_ITEM_VERTICAL_DISTANCE = 60;
	constexpr float LIST_ITEM_VERTICAL_DISTANCE = 40;
	Button* pTempPtrButtonJoinChannel[CHANNEL_COUNT] = { nullptr };
	for (size_t i = 0; i < CHANNEL_COUNT; ++i)
	{
		UIObjectHandle hTextChannelName = CreateText();
		Text* pTextChannelName = static_cast<Text*>(hTextChannelName.ToPtr());
		pTextChannelName->m_transform.SetParent(&pPanelChannelBrowserRoot->m_transform);
		pTextChannelName->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextChannelName->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextChannelName->m_transform.SetPosition(CHANNEL_NAME_TAB_OFFSET.x, CHANNEL_NAME_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		wchar_t buf[32];
		StringCchPrintfW(buf, _countof(buf), L"게임 채널 %u", static_cast<uint32_t>(i));
		pTextChannelName->SetText(buf);
		pTextChannelName->SetColor(Colors::WhiteSmoke);
		pTextChannelName->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextChannelName->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextChannelName->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
		pTextChannelName->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pTextChannelName->ApplyTextFormat();
		pTextChannelName->SetSize(CHANNEL_NAME_TAB_SIZE);

		UIObjectHandle hTextChannelHeadcount = CreateText();
		pScriptLobbyHandler->m_hTextChannelHeadcount[i] = hTextChannelHeadcount;
		Text* pTextChannelHeadcount = static_cast<Text*>(hTextChannelHeadcount.ToPtr());
		pTextChannelHeadcount->m_transform.SetParent(&pPanelChannelBrowserRoot->m_transform);
		pTextChannelHeadcount->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextChannelHeadcount->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextChannelHeadcount->m_transform.SetPosition(CHANNEL_HEADCOUNT_TAB_OFFSET.x, CHANNEL_HEADCOUNT_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextChannelHeadcount->SetText(L"- / -");
		pTextChannelHeadcount->SetColor(Colors::WhiteSmoke);
		pTextChannelHeadcount->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextChannelHeadcount->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextChannelHeadcount->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
		pTextChannelHeadcount->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pTextChannelHeadcount->ApplyTextFormat();
		pTextChannelHeadcount->SetSize(CHANNEL_HEADCOUNT_TAB_SIZE);

		UIObjectHandle hButtonJoinChannel = CreateButton();		// 행 항목 전체를 덮는 투명한 버튼
		Button* pButtonJoinChannel = static_cast<Button*>(hButtonJoinChannel.ToPtr());
		pTempPtrButtonJoinChannel[i] = pButtonJoinChannel;
		pButtonJoinChannel->m_transform.SetParent(&pPanelChannelBrowserRoot->m_transform);
		pButtonJoinChannel->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pButtonJoinChannel->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pButtonJoinChannel->m_transform.SetPosition(+400, CHANNEL_HEADCOUNT_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pButtonJoinChannel->SetText(L"입장");
		pButtonJoinChannel->SetSize(100, 26);
		pButtonJoinChannel->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pButtonJoinChannel->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pButtonJoinChannel->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
		pButtonJoinChannel->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pButtonJoinChannel->ApplyTextFormat();
	}
	pTempPtrButtonJoinChannel[0]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinChannel0));
	pTempPtrButtonJoinChannel[0] = nullptr;
	pTempPtrButtonJoinChannel[1]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinChannel1));
	pTempPtrButtonJoinChannel[1] = nullptr;
	pTempPtrButtonJoinChannel[2]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinChannel2));
	pTempPtrButtonJoinChannel[2] = nullptr;
	pTempPtrButtonJoinChannel[3]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinChannel3));
	pTempPtrButtonJoinChannel[3] = nullptr;
	pTempPtrButtonJoinChannel[4]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinChannel4));
	pTempPtrButtonJoinChannel[4] = nullptr;
	pTempPtrButtonJoinChannel[5]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinChannel5));
	pTempPtrButtonJoinChannel[5] = nullptr;
	for (size_t i = 0; i < _countof(pTempPtrButtonJoinChannel); ++i)
		assert(pTempPtrButtonJoinChannel[i] == nullptr);


	// ############################
	// 방목록 브라우저 UI
	constexpr XMFLOAT2 GAME_BROWSER_PANEL_SIZE(1280, 700);
	constexpr XMFLOAT2 GAME_BROWSER_PANEL_OFFSET(0, +40);
	UIObjectHandle hPanelGameListBrowserRoot = CreatePanel();
	pScriptLobbyHandler->m_hPanelGameListBrowserRoot = hPanelGameListBrowserRoot;
	Panel* pPanelGameListBrowserRoot = static_cast<Panel*>(hPanelGameListBrowserRoot.ToPtr());
	pPanelGameListBrowserRoot->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pPanelGameListBrowserRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelGameListBrowserRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelGameListBrowserRoot->m_transform.SetPosition(GAME_BROWSER_PANEL_OFFSET);
	pPanelGameListBrowserRoot->SetSize(GAME_BROWSER_PANEL_SIZE);
	pPanelGameListBrowserRoot->SetColor(XMVectorSetW(Colors::DimGray, 0.6f));
	pPanelGameListBrowserRoot->SetShape(PanelShape::RoundedRectangle);
	pPanelGameListBrowserRoot->SetRadius(8.0f, 8.0f);

	UIObjectHandle hButtonExitGameListBrowser = CreateButton();
	Button* pButtonExitGameListBrowser = static_cast<Button*>(hButtonExitGameListBrowser.ToPtr());
	pButtonExitGameListBrowser->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pButtonExitGameListBrowser->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
	pButtonExitGameListBrowser->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pButtonExitGameListBrowser->m_transform.SetPosition(-55, -18);
	pButtonExitGameListBrowser->SetSize(100, 26);
	pButtonExitGameListBrowser->SetButtonColor(Colors::DeepSkyBlue);
	pButtonExitGameListBrowser->SetTextColor(Colors::Black);
	pButtonExitGameListBrowser->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonExitGameListBrowser->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonExitGameListBrowser->SetText(L"채널선택");
	pButtonExitGameListBrowser->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonExitGameListBrowser->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonExitGameListBrowser->ApplyTextFormat();
	pButtonExitGameListBrowser->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickExitGameListBrowser));

	UIObjectHandle hButtonOpenShop = CreateButton();
	pScriptLobbyHandler->m_hButtonOpenShop = hButtonOpenShop;
	Button* pButtonOpenShop = static_cast<Button*>(hButtonOpenShop.ToPtr());
	pButtonOpenShop->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pButtonOpenShop->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pButtonOpenShop->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pButtonOpenShop->m_transform.SetPosition(+55, -18);
	pButtonOpenShop->SetSize(100, 26);
	pButtonOpenShop->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonOpenShop->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonOpenShop->SetText(L"상점/정비");
	pButtonOpenShop->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonOpenShop->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonOpenShop->ApplyTextFormat();


	UIObjectHandle hButtonUserInfo = CreateButton();
	pScriptLobbyHandler->m_hButtonUserInfo = hButtonUserInfo;
	Button* pButtonUserInfo = static_cast<Button*>(hButtonUserInfo.ToPtr());
	pButtonUserInfo->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pButtonUserInfo->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
	pButtonUserInfo->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
	pButtonUserInfo->m_transform.SetPosition(+55, -18 - 31);
	pButtonUserInfo->SetSize(100, 26);
	pButtonUserInfo->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonUserInfo->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonUserInfo->SetText(L"내 정보");
	pButtonUserInfo->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonUserInfo->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonUserInfo->ApplyTextFormat();


	constexpr XMFLOAT2 GAME_BROWSER_HEAD_TEXT_OFFSET(-GAME_BROWSER_PANEL_SIZE.x / 2 + 80 + GAME_BROWSER_PANEL_OFFSET.x, GAME_BROWSER_PANEL_SIZE.y / 2 - 40 + GAME_BROWSER_PANEL_OFFSET.y);
	UIObjectHandle hTextGameBrowserHead = CreateText();
	Text* pTextGameBrowserHead = static_cast<Text*>(hTextGameBrowserHead.ToPtr());
	pTextGameBrowserHead->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pTextGameBrowserHead->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameBrowserHead->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameBrowserHead->m_transform.SetPosition(GAME_BROWSER_HEAD_TEXT_OFFSET);
	pTextGameBrowserHead->SetText(L"방목록");
	pTextGameBrowserHead->SetColor(Colors::Goldenrod);
	pTextGameBrowserHead->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameBrowserHead->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameBrowserHead->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextGameBrowserHead->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextGameBrowserHead->ApplyTextFormat();
	pTextGameBrowserHead->SetSize(100, 30);

	constexpr XMFLOAT2 GAME_LIST_BROWSER_TAB_SIZE(1240, 30);
	constexpr XMFLOAT2 GAME_LIST_BROWSER_TAB_OFFSET(0, +300);
	UIObjectHandle hPanelGameListBrowserTab = CreatePanel();
	Panel* pPanelGameListBrowserTab = static_cast<Panel*>(hPanelGameListBrowserTab.ToPtr());
	pPanelGameListBrowserTab->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pPanelGameListBrowserTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelGameListBrowserTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelGameListBrowserTab->m_transform.SetPosition(GAME_LIST_BROWSER_TAB_OFFSET);
	pPanelGameListBrowserTab->SetSize(GAME_LIST_BROWSER_TAB_SIZE);
	pPanelGameListBrowserTab->SetColor(XMVectorSetW(ColorsLinear::DarkOliveGreen, 0.4f));
	pPanelGameListBrowserTab->SetShape(PanelShape::Rectangle);

	UIObjectHandle hPanelGameSelectedIndicator = CreatePanel();
	pScriptLobbyHandler->m_hPanelGameSelectedIndicator = hPanelGameSelectedIndicator;
	Panel* pPanelGameSelectedIndicator = static_cast<Panel*>(hPanelGameSelectedIndicator.ToPtr());
	pPanelGameSelectedIndicator->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
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
	pTextGameNoTab->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pTextGameNoTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameNoTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameNoTab->m_transform.SetPosition(GAME_NO_TAB_OFFSET);
	pTextGameNoTab->SetText(L"No");
	pTextGameNoTab->SetColor(Colors::Goldenrod);
	pTextGameNoTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameNoTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameNoTab->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextGameNoTab->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextGameNoTab->ApplyTextFormat();
	pTextGameNoTab->SetSize(GAME_NO_TAB_SIZE);

	constexpr XMFLOAT2 GAME_NAME_TAB_OFFSET(-220, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_NAME_TAB_SIZE(+420, +20);
	UIObjectHandle hTextGameNameTab = CreateText();
	Text* pTextGameNameTab = static_cast<Text*>(hTextGameNameTab.ToPtr());
	pTextGameNameTab->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pTextGameNameTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameNameTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameNameTab->m_transform.SetPosition(GAME_NAME_TAB_OFFSET);
	pTextGameNameTab->SetText(L"방제목");
	pTextGameNameTab->SetColor(Colors::Goldenrod);
	pTextGameNameTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameNameTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameNameTab->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextGameNameTab->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextGameNameTab->ApplyTextFormat();
	pTextGameNameTab->SetSize(GAME_NAME_TAB_SIZE);

	constexpr XMFLOAT2 GAME_MAP_TAB_OFFSET(+100, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_MAP_TAB_SIZE(+120, +20);
	UIObjectHandle hTextGameMapTab = CreateText();
	Text* pTextGameMapTab = static_cast<Text*>(hTextGameMapTab.ToPtr());
	pTextGameMapTab->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pTextGameMapTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameMapTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameMapTab->m_transform.SetPosition(GAME_MAP_TAB_OFFSET);
	pTextGameMapTab->SetText(L"맵");
	pTextGameMapTab->SetColor(Colors::Goldenrod);
	pTextGameMapTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameMapTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameMapTab->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextGameMapTab->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextGameMapTab->ApplyTextFormat();
	pTextGameMapTab->SetSize(GAME_MAP_TAB_SIZE);

	constexpr XMFLOAT2 GAME_HEADCOUNT_TAB_OFFSET(+240, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_HEADCOUNT_TAB_SIZE(+100, +20);
	UIObjectHandle hTextGameHeadcountTab = CreateText();
	Text* pTextGameHeadcountTab = static_cast<Text*>(hTextGameHeadcountTab.ToPtr());
	pTextGameHeadcountTab->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pTextGameHeadcountTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameHeadcountTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameHeadcountTab->m_transform.SetPosition(GAME_HEADCOUNT_TAB_OFFSET);
	pTextGameHeadcountTab->SetText(L"인원수");
	pTextGameHeadcountTab->SetColor(Colors::Goldenrod);
	pTextGameHeadcountTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameHeadcountTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameHeadcountTab->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextGameHeadcountTab->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextGameHeadcountTab->ApplyTextFormat();
	pTextGameHeadcountTab->SetSize(GAME_HEADCOUNT_TAB_SIZE);

	constexpr XMFLOAT2 GAME_MODE_TAB_POS(+340, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_MODE_TAB_SIZE(+120, +20);
	UIObjectHandle hTextGameModeTab = CreateText();
	Text* pTextGameModeTab = static_cast<Text*>(hTextGameModeTab.ToPtr());
	pTextGameModeTab->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pTextGameModeTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameModeTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameModeTab->m_transform.SetPosition(GAME_MODE_TAB_POS);
	pTextGameModeTab->SetText(L"게임모드");
	pTextGameModeTab->SetColor(Colors::Goldenrod);
	pTextGameModeTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameModeTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameModeTab->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextGameModeTab->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextGameModeTab->ApplyTextFormat();
	pTextGameModeTab->SetSize(GAME_MODE_TAB_SIZE);

	constexpr XMFLOAT2 GAME_STATE_TAB_OFFSET(+440, GAME_LIST_BROWSER_TAB_OFFSET.y);
	constexpr XMFLOAT2 GAME_STATE_TAB_SIZE(+80, +20);
	UIObjectHandle hTextGameStateTab = CreateText();
	Text* pTextGameStateTab = static_cast<Text*>(hTextGameStateTab.ToPtr());
	pTextGameStateTab->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pTextGameStateTab->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameStateTab->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameStateTab->m_transform.SetPosition(GAME_STATE_TAB_OFFSET);
	pTextGameStateTab->SetText(L"방상태");
	pTextGameStateTab->SetColor(Colors::Goldenrod);
	pTextGameStateTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameStateTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameStateTab->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextGameStateTab->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextGameStateTab->ApplyTextFormat();
	pTextGameStateTab->SetSize(GAME_STATE_TAB_SIZE);
	
	Button* pTempPtrButtonJoinGameRoom[MAX_GAME_PER_LIST_PAGE] = { nullptr };
	for (size_t i = 0; i < MAX_GAME_PER_LIST_PAGE; ++i)
	{
		UIObjectHandle hTextGameNo = CreateText();
		pScriptLobbyHandler->m_hTextGameNo[i] = hTextGameNo;
		Text* pTextGameNo = static_cast<Text*>(hTextGameNo.ToPtr());
		pTextGameNo->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
		pTextGameNo->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameNo->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameNo->m_transform.SetPosition(GAME_NO_TAB_OFFSET.x, GAME_NO_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameNo->SetColor(Colors::WhiteSmoke);
		pTextGameNo->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameNo->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameNo->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
		pTextGameNo->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pTextGameNo->ApplyTextFormat();
		pTextGameNo->SetSize(GAME_NO_TAB_SIZE);

		UIObjectHandle hTextGameName = CreateText();
		pScriptLobbyHandler->m_hTextGameName[i] = hTextGameName;
		Text* pTextGameName = static_cast<Text*>(hTextGameName.ToPtr());
		pTextGameName->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
		pTextGameName->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameName->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameName->m_transform.SetPosition(GAME_NAME_TAB_OFFSET.x, GAME_NAME_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameName->SetColor(Colors::WhiteSmoke);
		pTextGameName->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameName->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameName->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
		pTextGameName->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pTextGameName->ApplyTextFormat();
		pTextGameName->SetSize(GAME_NAME_TAB_SIZE);

		UIObjectHandle hTextGameMap = CreateText();
		pScriptLobbyHandler->m_hTextGameMap[i] = hTextGameMap;
		Text* pTextGameMap = static_cast<Text*>(hTextGameMap.ToPtr());
		pTextGameMap->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
		pTextGameMap->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameMap->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameMap->m_transform.SetPosition(GAME_MAP_TAB_OFFSET.x, GAME_MAP_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameMap->SetColor(Colors::WhiteSmoke);
		pTextGameMap->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameMap->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameMap->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
		pTextGameMap->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pTextGameMap->ApplyTextFormat();
		pTextGameMap->SetSize(GAME_MAP_TAB_SIZE);

		UIObjectHandle hTextGameHeadcount = CreateText();
		pScriptLobbyHandler->m_hTextGameHeadcount[i] = hTextGameHeadcount;
		Text* pTextGameHeadcount = static_cast<Text*>(hTextGameHeadcount.ToPtr());
		pTextGameHeadcount->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
		pTextGameHeadcount->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameHeadcount->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameHeadcount->m_transform.SetPosition(GAME_HEADCOUNT_TAB_OFFSET.x, GAME_HEADCOUNT_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameHeadcount->SetColor(Colors::WhiteSmoke);
		pTextGameHeadcount->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameHeadcount->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameHeadcount->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
		pTextGameHeadcount->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pTextGameHeadcount->ApplyTextFormat();
		pTextGameHeadcount->SetSize(GAME_HEADCOUNT_TAB_SIZE);

		UIObjectHandle hTextGameMode = CreateText();
		pScriptLobbyHandler->m_hTextGameMode[i] = hTextGameMode;
		Text* pTextGameMode = static_cast<Text*>(hTextGameMode.ToPtr());
		pTextGameMode->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
		pTextGameMode->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameMode->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameMode->m_transform.SetPosition(GAME_MODE_TAB_POS.x, GAME_MODE_TAB_POS.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameMode->SetColor(Colors::WhiteSmoke);
		pTextGameMode->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameMode->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameMode->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
		pTextGameMode->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pTextGameMode->ApplyTextFormat();
		pTextGameMode->SetSize(GAME_MODE_TAB_SIZE);

		UIObjectHandle hTextGameState = CreateText();
		pScriptLobbyHandler->m_hTextGameState[i] = hTextGameState;
		Text* pTextGameState = static_cast<Text*>(hTextGameState.ToPtr());
		pTextGameState->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
		pTextGameState->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameState->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameState->m_transform.SetPosition(GAME_STATE_TAB_OFFSET.x, GAME_STATE_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pTextGameState->SetColor(Colors::GreenYellow);
		pTextGameState->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextGameState->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameState->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
		pTextGameState->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pTextGameState->ApplyTextFormat();
		pTextGameState->SetSize(GAME_STATE_TAB_SIZE);

		UIObjectHandle hButtonJoinGameRoom = CreateButton();		// 행 항목 전체를 덮는 투명한 버튼
		pScriptLobbyHandler->m_hButtonJoinGameRoom[i] = hButtonJoinGameRoom;
		Button* pButtonJoinGameRoom = static_cast<Button*>(hButtonJoinGameRoom.ToPtr());
		pTempPtrButtonJoinGameRoom[i] = pButtonJoinGameRoom;
		pButtonJoinGameRoom->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
		pButtonJoinGameRoom->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pButtonJoinGameRoom->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pButtonJoinGameRoom->m_transform.SetPosition(0, GAME_STATE_TAB_OFFSET.y - UI_TAB_ITEM_VERTICAL_DISTANCE - i * LIST_ITEM_VERTICAL_DISTANCE);
		pButtonJoinGameRoom->SetSize(GAME_LIST_BROWSER_TAB_SIZE);
		pButtonJoinGameRoom->SetButtonColorA(0.0f);
		pButtonJoinGameRoom->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pButtonJoinGameRoom->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pButtonJoinGameRoom->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
		pButtonJoinGameRoom->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pButtonJoinGameRoom->ApplyTextFormat();
	}
	pTempPtrButtonJoinGameRoom[0]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom0));
	pTempPtrButtonJoinGameRoom[1]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom1));
	pTempPtrButtonJoinGameRoom[2]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom2));
	pTempPtrButtonJoinGameRoom[3]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom3));
	pTempPtrButtonJoinGameRoom[4]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom4));
	pTempPtrButtonJoinGameRoom[5]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom5));
	pTempPtrButtonJoinGameRoom[6]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom6));
	pTempPtrButtonJoinGameRoom[7]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom7));
	pTempPtrButtonJoinGameRoom[8]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom8));
	pTempPtrButtonJoinGameRoom[9]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom9));
	pTempPtrButtonJoinGameRoom[10]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom10));
	pTempPtrButtonJoinGameRoom[11]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickJoinGameRoom11));
	

	constexpr XMFLOAT2 GAME_LIST_PREV_BUTTON_OFFSET(-80, -GAME_BROWSER_PANEL_SIZE.y / 2 + BROWSER_BOTTOM_BUTTON_COMMON_Y_OFFSET + GAME_BROWSER_PANEL_OFFSET.y);
	UIObjectHandle hButtonGameListPrev = CreateButton();
	Button* pButtonGameListPrev = static_cast<Button*>(hButtonGameListPrev.ToPtr());
	pButtonGameListPrev->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pButtonGameListPrev->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonGameListPrev->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonGameListPrev->m_transform.SetPosition(GAME_LIST_PREV_BUTTON_OFFSET);
	pButtonGameListPrev->SetSize(40, 26);
	pButtonGameListPrev->SetTextColor(Colors::Gold);
	pButtonGameListPrev->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonGameListPrev->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonGameListPrev->SetText(L"◀");
	pButtonGameListPrev->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonGameListPrev->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonGameListPrev->ApplyTextFormat();
	pButtonGameListPrev->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickGameListPrev));

	constexpr XMFLOAT2 GAME_LIST_NEXT_BUTTON_OFFSET(+80, GAME_LIST_PREV_BUTTON_OFFSET.y);
	UIObjectHandle hButtonGameListNext = CreateButton();
	Button* pButtonGameListNext = static_cast<Button*>(hButtonGameListNext.ToPtr());
	pButtonGameListNext->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pButtonGameListNext->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonGameListNext->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonGameListNext->m_transform.SetPosition(GAME_LIST_NEXT_BUTTON_OFFSET);
	pButtonGameListNext->SetSize(40, 26);
	pButtonGameListNext->SetTextColor(Colors::Gold);
	pButtonGameListNext->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonGameListNext->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonGameListNext->SetText(L"▶");
	pButtonGameListNext->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonGameListNext->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonGameListNext->ApplyTextFormat();
	pButtonGameListNext->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickGameListNext));

	constexpr XMFLOAT2 GAME_LIST_PAGE_TEXT_OFFSET(0, GAME_LIST_PREV_BUTTON_OFFSET.y);
	UIObjectHandle hTextGameListPage = CreateText();
	pScriptLobbyHandler->m_hTextGameListPage = hTextGameListPage;
	Text* pTextGameListPage = static_cast<Text*>(hTextGameListPage.ToPtr());
	pTextGameListPage->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pTextGameListPage->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameListPage->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameListPage->m_transform.SetPosition(GAME_LIST_PAGE_TEXT_OFFSET);
	pTextGameListPage->SetColor(Colors::GreenYellow);
	pTextGameListPage->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pTextGameListPage->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameListPage->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pTextGameListPage->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pTextGameListPage->ApplyTextFormat();
	pTextGameListPage->SetSize(80, 20);

	constexpr XMFLOAT2 GAME_LIST_REFRESH_BUTTON_OFFSET(+540, -GAME_BROWSER_PANEL_SIZE.y / 2 + BROWSER_BOTTOM_BUTTON_COMMON_Y_OFFSET + GAME_BROWSER_PANEL_OFFSET.y);
	UIObjectHandle hButtonRefreshGameList = CreateButton();
	Button* pButtonRefreshGameList = static_cast<Button*>(hButtonRefreshGameList.ToPtr());
	pButtonRefreshGameList->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pButtonRefreshGameList->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonRefreshGameList->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonRefreshGameList->m_transform.SetPosition(GAME_LIST_REFRESH_BUTTON_OFFSET);
	pButtonRefreshGameList->SetSize(100, 26);
	pButtonRefreshGameList->SetTextColor(Colors::Gold);
	pButtonRefreshGameList->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonRefreshGameList->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonRefreshGameList->SetText(L"새로고침");
	pButtonRefreshGameList->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonRefreshGameList->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonRefreshGameList->ApplyTextFormat();
	pButtonRefreshGameList->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickRefreshGameList));

	constexpr XMFLOAT2 CREATE_GAME_ROOM_BUTTON_OFFSET(-540, GAME_LIST_REFRESH_BUTTON_OFFSET.y);
	UIObjectHandle hButtonCreateGameRoom = CreateButton();
	Button* pButtonCreateGameRoom = static_cast<Button*>(hButtonCreateGameRoom.ToPtr());
	pButtonCreateGameRoom->m_transform.SetParent(&pPanelGameListBrowserRoot->m_transform);
	pButtonCreateGameRoom->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonCreateGameRoom->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonCreateGameRoom->m_transform.SetPosition(CREATE_GAME_ROOM_BUTTON_OFFSET);
	pButtonCreateGameRoom->SetSize(100, 26);
	pButtonCreateGameRoom->SetButtonColor(Colors::DodgerBlue);
	pButtonCreateGameRoom->SetTextColor(Colors::Black);
	pButtonCreateGameRoom->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonCreateGameRoom->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonCreateGameRoom->SetText(L"방생성");
	pButtonCreateGameRoom->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonCreateGameRoom->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonCreateGameRoom->ApplyTextFormat();
	pButtonCreateGameRoom->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickCreateGameRoom));

	// 방 생성 메뉴 UI
	constexpr XMFLOAT2 CREATE_GAME_PANEL_SIZE(600, 300);
	UIObjectHandle hPanelCreateGameRoomRoot = CreatePanel();
	pScriptLobbyHandler->m_hPanelCreateGameRoomRoot = hPanelCreateGameRoomRoot;
	Panel* pPanelCreateGameRoomRoot = static_cast<Panel*>(hPanelCreateGameRoomRoot.ToPtr());
	pPanelCreateGameRoomRoot->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pPanelCreateGameRoomRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelCreateGameRoomRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	// pPanelCreateGameRoomRoot->m_transform.SetPosition(0, 0);
	pPanelCreateGameRoomRoot->SetSize(CREATE_GAME_PANEL_SIZE);
	pPanelCreateGameRoomRoot->SetColor(ColorsLinear::DimGray);
	pPanelCreateGameRoomRoot->SetShape(PanelShape::RoundedRectangle);
	pPanelCreateGameRoomRoot->SetRadius(8.0f, 8.0f);

	constexpr XMFLOAT2 CREATE_GAME_HEAD_TEXT_SIZE(200, 30);
	UIObjectHandle hTextCreateGameHead = CreateText();
	Text* pTextCreateGameHead = static_cast<Text*>(hTextCreateGameHead.ToPtr());
	pTextCreateGameHead->m_transform.SetParent(&pPanelCreateGameRoomRoot->m_transform);
	pTextCreateGameHead->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextCreateGameHead->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextCreateGameHead->m_transform.SetPosition(-CREATE_GAME_PANEL_SIZE.x / 2 + CREATE_GAME_HEAD_TEXT_SIZE.x / 2 + 20, CREATE_GAME_PANEL_SIZE.y / 2 - CREATE_GAME_HEAD_TEXT_SIZE.y / 2 - 10);
	pTextCreateGameHead->SetSize(CREATE_GAME_HEAD_TEXT_SIZE);
	pTextCreateGameHead->SetColor(Colors::WhiteSmoke);
	pTextCreateGameHead->SetText(L"방생성");
	pTextCreateGameHead->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextCreateGameHead->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextCreateGameHead->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_HEAVY);
	pTextCreateGameHead->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pTextCreateGameHead->ApplyTextFormat();

	constexpr XMFLOAT2 CREATE_GAME_ROOM_NAME_DESC_TEXT_SIZE(60, 20);
	constexpr XMFLOAT2 CREATE_GAME_ROOM_NAME_DESC_TEXT_OFFSET(-200, +50);
	UIObjectHandle hTextCreateGameRoomNameDesc = CreateText();
	Text* pTextCreateGameRoomNameDesc = static_cast<Text*>(hTextCreateGameRoomNameDesc.ToPtr());
	pTextCreateGameRoomNameDesc->m_transform.SetParent(&pPanelCreateGameRoomRoot->m_transform);
	pTextCreateGameRoomNameDesc->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextCreateGameRoomNameDesc->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextCreateGameRoomNameDesc->m_transform.SetPosition(CREATE_GAME_ROOM_NAME_DESC_TEXT_OFFSET);
	pTextCreateGameRoomNameDesc->SetSize(CREATE_GAME_ROOM_NAME_DESC_TEXT_SIZE);
	pTextCreateGameRoomNameDesc->SetColor(Colors::WhiteSmoke);
	pTextCreateGameRoomNameDesc->SetText(L"방제목");
	pTextCreateGameRoomNameDesc->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextCreateGameRoomNameDesc->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextCreateGameRoomNameDesc->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pTextCreateGameRoomNameDesc->GetTextFormat().SetSize(STATIC_TEXT_SIZE_SMALL);
	pTextCreateGameRoomNameDesc->ApplyTextFormat();

	constexpr XMFLOAT2 GAME_ROOM_NAME_INPUT_FIELD_SIZE(400, 20);
	constexpr XMFLOAT2 GAME_ROOM_NAME_INPUT_FIELD_OFFSET(CREATE_GAME_ROOM_NAME_DESC_TEXT_OFFSET.x + CREATE_GAME_ROOM_NAME_DESC_TEXT_SIZE.x / 2 + GAME_ROOM_NAME_INPUT_FIELD_SIZE.x / 2 + 20, CREATE_GAME_ROOM_NAME_DESC_TEXT_OFFSET.y);
	UIObjectHandle hInputFieldCreateGameRoomName = CreateInputField();
	pScriptLobbyHandler->m_hInputFieldCreateGameRoomName = hInputFieldCreateGameRoomName;
	InputField* pInputFieldCreateGameRoomName = static_cast<InputField*>(hInputFieldCreateGameRoomName.ToPtr());
	pInputFieldCreateGameRoomName->m_transform.SetParent(&pPanelCreateGameRoomRoot->m_transform);
	pInputFieldCreateGameRoomName->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pInputFieldCreateGameRoomName->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pInputFieldCreateGameRoomName->m_transform.SetPosition(GAME_ROOM_NAME_INPUT_FIELD_OFFSET);
	pInputFieldCreateGameRoomName->SetSize(GAME_ROOM_NAME_INPUT_FIELD_SIZE);
	pInputFieldCreateGameRoomName->SetMaxChar(MAX_CHAT_MSG_LEN);
	pInputFieldCreateGameRoomName->SetTextColor(Colors::Black);
	pInputFieldCreateGameRoomName->SetBkColor(ColorsLinear::DimGray);
	pInputFieldCreateGameRoomName->GetTextFormat().SetSize(CHAT_MSG_TEXT_SIZE);
	pInputFieldCreateGameRoomName->ApplyTextFormat();
	pInputFieldCreateGameRoomName->SetMaxChar(MAX_GAME_ROOM_NAME_LEN);
	pInputFieldCreateGameRoomName->AllowReturn(false);
	pInputFieldCreateGameRoomName->AllowSpace(true);

	constexpr XMFLOAT2 CREATE_GAME_ROOM_MAX_PLAYER_DESC_TEXT_OFFSET(-220, -50);
	UIObjectHandle hTextCreateGameRoomMaxPlayerDesc = CreateText();
	Text* pTextCreateGameRoomMaxPlayerDesc = static_cast<Text*>(hTextCreateGameRoomMaxPlayerDesc.ToPtr());
	pTextCreateGameRoomMaxPlayerDesc->m_transform.SetParent(&pPanelCreateGameRoomRoot->m_transform);
	pTextCreateGameRoomMaxPlayerDesc->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextCreateGameRoomMaxPlayerDesc->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextCreateGameRoomMaxPlayerDesc->m_transform.SetPosition(CREATE_GAME_ROOM_MAX_PLAYER_DESC_TEXT_OFFSET);
	pTextCreateGameRoomMaxPlayerDesc->SetSize(100, 20);
	pTextCreateGameRoomMaxPlayerDesc->SetColor(Colors::WhiteSmoke);
	pTextCreateGameRoomMaxPlayerDesc->SetText(L"최대 인원 수");
	pTextCreateGameRoomMaxPlayerDesc->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextCreateGameRoomMaxPlayerDesc->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextCreateGameRoomMaxPlayerDesc->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pTextCreateGameRoomMaxPlayerDesc->GetTextFormat().SetSize(STATIC_TEXT_SIZE_SMALL);
	pTextCreateGameRoomMaxPlayerDesc->ApplyTextFormat();

	{
		RadioButton* pTempPtrRadioButtonGameRoomMaxPlayer[MAX_PLAYERS_PER_TEAM] = { nullptr };

		UIObjectHandle hRadioButtonGameRoomMaxPlayer1vs1 = CreateRadioButton();
		pScriptLobbyHandler->m_hRadioButtonGameRoomMaxPlayer[0] = hRadioButtonGameRoomMaxPlayer1vs1;
		RadioButton* pRadioButtonGameRoomMaxPlayer1vs1 = static_cast<RadioButton*>(hRadioButtonGameRoomMaxPlayer1vs1.ToPtr());
		pTempPtrRadioButtonGameRoomMaxPlayer[0] = pRadioButtonGameRoomMaxPlayer1vs1;
		pRadioButtonGameRoomMaxPlayer1vs1->m_transform.SetParent(&pPanelCreateGameRoomRoot->m_transform);
		pRadioButtonGameRoomMaxPlayer1vs1->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pRadioButtonGameRoomMaxPlayer1vs1->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pRadioButtonGameRoomMaxPlayer1vs1->m_transform.SetPosition(CREATE_GAME_ROOM_MAX_PLAYER_DESC_TEXT_OFFSET.x + 40, CREATE_GAME_ROOM_MAX_PLAYER_DESC_TEXT_OFFSET.y);
		pRadioButtonGameRoomMaxPlayer1vs1->SetText(L"1 vs 1");
		pRadioButtonGameRoomMaxPlayer1vs1->SetButtonColorRGB(ColorsLinear::SteelBlue);
		pRadioButtonGameRoomMaxPlayer1vs1->GetTextFormat().SetSize(STATIC_TEXT_SIZE_SMALL);
		pRadioButtonGameRoomMaxPlayer1vs1->ApplyTextFormat();

		for (int i = 1; i < MAX_PLAYERS_PER_TEAM; ++i)
		{
			wchar_t buf[16];
			StringCchPrintfW(buf, _countof(buf), L"%d vs %d", i + 1, i + 1);

			UIObjectHandle hRadioButtonGameRoomMaxPlayer = CreateRadioButton();
			pScriptLobbyHandler->m_hRadioButtonGameRoomMaxPlayer[i] = hRadioButtonGameRoomMaxPlayer;
			RadioButton* pRadioButtonGameRoomMaxPlayer = static_cast<RadioButton*>(hRadioButtonGameRoomMaxPlayer.ToPtr());
			pTempPtrRadioButtonGameRoomMaxPlayer[i] = pRadioButtonGameRoomMaxPlayer;
			pRadioButtonGameRoomMaxPlayer->m_transform.SetParent(&pPanelCreateGameRoomRoot->m_transform);
			pRadioButtonGameRoomMaxPlayer->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
			pRadioButtonGameRoomMaxPlayer->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
			pRadioButtonGameRoomMaxPlayer->m_transform.SetPosition(CREATE_GAME_ROOM_MAX_PLAYER_DESC_TEXT_OFFSET.x + 40 + i * 60, CREATE_GAME_ROOM_MAX_PLAYER_DESC_TEXT_OFFSET.y);
			pRadioButtonGameRoomMaxPlayer->SetText(buf);
			pRadioButtonGameRoomMaxPlayer->SetButtonColorRGB(ColorsLinear::SteelBlue);
			pRadioButtonGameRoomMaxPlayer->GetTextFormat().SetSize(STATIC_TEXT_SIZE_SMALL);
			pRadioButtonGameRoomMaxPlayer->ApplyTextFormat();

			// 라디오버튼 그룹화
			pRadioButtonGameRoomMaxPlayer1vs1->Join(pRadioButtonGameRoomMaxPlayer);
		}

		pTempPtrRadioButtonGameRoomMaxPlayer[0]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickRadioButtonGameRoom1vs1));
		pTempPtrRadioButtonGameRoomMaxPlayer[1]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickRadioButtonGameRoom2vs2));
		pTempPtrRadioButtonGameRoomMaxPlayer[2]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickRadioButtonGameRoom3vs3));
		pTempPtrRadioButtonGameRoomMaxPlayer[3]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickRadioButtonGameRoom4vs4));
		pTempPtrRadioButtonGameRoomMaxPlayer[4]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickRadioButtonGameRoom5vs5));
		pTempPtrRadioButtonGameRoomMaxPlayer[5]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickRadioButtonGameRoom6vs6));
		pTempPtrRadioButtonGameRoomMaxPlayer[6]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickRadioButtonGameRoom7vs7));
		pTempPtrRadioButtonGameRoomMaxPlayer[7]->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickRadioButtonGameRoom8vs8));
	}


	constexpr XMFLOAT2 CREATE_GAME_ROOM_REQ_BUTTON_SIZE(80, 22);
	constexpr XMFLOAT2 CREATE_GAME_ROOM_REQ_BUTTON_OFFSET(CREATE_GAME_PANEL_SIZE.x / 2 - CREATE_GAME_ROOM_REQ_BUTTON_SIZE.x / 2 - CREATE_GAME_ROOM_REQ_BUTTON_SIZE.x - 25, -CREATE_GAME_PANEL_SIZE.y / 2 + CREATE_GAME_ROOM_REQ_BUTTON_SIZE.y / 2 + 15);
	UIObjectHandle hButtonCreateGameRoomReq = CreateButton();
	Button* pButtonCreateGameRoomReq = static_cast<Button*>(hButtonCreateGameRoomReq.ToPtr());
	pButtonCreateGameRoomReq->m_transform.SetParent(&pPanelCreateGameRoomRoot->m_transform);
	pButtonCreateGameRoomReq->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonCreateGameRoomReq->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonCreateGameRoomReq->m_transform.SetPosition(CREATE_GAME_ROOM_REQ_BUTTON_OFFSET);
	pButtonCreateGameRoomReq->SetSize(CREATE_GAME_ROOM_REQ_BUTTON_SIZE);
	pButtonCreateGameRoomReq->SetButtonColor(Colors::DodgerBlue);
	pButtonCreateGameRoomReq->SetTextColor(Colors::Black);
	pButtonCreateGameRoomReq->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonCreateGameRoomReq->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonCreateGameRoomReq->SetText(L"생성");
	pButtonCreateGameRoomReq->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonCreateGameRoomReq->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonCreateGameRoomReq->ApplyTextFormat();
	pButtonCreateGameRoomReq->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickCreateGameRoomReq));

	constexpr XMFLOAT2 CREATE_GAME_ROOM_CANCEL_BUTTON_SIZE(80, 22);
	constexpr XMFLOAT2 CREATE_GAME_ROOM_CANCEL_BUTTON_OFFSET(CREATE_GAME_PANEL_SIZE.x / 2 - CREATE_GAME_ROOM_REQ_BUTTON_SIZE.x / 2 - 15, CREATE_GAME_ROOM_REQ_BUTTON_OFFSET.y);
	UIObjectHandle hButtonCreateGameRoomCancel = CreateButton();
	Button* pButtonCreateGameRoomCancel = static_cast<Button*>(hButtonCreateGameRoomCancel.ToPtr());
	pButtonCreateGameRoomCancel->m_transform.SetParent(&pPanelCreateGameRoomRoot->m_transform);
	pButtonCreateGameRoomCancel->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonCreateGameRoomCancel->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonCreateGameRoomCancel->m_transform.SetPosition(CREATE_GAME_ROOM_CANCEL_BUTTON_OFFSET);
	pButtonCreateGameRoomCancel->SetSize(CREATE_GAME_ROOM_CANCEL_BUTTON_SIZE);
	pButtonCreateGameRoomCancel->SetButtonColor(Colors::DimGray);
	pButtonCreateGameRoomCancel->SetTextColor(Colors::Black);
	pButtonCreateGameRoomCancel->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonCreateGameRoomCancel->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonCreateGameRoomCancel->SetText(L"닫기");
	pButtonCreateGameRoomCancel->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonCreateGameRoomCancel->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonCreateGameRoomCancel->ApplyTextFormat();
	pButtonCreateGameRoomCancel->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickCreateGameRoomCancel));


	// ############################
	// 채팅 UI
	constexpr XMFLOAT2 CHAT_PANEL_SIZE(840, 200);
	constexpr XMFLOAT2 CHAT_PANEL_OFFSET(-GAME_BROWSER_PANEL_SIZE.x / 2 + CHAT_PANEL_SIZE.x / 2, CHAT_PANEL_SIZE.y / 2 + 20);
	UIObjectHandle hPanelChatRoot = CreatePanel();
	pScriptLobbyHandler->m_hPanelChatRoot = hPanelChatRoot;
	Panel* pPanelChatRoot = static_cast<Panel*>(hPanelChatRoot.ToPtr());
	pPanelChatRoot->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pPanelChatRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelChatRoot->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pPanelChatRoot->m_transform.SetPosition(CHAT_PANEL_OFFSET);
	pPanelChatRoot->SetSize(CHAT_PANEL_SIZE);
	pPanelChatRoot->SetColor(XMVectorSetW(Colors::DimGray, 0.6f));
	pPanelChatRoot->SetShape(PanelShape::RoundedRectangle);
	pPanelChatRoot->SetRadius(8.0f, 8.0f);

	constexpr XMFLOAT2 SEND_CHAT_MSG_BUTTON_SIZE(80, 22);
	constexpr XMFLOAT2 SEND_CHAT_MSG_BUTTON_OFFSET(CHAT_PANEL_OFFSET.x + CHAT_PANEL_SIZE.x / 2 - SEND_CHAT_MSG_BUTTON_SIZE.x / 2 - 10, CHAT_PANEL_OFFSET.y - CHAT_PANEL_SIZE.y / 2 + SEND_CHAT_MSG_BUTTON_SIZE.y / 2 + 10);
	UIObjectHandle hButtonSendChatMsg = CreateButton();
	Button* pButtonSendChatMsg = static_cast<Button*>(hButtonSendChatMsg.ToPtr());
	pButtonSendChatMsg->m_transform.SetParent(&pPanelChatRoot->m_transform);
	pButtonSendChatMsg->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonSendChatMsg->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pButtonSendChatMsg->m_transform.SetPosition(SEND_CHAT_MSG_BUTTON_OFFSET);
	pButtonSendChatMsg->SetSize(SEND_CHAT_MSG_BUTTON_SIZE);
	pButtonSendChatMsg->SetTextColor(Colors::Gold);
	pButtonSendChatMsg->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonSendChatMsg->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonSendChatMsg->SetText(L"전송");
	pButtonSendChatMsg->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonSendChatMsg->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonSendChatMsg->ApplyTextFormat();
	pButtonSendChatMsg->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickSendChatMsg));

	constexpr XMFLOAT2 CHAT_INPUT_FIELD_SIZE(CHAT_PANEL_SIZE.x - SEND_CHAT_MSG_BUTTON_SIZE.x - 30, 22);
	constexpr XMFLOAT2 CHAT_INPUT_FIELD_OFFSET(CHAT_PANEL_OFFSET.x - CHAT_PANEL_SIZE.x / 2 + CHAT_INPUT_FIELD_SIZE.x / 2 + 10, SEND_CHAT_MSG_BUTTON_OFFSET.y);
	UIObjectHandle hInputFieldChatMsg = CreateInputField();
	pScriptLobbyHandler->m_hInputFieldChatMsg = hInputFieldChatMsg;
	InputField* pInputFieldChatInput = static_cast<InputField*>(hInputFieldChatMsg.ToPtr());
	pInputFieldChatInput->m_transform.SetParent(&pPanelChatRoot->m_transform);
	pInputFieldChatInput->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pInputFieldChatInput->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
	pInputFieldChatInput->m_transform.SetPosition(CHAT_INPUT_FIELD_OFFSET);
	pInputFieldChatInput->SetSize(CHAT_INPUT_FIELD_SIZE);
	pInputFieldChatInput->SetMaxChar(MAX_CHAT_MSG_LEN);
	pInputFieldChatInput->SetTextColor(Colors::Black);
	pInputFieldChatInput->SetBkColor(ColorsLinear::DimGray);
	pInputFieldChatInput->GetTextFormat().SetSize(CHAT_MSG_TEXT_SIZE);
	pInputFieldChatInput->ApplyTextFormat();
	pInputFieldChatInput->AllowReturn(false);
	pInputFieldChatInput->AllowSpace(true);

	constexpr XMFLOAT2 CHAT_MSG_SIZE(CHAT_INPUT_FIELD_SIZE.x + SEND_CHAT_MSG_BUTTON_SIZE.x, CHAT_INPUT_FIELD_SIZE.y);
	constexpr XMFLOAT2 FIRST_CHAT_MSG_OFFSET(CHAT_PANEL_OFFSET.x, CHAT_PANEL_OFFSET.y + CHAT_PANEL_SIZE.y / 2 - CHAT_MSG_SIZE.y / 2 - 10);
	for (size_t i = 0; i < CHAT_MSG_ITEM_ROW_COUNT; ++i)
	{
		UIObjectHandle hTextLobbyChatMsg = CreateText();
		pScriptLobbyHandler->m_hTextLobbyChatMsg[i] = hTextLobbyChatMsg;
		Text* pTextLobbyChatMsg = static_cast<Text*>(hTextLobbyChatMsg.ToPtr());
		pTextLobbyChatMsg->m_transform.SetParent(&pPanelChatRoot->m_transform);
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
	// 방 UI
	constexpr XMFLOAT2 GAME_ROOM_PANEL_SIZE(1280, 700);
	constexpr XMFLOAT2 GAME_ROOM_PANEL_OFFSET(0, +40);
	UIObjectHandle hPanelGameRoomRoot = CreatePanel();
	pScriptLobbyHandler->m_hPanelGameRoomRoot = hPanelGameRoomRoot;
	Panel* pPanelGameRoomRoot = static_cast<Panel*>(hPanelGameRoomRoot.ToPtr());
	pPanelGameRoomRoot->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pPanelGameRoomRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelGameRoomRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelGameRoomRoot->m_transform.SetPosition(GAME_ROOM_PANEL_OFFSET);
	pPanelGameRoomRoot->SetSize(GAME_ROOM_PANEL_SIZE);
	pPanelGameRoomRoot->SetColor(XMVectorSetW(Colors::DimGray, 0.6f));
	pPanelGameRoomRoot->SetShape(PanelShape::RoundedRectangle);
	pPanelGameRoomRoot->SetRadius(8.0f, 8.0f);

	// 맵 미리보기 이미지
	constexpr XMFLOAT2 GAME_ROOM_MAP_PREVIEW_IMAGE_SIZE(400, 300);
	constexpr XMFLOAT2 GAME_ROOM_MAP_PREVIEW_IMAGE_OFFSET(-GAME_ROOM_PANEL_SIZE.x / 2 + GAME_ROOM_MAP_PREVIEW_IMAGE_SIZE.x / 2 + 10, +GAME_ROOM_PANEL_SIZE.y / 2 - GAME_ROOM_MAP_PREVIEW_IMAGE_SIZE.y / 2 - 110);
	UIObjectHandle hImageGameRoomMapPreview = CreateImage();
	pScriptLobbyHandler->m_hImageGameRoomMapPreview = hImageGameRoomMapPreview;
	Image* pImageGameRoomMapPreview = static_cast<Image*>(hImageGameRoomMapPreview.ToPtr());
	pImageGameRoomMapPreview->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
	pImageGameRoomMapPreview->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pImageGameRoomMapPreview->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pImageGameRoomMapPreview->m_transform.SetPosition(GAME_ROOM_MAP_PREVIEW_IMAGE_OFFSET);
	pImageGameRoomMapPreview->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\warehouse_preview.png", false));
	pImageGameRoomMapPreview->SetSize(GAME_ROOM_MAP_PREVIEW_IMAGE_SIZE);
	pImageGameRoomMapPreview->SetNativeSize(false);

	constexpr XMFLOAT2 GAME_ROOM_CHANGE_MAP_BUTTON_SIZE(GAME_ROOM_MAP_PREVIEW_IMAGE_SIZE.x, 26);
	constexpr XMFLOAT2 GAME_ROOM_CHANGE_MAP_BUTTON_OFFSET(GAME_ROOM_MAP_PREVIEW_IMAGE_OFFSET.x, GAME_ROOM_MAP_PREVIEW_IMAGE_OFFSET.y - GAME_ROOM_MAP_PREVIEW_IMAGE_SIZE.y / 2 - GAME_ROOM_CHANGE_MAP_BUTTON_SIZE.y / 2 - 10);
	UIObjectHandle hButtonGameRoomChangeMap = CreateButton();
	Button* pButtonGameRoomChangeMap = static_cast<Button*>(hButtonGameRoomChangeMap.ToPtr());
	pButtonGameRoomChangeMap->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
	pButtonGameRoomChangeMap->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonGameRoomChangeMap->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonGameRoomChangeMap->m_transform.SetPosition(GAME_ROOM_CHANGE_MAP_BUTTON_OFFSET);
	pButtonGameRoomChangeMap->SetSize(GAME_ROOM_CHANGE_MAP_BUTTON_SIZE);
	pButtonGameRoomChangeMap->SetButtonColor(Colors::DarkOrange);
	pButtonGameRoomChangeMap->SetTextColor(Colors::Black);
	pButtonGameRoomChangeMap->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonGameRoomChangeMap->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonGameRoomChangeMap->SetText(L"맵 변경");
	pButtonGameRoomChangeMap->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonGameRoomChangeMap->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonGameRoomChangeMap->ApplyTextFormat();

	// 현재 게임 정보 표시 UI
	constexpr XMFLOAT2 GAME_ROOM_INFO_TEXT_SIZE(400, 120);
	constexpr XMFLOAT2 GAME_ROOM_INFO_TEXT_OFFSET(-GAME_ROOM_PANEL_SIZE.x / 2 + GAME_ROOM_INFO_TEXT_SIZE.x / 2 + 10, -GAME_ROOM_PANEL_SIZE.y / 2 + GAME_ROOM_INFO_TEXT_SIZE.y / 2 + 100);
	UIObjectHandle hTextGameRoomInfo = CreateText();
	pScriptLobbyHandler->m_hTextGameRoomInfo = hTextGameRoomInfo;
	Text* pTextGameRoomInfo = static_cast<Text*>(hTextGameRoomInfo.ToPtr());
	pTextGameRoomInfo->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
	pTextGameRoomInfo->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameRoomInfo->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameRoomInfo->m_transform.SetPosition(GAME_ROOM_INFO_TEXT_OFFSET);
	pTextGameRoomInfo->SetColor(Colors::WhiteSmoke);
	pTextGameRoomInfo->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextGameRoomInfo->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	pTextGameRoomInfo->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextGameRoomInfo->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextGameRoomInfo->ApplyTextFormat();
	pTextGameRoomInfo->SetSize(GAME_ROOM_INFO_TEXT_SIZE);

	constexpr XMFLOAT2 GAME_ROOM_NAME_PANEL_SIZE(840, 30);
	constexpr XMFLOAT2 GAME_ROOM_NAME_PANEL_OFFSET(GAME_ROOM_PANEL_SIZE.x / 2 - GAME_ROOM_NAME_PANEL_SIZE.x / 2 - 15 + GAME_ROOM_PANEL_OFFSET.x, GAME_ROOM_PANEL_SIZE.y / 2 - GAME_ROOM_NAME_PANEL_SIZE.y / 2 - 25 + GAME_ROOM_PANEL_OFFSET.y);
	UIObjectHandle hPanelGameNamePanel = CreatePanel();
	Panel* pPanelGameNamePanel = static_cast<Panel*>(hPanelGameNamePanel.ToPtr());
	pPanelGameNamePanel->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
	pPanelGameNamePanel->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelGameNamePanel->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelGameNamePanel->m_transform.SetPosition(GAME_ROOM_NAME_PANEL_OFFSET);
	pPanelGameNamePanel->SetSize(GAME_ROOM_NAME_PANEL_SIZE);
	pPanelGameNamePanel->SetColor(XMVectorSetW(Colors::Orange, 0.4f));
	pPanelGameNamePanel->SetShape(PanelShape::Rectangle);

	constexpr XMFLOAT2 GAME_ROOM_NAME_PANEL_TEXT_SIZE(GAME_ROOM_NAME_PANEL_SIZE.x - 20, 30);
	constexpr XMFLOAT2 GAME_ROOM_NAME_PANEL_TEXT_OFFSET(GAME_ROOM_NAME_PANEL_OFFSET);
	UIObjectHandle hTextGameRoomNamePanel = CreateText();
	pScriptLobbyHandler->m_hTextGameRoomNamePanel = hTextGameRoomNamePanel;
	Text* pTextGameRoomNamePanel = static_cast<Text*>(hTextGameRoomNamePanel.ToPtr());
	pTextGameRoomNamePanel->m_transform.SetParent(&pPanelGameNamePanel->m_transform);
	pTextGameRoomNamePanel->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pTextGameRoomNamePanel->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pTextGameRoomNamePanel->m_transform.SetPosition(GAME_ROOM_NAME_PANEL_TEXT_OFFSET);
	pTextGameRoomNamePanel->SetText(L"[132] Only the best there is!");
	pTextGameRoomNamePanel->SetColor(Colors::WhiteSmoke);
	pTextGameRoomNamePanel->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pTextGameRoomNamePanel->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextGameRoomNamePanel->GetTextFormat().SetWeight(MEDIUM_TEXT_WEIGHT);
	pTextGameRoomNamePanel->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
	pTextGameRoomNamePanel->ApplyTextFormat();
	pTextGameRoomNamePanel->SetSize(GAME_ROOM_NAME_PANEL_TEXT_SIZE);

	constexpr XMFLOAT2 MOVE_TEAM_BUTTON_SIZE(GAME_ROOM_NAME_PANEL_SIZE.x / 2 - 10, 50);
	constexpr XMFLOAT2 MOVE_TO_RED_TEAM_BUTTON_OFFSET(GAME_ROOM_NAME_PANEL_OFFSET.x - MOVE_TEAM_BUTTON_SIZE.x / 2 - 10, GAME_ROOM_NAME_PANEL_OFFSET.y - MOVE_TEAM_BUTTON_SIZE.y / 2 - 40);
	constexpr XMFLOAT4 MOVE_TEAM_BUTTON_COLOR(0.05f, 0.05f, 0.075f, 1.0f);
	UIObjectHandle hButtonMoveToRedTeam = CreateButton();
	Button* pButtonMoveToRedTeam = static_cast<Button*>(hButtonMoveToRedTeam.ToPtr());
	pButtonMoveToRedTeam->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
	pButtonMoveToRedTeam->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonMoveToRedTeam->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonMoveToRedTeam->m_transform.SetPosition(MOVE_TO_RED_TEAM_BUTTON_OFFSET);
	pButtonMoveToRedTeam->SetSize(MOVE_TEAM_BUTTON_SIZE);
	pButtonMoveToRedTeam->SetButtonColor(MOVE_TEAM_BUTTON_COLOR);
	pButtonMoveToRedTeam->SetTextColor(Colors::OrangeRed);
	pButtonMoveToRedTeam->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonMoveToRedTeam->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonMoveToRedTeam->SetText(L"RED TEAM");
	pButtonMoveToRedTeam->GetTextFormat().SetSize(40);
	pButtonMoveToRedTeam->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pButtonMoveToRedTeam->ApplyTextFormat();
	pButtonMoveToRedTeam->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickMoveToRedTeam));

	constexpr XMFLOAT2 MOVE_TO_BLUE_TEAM_BUTTON_OFFSET(GAME_ROOM_NAME_PANEL_OFFSET.x + MOVE_TEAM_BUTTON_SIZE.x / 2 + 10, MOVE_TO_RED_TEAM_BUTTON_OFFSET.y);
	UIObjectHandle hButtonMoveToBlueTeam = CreateButton();
	Button* pButtonMoveToBlueTeam = static_cast<Button*>(hButtonMoveToBlueTeam.ToPtr());
	pButtonMoveToBlueTeam->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
	pButtonMoveToBlueTeam->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonMoveToBlueTeam->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonMoveToBlueTeam->m_transform.SetPosition(MOVE_TO_BLUE_TEAM_BUTTON_OFFSET);
	pButtonMoveToBlueTeam->SetSize(MOVE_TEAM_BUTTON_SIZE);
	pButtonMoveToBlueTeam->SetButtonColor(MOVE_TEAM_BUTTON_COLOR);
	pButtonMoveToBlueTeam->SetTextColor(Colors::DeepSkyBlue);
	pButtonMoveToBlueTeam->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonMoveToBlueTeam->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonMoveToBlueTeam->SetText(L"BLUE TEAM");
	pButtonMoveToBlueTeam->GetTextFormat().SetSize(40);
	pButtonMoveToBlueTeam->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
	pButtonMoveToBlueTeam->ApplyTextFormat();
	pButtonMoveToBlueTeam->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickMoveToBlueTeam));

	constexpr XMFLOAT2 GAME_ROOM_TEAM_PANEL_SIZE(MOVE_TEAM_BUTTON_SIZE.x, 500);
	constexpr XMFLOAT2 GAME_ROOM_RED_TEAM_PANEL_OFFSET(MOVE_TO_RED_TEAM_BUTTON_OFFSET.x, MOVE_TO_BLUE_TEAM_BUTTON_OFFSET.y - MOVE_TEAM_BUTTON_SIZE.y / 2 - GAME_ROOM_TEAM_PANEL_SIZE.y / 2 - 20);
	UIObjectHandle hPanelGameRoomRedTeam = CreatePanel();
	Panel* pPanelGameRoomRedTeam = static_cast<Panel*>(hPanelGameRoomRedTeam.ToPtr());
	pPanelGameRoomRedTeam->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
	pPanelGameRoomRedTeam->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelGameRoomRedTeam->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelGameRoomRedTeam->m_transform.SetPosition(GAME_ROOM_RED_TEAM_PANEL_OFFSET);
	pPanelGameRoomRedTeam->SetSize(GAME_ROOM_TEAM_PANEL_SIZE);
	pPanelGameRoomRedTeam->SetColor(XMVectorSetW(Colors::Red, 0.2f));
	pPanelGameRoomRedTeam->SetShape(PanelShape::Rectangle);

	constexpr XMFLOAT2 GAME_ROOM_BLUE_TEAM_PANEL_OFFSET(MOVE_TO_BLUE_TEAM_BUTTON_OFFSET.x, GAME_ROOM_RED_TEAM_PANEL_OFFSET.y);
	UIObjectHandle hPanelGameRoomBlueTeam = CreatePanel();
	Panel* pPanelGameRoomBlueTeam = static_cast<Panel*>(hPanelGameRoomBlueTeam.ToPtr());
	pPanelGameRoomBlueTeam->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
	pPanelGameRoomBlueTeam->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelGameRoomBlueTeam->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelGameRoomBlueTeam->m_transform.SetPosition(GAME_ROOM_BLUE_TEAM_PANEL_OFFSET);
	pPanelGameRoomBlueTeam->SetSize(GAME_ROOM_TEAM_PANEL_SIZE);
	pPanelGameRoomBlueTeam->SetColor(XMVectorSetW(Colors::Blue, 0.2f));
	pPanelGameRoomBlueTeam->SetShape(PanelShape::Rectangle);

	constexpr XMFLOAT2 GAME_ROOM_PLAYER_NAME_TEXT_SIZE(MOVE_TEAM_BUTTON_SIZE.x - 20, 30);
	constexpr XMFLOAT2 FIRST_GAME_ROOM_RED_TEAM_PLAYER_NAME_TEXT_OFFSET(MOVE_TO_RED_TEAM_BUTTON_OFFSET.x, MOVE_TO_RED_TEAM_BUTTON_OFFSET.y - MOVE_TEAM_BUTTON_SIZE.y / 2 - GAME_ROOM_PLAYER_NAME_TEXT_SIZE.y / 2 - 30);
	constexpr XMFLOAT2 FIRST_GAME_ROOM_BLUE_TEAM_PLAYER_NAME_TEXT_OFFSET(MOVE_TO_BLUE_TEAM_BUTTON_OFFSET.x, MOVE_TO_BLUE_TEAM_BUTTON_OFFSET.y - MOVE_TEAM_BUTTON_SIZE.y / 2 - GAME_ROOM_PLAYER_NAME_TEXT_SIZE.y / 2 - 30);
	for (size_t i = 0; i < MAX_PLAYERS_PER_TEAM; ++i)
	{
		UIObjectHandle hTextGameRoomRedTeamPlayer = CreateText();
		pScriptLobbyHandler->m_hTextGameRoomRedTeamPlayers[i] = hTextGameRoomRedTeamPlayer;
		Text* pTextGameRoomRedTeamPlayer = static_cast<Text*>(hTextGameRoomRedTeamPlayer.ToPtr());
		pTextGameRoomRedTeamPlayer->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
		pTextGameRoomRedTeamPlayer->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameRoomRedTeamPlayer->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameRoomRedTeamPlayer->m_transform.SetPosition(FIRST_GAME_ROOM_RED_TEAM_PLAYER_NAME_TEXT_OFFSET.x, FIRST_GAME_ROOM_RED_TEAM_PLAYER_NAME_TEXT_OFFSET.y - i * (GAME_ROOM_PLAYER_NAME_TEXT_SIZE.y + 20));
		pTextGameRoomRedTeamPlayer->SetColor(Colors::WhiteSmoke);
		pTextGameRoomRedTeamPlayer->SetText(L"Hello06");
		pTextGameRoomRedTeamPlayer->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pTextGameRoomRedTeamPlayer->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameRoomRedTeamPlayer->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pTextGameRoomRedTeamPlayer->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pTextGameRoomRedTeamPlayer->ApplyTextFormat();
		pTextGameRoomRedTeamPlayer->SetSize(GAME_ROOM_PLAYER_NAME_TEXT_SIZE);

		UIObjectHandle hTextGameRoomBlueTeamPlayer = CreateText();
		pScriptLobbyHandler->m_hTextGameRoomBlueTeamPlayers[i] = hTextGameRoomBlueTeamPlayer;
		Text* pTextGameRoomBlueTeamPlayer = static_cast<Text*>(hTextGameRoomBlueTeamPlayer.ToPtr());
		pTextGameRoomBlueTeamPlayer->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
		pTextGameRoomBlueTeamPlayer->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pTextGameRoomBlueTeamPlayer->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pTextGameRoomBlueTeamPlayer->m_transform.SetPosition(FIRST_GAME_ROOM_BLUE_TEAM_PLAYER_NAME_TEXT_OFFSET.x, FIRST_GAME_ROOM_BLUE_TEAM_PLAYER_NAME_TEXT_OFFSET.y - i * (GAME_ROOM_PLAYER_NAME_TEXT_SIZE.y + 20));
		pTextGameRoomBlueTeamPlayer->SetColor(Colors::WhiteSmoke);
		pTextGameRoomBlueTeamPlayer->SetText(L"Hello06");
		pTextGameRoomBlueTeamPlayer->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pTextGameRoomBlueTeamPlayer->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextGameRoomBlueTeamPlayer->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pTextGameRoomBlueTeamPlayer->GetTextFormat().SetSize(MEDIUM_TEXT_SIZE);
		pTextGameRoomBlueTeamPlayer->ApplyTextFormat();
		pTextGameRoomBlueTeamPlayer->SetSize(GAME_ROOM_PLAYER_NAME_TEXT_SIZE);
	}

	constexpr XMFLOAT2 EXIT_GAME_ROOM_BUTTON_SIZE(100, 26);
	constexpr XMFLOAT2 EXIT_GAME_ROOM_BUTTON_OFFSET(GAME_ROOM_PANEL_SIZE.x / 2 - EXIT_GAME_ROOM_BUTTON_SIZE.x / 2 - 15 + GAME_ROOM_PANEL_OFFSET.x, -GAME_ROOM_PANEL_SIZE.y / 2 + EXIT_GAME_ROOM_BUTTON_SIZE.y / 2 + 15 + GAME_ROOM_PANEL_OFFSET.y);
	UIObjectHandle hButtonExitGameRoom = CreateButton();
	Button* pButtonExitGameRoom = static_cast<Button*>(hButtonExitGameRoom.ToPtr());
	pButtonExitGameRoom->m_transform.SetParent(&pPanelGameRoomRoot->m_transform);
	pButtonExitGameRoom->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonExitGameRoom->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonExitGameRoom->m_transform.SetPosition(EXIT_GAME_ROOM_BUTTON_OFFSET);
	pButtonExitGameRoom->SetSize(EXIT_GAME_ROOM_BUTTON_SIZE);
	pButtonExitGameRoom->SetButtonColor(Colors::OrangeRed);
	pButtonExitGameRoom->SetTextColor(Colors::Black);
	pButtonExitGameRoom->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonExitGameRoom->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonExitGameRoom->SetText(L"나가기");
	pButtonExitGameRoom->GetTextFormat().SetSize(STATIC_TEXT_SIZE_MEDIUM);
	pButtonExitGameRoom->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonExitGameRoom->ApplyTextFormat();
	pButtonExitGameRoom->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickExitGameRoom));
}
