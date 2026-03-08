#include "Lobby.h"
#include "../Script/LobbyHandler.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(Lobby);

void Lobby::OnLoadScene()
{
	constexpr uint32_t STATIC_TEXT_SIZE = 14u;
	constexpr uint32_t INPUT_TEXT_SIZE = 12u;
	constexpr uint32_t BUTTON_TEXT_SIZE = 12u;

	{
		GameObjectHandle hGameObjectLobbyHandler = CreateGameObject();
		GameObject* pGameObjectLobbyHandler = hGameObjectLobbyHandler.ToPtr();
		ComponentHandle<LobbyHandler> hScriptLobbyHandler = pGameObjectLobbyHandler->AddComponent<LobbyHandler>();
		LobbyHandler* pScriptLobbyHandler = hScriptLobbyHandler.ToPtr();


		UIObjectHandle hImageLoginBackground = CreateImage();
		pScriptLobbyHandler->m_hImageLoginBackground = hImageLoginBackground;
		Image* pImageLoginBackground = static_cast<Image*>(hImageLoginBackground.ToPtr());
		pImageLoginBackground->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pImageLoginBackground->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pImageLoginBackground->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\login_bgr.png"));
		pImageLoginBackground->SetNativeSize(true);


		UIObjectHandle hPanelLoginWindowRoot = CreatePanel();
		pScriptLobbyHandler->m_hPanelLoginWindowRoot = hPanelLoginWindowRoot;
		Panel* pPanelLoginWindowRoot = static_cast<Panel*>(hPanelLoginWindowRoot.ToPtr());
		pPanelLoginWindowRoot->m_transform.SetParent(&pImageLoginBackground->m_transform);
		pPanelLoginWindowRoot->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pPanelLoginWindowRoot->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
		pPanelLoginWindowRoot->m_transform.SetPosition(0, 0);
		pPanelLoginWindowRoot->SetSize(360, 160);
		pPanelLoginWindowRoot->SetColor(Colors::DarkOliveGreen);
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
		pTextId->SetSize(XMFLOAT2(40, 30));


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
		pTextPw->SetSize(XMFLOAT2(100, 30));


		constexpr float ID_INPUT_FIELD_WIDTH = 210;
		constexpr float ID_INPUT_FIELD_HEIGHT = 26;
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
		pInputFieldId->SetSize(ID_INPUT_FIELD_WIDTH, ID_INPUT_FIELD_HEIGHT);
		pInputFieldId->SetBkColor(Colors::DimGray);
		pInputFieldId->SetTextColor(Colors::Black);
		pInputFieldId->AllowReturn(false);
		pInputFieldId->AllowSpace(false);
		pInputFieldId->SetMaxChar(16);


		constexpr float PW_INPUT_FIELD_WIDTH = 210;
		constexpr float PW_INPUT_FIELD_HEIGHT = 26;
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
		pInputFieldPw->SetSize(PW_INPUT_FIELD_WIDTH, PW_INPUT_FIELD_HEIGHT);
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
		pButtonCreateAccount->SetText(L"계정 생성");
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
		pButtonLogin->SetText(L"로그인");
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
		pButtonExit->SetText(L"나가기");
		pButtonExit->GetTextFormat().SetSize(BUTTON_TEXT_SIZE);
		pButtonExit->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_NORMAL);
		pButtonExit->ApplyTextFormat();
		pButtonExit->SetHandlerOnClick(MakeUIHandler(hScriptLobbyHandler, &LobbyHandler::OnClickExitGame));

	}
}
