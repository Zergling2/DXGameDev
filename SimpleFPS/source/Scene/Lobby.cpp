#include "Lobby.h"
#include "../Script/GameResources.h"
#include "../Script/LobbyHandler.h"


using namespace ze;

ZE_IMPLEMENT_SCENE(Lobby);

static bool g_gameResourcesAndLobbyHandler = false;
const wchar_t* GO_GAME_RESOURCES_NAME = L"_$GameResources";
const wchar_t* GO_LOBBY_HANDLER_NAME = L"_$LobbyHandler";

const wchar_t* BTN_TEXT_CREATE_ACCOUNT = L"계정 생성";
const wchar_t* BTN_TEXT_LOGIN = L"로그인";
const wchar_t* BTN_TEXT_EXIT = L"나가기";

void Lobby::OnLoadScene()
{
	constexpr uint32_t STATIC_TEXT_SIZE = 14u;
	constexpr uint32_t INPUT_TEXT_SIZE = 12u;
	constexpr uint32_t BUTTON_TEXT_SIZE = 12u;

	ComponentHandle<GameResources> hScriptGameResources;
	ComponentHandle<LobbyHandler> hScriptLobbyHandler;

	if (!g_gameResourcesAndLobbyHandler)
	{
		// 1. 게임 리소스 매니저 게임오브젝트 생성
		GameObjectHandle hGameObjectGameResources = CreateGameObject(GO_GAME_RESOURCES_NAME);
		GameObject* pGameObjectGameResources = hGameObjectGameResources.ToPtr();
		pGameObjectGameResources->DontDestroyOnLoad();	// DontDestroyOnLoad
		hScriptGameResources = pGameObjectGameResources->AddComponent<GameResources>();
		GameResources* pScriptGameResources = hScriptGameResources.ToPtr();

		pScriptGameResources->m_texLoginBgr = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\login_bgr.png");
		pScriptGameResources->m_texGameListBgr = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\gamelist_bgr.png");

		// 2. 로비 핸들러 게임오브젝트 생성
		GameObjectHandle hGameObjectLobbyHandler = CreateGameObject(GO_LOBBY_HANDLER_NAME);
		GameObject* pGameObjectLobbyHandler = hGameObjectLobbyHandler.ToPtr();
		pGameObjectLobbyHandler->DontDestroyOnLoad();	// DontDestroyOnLoad
		hScriptLobbyHandler = pGameObjectLobbyHandler->AddComponent<LobbyHandler>();
		LobbyHandler* pScriptLobbyHandler = hScriptLobbyHandler.ToPtr();
		pScriptLobbyHandler->m_hScriptGameResources = hScriptGameResources;


		g_gameResourcesAndLobbyHandler = true;
	}
	else
	{
		// 1. 게임 리소스 매니저 핸들 획득
		GameObjectHandle hGameObjectGameResources = GameObject::Find(GO_GAME_RESOURCES_NAME);
		GameObject* pGameObjectGameResources = hGameObjectGameResources.ToPtr();
		assert(pGameObjectGameResources);
		hScriptGameResources = pGameObjectGameResources->GetComponent<GameResources>();

		// 2. 로비 핸들러 핸들 획득
		GameObjectHandle hGameObjectLobbyHandler = GameObject::Find(GO_LOBBY_HANDLER_NAME);
		GameObject* pGameObjectLobbyHandler = hGameObjectLobbyHandler.ToPtr();
		assert(pGameObjectLobbyHandler);
		hScriptLobbyHandler = pGameObjectLobbyHandler->GetComponent<LobbyHandler>();
	}

	GameResources* pScriptGameResources = hScriptGameResources.ToPtr();
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
	pPanelLoginWindowRoot->SetColor(Colors::DimGray);
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
	pInputFieldId->GetTextFormat().SetSize(INPUT_TEXT_SIZE);
	pInputFieldId->ApplyTextFormat();
	pInputFieldId->SetSize(IDPW_INPUT_FIELD_SIZE);
	pInputFieldId->SetBkColor(Colors::DimGray);
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
	pInputFieldPw->GetTextFormat().SetSize(INPUT_TEXT_SIZE);
	pInputFieldPw->ApplyTextFormat();
	pInputFieldPw->SetSize(IDPW_INPUT_FIELD_SIZE);
	pInputFieldPw->SetBkColor(Colors::DimGray);
	pInputFieldPw->SetTextColor(Colors::Black);
	pInputFieldPw->AllowReturn(false);
	pInputFieldPw->AllowSpace(false);
	pInputFieldPw->SetPassword(true);
	pInputFieldPw->SetMaxChar(16);


	constexpr XMFLOAT2 BUTTON_SIZE = XMFLOAT2(80, 24);
	UIObjectHandle hButtonCreateAccount = CreateButton();
	Button* pButtonCreateAccount = static_cast<Button*>(hButtonCreateAccount.ToPtr());
	pButtonCreateAccount->m_transform.SetParent(&pPanelLoginWindowRoot->m_transform);
	pButtonCreateAccount->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonCreateAccount->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonCreateAccount->m_transform.SetPosition(-120, -56);
	pButtonCreateAccount->SetSize(BUTTON_SIZE);
	pButtonCreateAccount->SetButtonColor(ColorsLinear::Orange);
	pButtonCreateAccount->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonCreateAccount->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonCreateAccount->SetText(BTN_TEXT_CREATE_ACCOUNT);
	pButtonCreateAccount->SetTextColor(ColorsLinear::Black);
	pButtonCreateAccount->GetTextFormat().SetSize(BUTTON_TEXT_SIZE);
	pButtonCreateAccount->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonCreateAccount->ApplyTextFormat();


	UIObjectHandle hButtonLogin = CreateButton();
	Button* pButtonLogin = static_cast<Button*>(hButtonLogin.ToPtr());
	pButtonLogin->m_transform.SetParent(&pPanelLoginWindowRoot->m_transform);
	pButtonLogin->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonLogin->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonLogin->m_transform.SetPosition(30, -56);
	pButtonLogin->SetSize(BUTTON_SIZE);
	// pButtonLogin->SetButtonColor(Colors::Gray);
	pButtonLogin->SetTextColor(Colors::Gold);
	pButtonLogin->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonLogin->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonLogin->SetText(BTN_TEXT_LOGIN);
	pButtonLogin->GetTextFormat().SetSize(BUTTON_TEXT_SIZE);
	pButtonLogin->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonLogin->ApplyTextFormat();
	pButtonLogin->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickLogin));


	UIObjectHandle hButtonExit = CreateButton();
	Button* pButtonExit = static_cast<Button*>(hButtonExit.ToPtr());
	pButtonExit->m_transform.SetParent(&pPanelLoginWindowRoot->m_transform);
	pButtonExit->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pButtonExit->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pButtonExit->m_transform.SetPosition(120, -56);
	pButtonExit->SetSize(BUTTON_SIZE);
	pButtonExit->SetButtonColor(0.2f, 0.2f, 0.2f, 1.0f);
	pButtonExit->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pButtonExit->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pButtonExit->SetText(BTN_TEXT_EXIT);
	pButtonExit->GetTextFormat().SetSize(BUTTON_TEXT_SIZE);
	pButtonExit->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
	pButtonExit->ApplyTextFormat();
	pButtonExit->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickExitGame));




	// #
	// Game list browser UI
	UIObjectHandle hPanelGameListBrowserRoot = CreatePanel();
	pScriptLobbyHandler->m_hPanelGameListBrowserRoot = hPanelGameListBrowserRoot;
	Panel* pPanelGameListBrowserRoot = static_cast<Panel*>(hPanelGameListBrowserRoot.ToPtr());
	pPanelGameListBrowserRoot->m_transform.SetParent(&pImageLobbyBgr->m_transform);
	pPanelGameListBrowserRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
	pPanelGameListBrowserRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
	pPanelGameListBrowserRoot->m_transform.SetPosition(0, 0);
	pPanelGameListBrowserRoot->SetSize(1366, 768);
	pPanelGameListBrowserRoot->SetColor(XMVectorSetW(Colors::DimGray, 0.25f));
	pPanelGameListBrowserRoot->SetShape(PanelShape::Rectangle);
	// pPanelGameListBrowserRoot->SetRadius(4.0f, 4.0f);




	pScriptLobbyHandler->RequestUIUpdate();
}
