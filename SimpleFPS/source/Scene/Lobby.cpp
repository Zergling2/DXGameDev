#include "Lobby.h"
#include "..\Script\FirstPersonMovement.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(Lobby);

void Lobby::OnLoadScene()
{
	{
		UIObjectHandle hBgrImg = CreateImage();
		Image* pBgrImg = static_cast<Image*>(hBgrImg.ToPtr());
		pBgrImg->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\lobby_bgr.jpg"));
		pBgrImg->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pBgrImg->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pBgrImg->SetSize(XMFLOAT2(1366, 768));
		pBgrImg->SetNativeSize(false);

		UIObjectHandle hPanel = CreatePanel();
		Panel* pPanel = static_cast<Panel*>(hPanel.ToPtr());
		pPanel->SetSize(XMFLOAT2(400, 600));
		pPanel->SetColor(XMVectorSet(0.0f, 0.5f, 0.25f, 0.5f));
		pPanel->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pPanel->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pPanel->m_transform.m_position.x = 0.0f;
		pPanel->m_transform.m_position.y = 0.0f;
		pPanel->SetShape(PANEL_SHAPE::ROUNDED_RECTANGLE);
		pPanel->m_transform.SetParent(&pBgrImg->m_transform);


		UIObjectHandle hTitleText = CreateText();
		Text* pTitleText = static_cast<Text*>(hTitleText.ToPtr());
		pTitleText->SetText(L"SIMPLE FPS GAME");
		pTitleText->SetSize(XMFLOAT2(160, 30));
		pTitleText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pTitleText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pTitleText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTitleText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTitleText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_HEAVY);
		pTitleText->GetTextFormat().SetSize(32);
		pTitleText->Apply();
		pTitleText->m_transform.m_position.x = +0.0f;
		pTitleText->m_transform.m_position.y = +160.0f;


		UIObjectHandle hButtonGameStart = CreateButton();
		Button* pButtonGameStart = static_cast<Button*>(hButtonGameStart.ToPtr());
		pButtonGameStart->SetSize(XMFLOAT2(160, 30));
		pButtonGameStart->SetButtonColor(XMVectorSet(0.25f, 0.75f, 0.25f, 1.0f));
		pButtonGameStart->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pButtonGameStart->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pButtonGameStart->m_transform.m_position.x = +0.0f;
		pButtonGameStart->m_transform.m_position.y = +40.0f;
		pButtonGameStart->SetText(L"게임 시작");
		pButtonGameStart->SetColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		pButtonGameStart->GetTextFormat().SetSize(20);
		pButtonGameStart->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_BOLD);
		pButtonGameStart->Apply();
		pButtonGameStart->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pButtonGameStart->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pButtonGameStart->m_transform.SetParent(&pPanel->m_transform);


		UIObjectHandle hButtonGameExit = CreateButton();
		Button* pButtonGameExit = static_cast<Button*>(hButtonGameExit.ToPtr());
		pButtonGameExit->SetSize(XMFLOAT2(160, 30));
		pButtonGameExit->SetButtonColor(XMVectorSet(0.75f, 0.25f, 0.25f, 1.0f));
		pButtonGameExit->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pButtonGameExit->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pButtonGameExit->m_transform.m_position.x = +0.0f;
		pButtonGameExit->m_transform.m_position.y = -40.0f;
		pButtonGameExit->SetText(L"게임 종료");
		pButtonGameExit->SetColor(XMFLOAT4(0.0f, 0.05f, 0.0f, 1.0f));
		pButtonGameExit->GetTextFormat().SetSize(20);
		pButtonGameExit->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_BOLD);
		pButtonGameExit->Apply();
		pButtonGameExit->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pButtonGameExit->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pButtonGameExit->m_transform.SetParent(&pPanel->m_transform);

		UIObjectHandle hInputField0 = CreateInputField();
		InputField* pInputField0 = static_cast<InputField*>(hInputField0.ToPtr());
		pInputField0->SetSize(XMFLOAT2(200, 26));
		pInputField0->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pInputField0->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pInputField0->m_transform.m_position.x = -40.0f;
		pInputField0->m_transform.m_position.y = -100.0f;
		pInputField0->SetShape(INPUT_FIELD_SHAPE::ROUNDED_RECTANGLE);
		pInputField0->SetRadiusX(8.0f);
		pInputField0->SetRadiusY(8.0f);
		pInputField0->SetBkColor(ColorsLinear::LightGray);
		pInputField0->SetColor(ColorsLinear::Blue);
		pInputField0->GetTextFormat().SetSize(16);
		pInputField0->Apply();
		pInputField0->m_transform.SetParent(&pPanel->m_transform);

		UIObjectHandle hInputField1 = CreateInputField();
		InputField* pInputField1 = static_cast<InputField*>(hInputField1.ToPtr());
		pInputField1->SetSize(XMFLOAT2(200, 26));
		pInputField1->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pInputField1->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pInputField1->m_transform.m_position.x = -40.0f;
		pInputField1->m_transform.m_position.y = -140.0f;
		pInputField1->SetShape(INPUT_FIELD_SHAPE::ROUNDED_RECTANGLE);
		pInputField1->SetRadiusX(8.0f);
		pInputField1->SetRadiusY(8.0f);
		pInputField1->SetBkColor(ColorsLinear::Yellow);
		pInputField1->SetColor(ColorsLinear::Black);
		pInputField1->GetTextFormat().SetSize(14);
		pInputField1->SetPassword(true);
		pInputField1->Apply();
		pInputField1->m_transform.SetParent(&pPanel->m_transform);
	}

	{
		GameObjectHandle hMainCamera = CreateGameObject(L"Main Camera");
		GameObject* pMainCamera = hMainCamera.ToPtr();
		pMainCamera->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f));
		ComponentHandle<Camera> hMainCameraComponent = pMainCamera->AddComponent<Camera>();	// 카메라 컴포넌트 추가
		Camera* pMainCameraComponent = hMainCameraComponent.ToPtr();
		pMainCameraComponent->SetBackgroundColor(ColorsLinear::Gray);
		pMainCameraComponent->SetDepth(0);
		pMainCameraComponent->SetFieldOfView(92);
		pMainCameraComponent->SetClippingPlanes(0.3f, 500.0f);
		ComponentHandle<FirstPersonMovement> hFPSCam = pMainCamera->AddComponent<FirstPersonMovement>();	// 1인칭 카메라 조작
	}
}
