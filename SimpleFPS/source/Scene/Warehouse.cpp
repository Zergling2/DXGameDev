#include "Warehouse.h"
#include "..\Script\FirstPersonMovement.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(Warehouse);

void Warehouse::OnLoadScene()
{
	// Lights
	// {
	// 	GameObjectHandle hSun = CreateGameObject(L"Sun");
	// 	GameObject* pSun = hSun.ToPtr();
	// 	pSun->m_transform.SetRotationEuler(XMVectorSet(XMConvertToRadians(45), XMConvertToRadians(150), 0.0f, 0.0f));
	// 	ComponentHandle<DirectionalLight> hLight = pSun->AddComponent<DirectionalLight>();
	// 	DirectionalLight* pLight = hLight.ToPtr();
	// 	XMStoreFloat4A(&pLight->m_ambient, XMVectorScale(Vector3::One(), 0.1f));
	// 	XMStoreFloat4A(&pLight->m_diffuse, XMVectorScale(Vector3::One(), 0.6f));
	// 	XMStoreFloat4A(&pLight->m_specular, Vector3::One());
	// }

	GameObjectHandle hFlashLight = CreateGameObject(L"손전등");
	{
		GameObject* pFL = hFlashLight.ToPtr();
		pFL->m_transform.SetPosition(XMVectorSet(0.0f, 1.0f, 1.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMR = pFL->AddComponent<MeshRenderer>();
		auto mesh = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\flash.obj").m_staticMeshes[0];
		hMR.ToPtr()->SetMesh(mesh);

		ComponentHandle<SpotLight> hSpotLight = pFL->AddComponent<SpotLight>();
		SpotLight* pSpotLight = hSpotLight.ToPtr();
		XMStoreFloat4A(&pSpotLight->m_diffuse, XMVectorScale(ColorsLinear::White, 1.0f));
		XMStoreFloat4A(&pSpotLight->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 1.0f), 1.0f));
		pSpotLight->SetDistAtt(0.0f, 1.0f, 0.0f);
		pSpotLight->SetRange(50.0f);

		ComponentHandle<PointLight> hPointLight = pFL->AddComponent<PointLight>();
		PointLight* pPointLight = hPointLight.ToPtr();
		XMStoreFloat4A(&pPointLight->m_diffuse, XMVectorScale(ColorsLinear::GreenYellow, 0.6f));
		XMStoreFloat4A(&pPointLight->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::GreenYellow, 0.2f), 2.0f));
		pPointLight->SetDistAtt(0.0f, 1.0f, 0.0f);
		pPointLight->SetRange(3.0f);
	}

	// Main Camera
	GameObject* pMainCamera = nullptr;
	ComponentHandle<FirstPersonMovement> hScriptFPSMovement;
	FirstPersonMovement* pScriptFPSMovement = nullptr;
	{
		GameObjectHandle hMainCamera = CreateGameObject(L"Main Camera");

		pMainCamera = hMainCamera.ToPtr();
		pMainCamera->m_transform.SetPosition(XMVectorSet(0.0f, 1.6f, -5.0f, 0.0f));

		ComponentHandle<Camera> hCameraComponent = pMainCamera->AddComponent<Camera>();
		Camera* pCameraComponent = hCameraComponent.ToPtr();
		pCameraComponent->SetDepth(0);
		pCameraComponent->SetFieldOfView(82);
		pCameraComponent->SetClippingPlanes(0.1f, 300.0f);

		hScriptFPSMovement = pMainCamera->AddComponent<FirstPersonMovement>();		// 1인칭 카메라 조작
		pScriptFPSMovement = hScriptFPSMovement.ToPtr();


		// ####################
		// 스크립트 멤버 대입
		pScriptFPSMovement->m_hComponentCamera = hCameraComponent;
	}


	std::shared_ptr<Material> treeBillboardMtl = ResourceLoader::GetInstance()->CreateMaterial();
	treeBillboardMtl->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\treebranchstylizedextraameixa.png");
	// 빌보드 테스트
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Spherical");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPositionY(1.25f);

		ComponentHandle<Billboard> hBillboard = pGameObject->AddComponent<Billboard>();
		Billboard* pBillboard = hBillboard.ToPtr();
		pBillboard->SetBillboardQuadWidth(4.0);
		pBillboard->SetBillboardType(BillboardType::Spherical);
		pBillboard->SetMaterial(treeBillboardMtl);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"CylindricalY");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(-3.0f, 2.0f, 0.0f);

		ComponentHandle<Billboard> hBillboard = pGameObject->AddComponent<Billboard>();
		Billboard* pBillboard = hBillboard.ToPtr();
		pBillboard->SetBillboardQuadHeight(4.0);
		pBillboard->SetBillboardType(BillboardType::CylindricalY);
		pBillboard->SetMaterial(treeBillboardMtl);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"ScreenAligned");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(1.0f, 2.0f, 3.0f);

		ComponentHandle<Billboard> hBillboard = pGameObject->AddComponent<Billboard>();
		Billboard* pBillboard = hBillboard.ToPtr();
		// pBillboard->SetBillboardQuadHeight(4.0);
		pBillboard->SetBillboardType(BillboardType::ScreenAligned);
		pBillboard->SetMaterial(treeBillboardMtl);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"ScreenAligned");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(1.0f, 2.0f, 5.0f);

		ComponentHandle<Billboard> hBillboard = pGameObject->AddComponent<Billboard>();
		Billboard* pBillboard = hBillboard.ToPtr();
		// pBillboard->SetBillboardQuadHeight(4.0);
		pBillboard->SetBillboardType(BillboardType::ScreenAligned);
		pBillboard->SetMaterial(treeBillboardMtl);
	}
	
	{
		// Adapter Info UI
		{
			UIObjectHandle hText = CreateText();
			Text* pText = static_cast<Text*>(hText.ToPtr());
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
			UIObjectHandle hText = CreateText();
			Text* pText = static_cast<Text*>(hText.ToPtr());
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
			UIObjectHandle hText = CreateText();
			Text* pText = static_cast<Text*>(hText.ToPtr());
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
			UIObjectHandle hText = CreateText();
			Text* pText = static_cast<Text*>(hText.ToPtr());
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
	}

	// UI
	{
		UIObjectHandle hCrosshair = CreateImage();
		Image* pCrosshair = static_cast<Image*>(hCrosshair.ToPtr());
		pCrosshair->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\crosshair.dds"));
		pCrosshair->SetNativeSize(true);
		pCrosshair->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pCrosshair->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);

		UIObjectHandle hHealthBgr = CreateImage();
		Image* pHealthBgr = static_cast<Image*>(hHealthBgr.ToPtr());
		pHealthBgr->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\health.png"));
		pHealthBgr->SetNativeSize(true);
		pHealthBgr->m_transform.SetPosition(pHealthBgr->GetHalfSizeX() + 4, pHealthBgr->GetHalfSizeY() + 4);
		pHealthBgr->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pHealthBgr->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);

		UIObjectHandle hWeaponIndicatorBgr = CreateImage();
		Image* pWeaponIndicatorBgr = static_cast<Image*>(hWeaponIndicatorBgr.ToPtr());
		pWeaponIndicatorBgr->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapon_indicator.png"));
		pWeaponIndicatorBgr->SetNativeSize(true);
		pWeaponIndicatorBgr->m_transform.SetPosition(-pWeaponIndicatorBgr->GetHalfSizeX() - 4, pWeaponIndicatorBgr->GetHalfSizeY() + 4);
		pWeaponIndicatorBgr->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
		pWeaponIndicatorBgr->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);

		{
			UIObjectHandle hHPText = CreateText();
			Text* pHPText = static_cast<Text*>(hHPText.ToPtr());
			pHPText->SetText(L"100");
			pHPText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
			pHPText->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
			pHPText->m_transform.SetPosition(pHealthBgr->m_transform.GetPosition());
			pHPText->m_transform.TranslateX(-56);
			pHPText->SetSize(XMFLOAT2(128, 48));
			pHPText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pHPText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pHPText->SetColor(ColorsLinear::White);
			pHPText->GetTextFormat().SetSize(28);
			pHPText->GetTextFormat().SetFontFamilyName(L"Impact");
			pHPText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
			pHPText->ApplyTextFormat();
		}

		{
			UIObjectHandle hAPText = CreateText();
			Text* pAPText = static_cast<Text*>(hAPText.ToPtr());
			pAPText->SetText(L"100");
			pAPText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
			pAPText->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
			pAPText->m_transform.SetPosition(pHealthBgr->m_transform.GetPosition());
			pAPText->m_transform.TranslateX(120);
			pAPText->SetSize(XMFLOAT2(128, 48));
			pAPText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pAPText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pAPText->SetColor(ColorsLinear::White);
			pAPText->GetTextFormat().SetSize(28);
			pAPText->GetTextFormat().SetFontFamilyName(L"Impact");
			pAPText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
			pAPText->ApplyTextFormat();
		}

		{
			UIObjectHandle hPointText = CreateText();
			Text* pPointText = static_cast<Text*>(hPointText.ToPtr());
			pPointText->SetText(L"126P");
			pPointText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
			pPointText->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
			pPointText->m_transform.SetPosition(pWeaponIndicatorBgr->m_transform.GetPosition());
			pPointText->m_transform.Translate(-45, 34);
			pPointText->SetSize(XMFLOAT2(128, 32));
			pPointText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pPointText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pPointText->SetColor(ColorsLinear::White);
			pPointText->GetTextFormat().SetSize(22);
			pPointText->GetTextFormat().SetFontFamilyName(L"Agency FB");
			pPointText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
			pPointText->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
			pPointText->ApplyTextFormat();
		}

		{
			UIObjectHandle hWeaponNameText = CreateText();
			Text* pWeaponNameText = static_cast<Text*>(hWeaponNameText.ToPtr());
			pWeaponNameText->SetText(L"M16");
			pWeaponNameText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
			pWeaponNameText->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
			pWeaponNameText->m_transform.SetPosition(pWeaponIndicatorBgr->m_transform.GetPosition());
			pWeaponNameText->m_transform.TranslateY(6);
			pWeaponNameText->SetSize(XMFLOAT2(200, 32));
			pWeaponNameText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pWeaponNameText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pWeaponNameText->SetColor(ColorsLinear::White);
			pWeaponNameText->GetTextFormat().SetSize(22);
			pWeaponNameText->GetTextFormat().SetFontFamilyName(L"Agency FB");
			pWeaponNameText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
			pWeaponNameText->ApplyTextFormat();
		}

		{
			UIObjectHandle hWeaponNameText = CreateText();
			Text* pWeaponNameText = static_cast<Text*>(hWeaponNameText.ToPtr());
			pWeaponNameText->SetText(L"30 / 90");
			pWeaponNameText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
			pWeaponNameText->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
			pWeaponNameText->m_transform.SetPosition(pWeaponIndicatorBgr->m_transform.GetPosition());
			pWeaponNameText->m_transform.Translate(16, -28);
			pWeaponNameText->SetSize(XMFLOAT2(128, 40));
			pWeaponNameText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pWeaponNameText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pWeaponNameText->SetColor(ColorsLinear::White);
			pWeaponNameText->GetTextFormat().SetSize(34);
			pWeaponNameText->GetTextFormat().SetFontFamilyName(L"Agency FB");
			pWeaponNameText->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
			pWeaponNameText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
			pWeaponNameText->ApplyTextFormat();
		}

		// 무기교체 UI
		{
			constexpr FLOAT WEAPON_CHANGE_PANEL_WIDTH = 180.0f;
			constexpr FLOAT WEAPON_CHANGE_PANEL_HEIGHT = 220.0f;
			UIObjectHandle hPanel = CreatePanel();
			Panel* pPanel = static_cast<Panel*>(hPanel.ToPtr());
			// pPanel->SetShape(PanelShape::RoundedRectangle);		// Default
			pPanel->SetColor(ColorsLinear::DarkGray);
			pPanel->SetColorA(0.5f);
			pPanel->SetSize(WEAPON_CHANGE_PANEL_WIDTH, WEAPON_CHANGE_PANEL_HEIGHT);
			pPanel->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
			pPanel->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
			pPanel->m_transform.SetPositionX(pPanel->GetSizeX() / 2.0f + 5);


			pScriptFPSMovement->m_hWeaponChangePanel = hPanel;
			pPanel->SetActive(false);



			{
				UIObjectHandle hSlot1 = CreateImage();
				Image* pSlot1 = static_cast<Image*>(hSlot1.ToPtr());
				pSlot1->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\m16a1.png"));
				pSlot1->SetNativeSize(true);
				pSlot1->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
				pSlot1->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
				pSlot1->m_transform.SetPosition(-300, 80);

				UIObjectHandle hSlot2 = CreateImage();
				Image* pSlot2 = static_cast<Image*>(hSlot2.ToPtr());
				pSlot2->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\m4a1.png"));
				pSlot2->SetNativeSize(true);
				pSlot2->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
				pSlot2->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
				pSlot2->m_transform.SetPosition(0, 80);

				UIObjectHandle hSlot3 = CreateImage();
				Image* pSlot3 = static_cast<Image*>(hSlot3.ToPtr());
				pSlot3->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\m762.png"));
				pSlot3->SetNativeSize(true);
				pSlot3->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
				pSlot3->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
				pSlot3->m_transform.SetPosition(300, 80);
			}


			constexpr float BUTTON_WIDTH = 40;
			constexpr float BUTTON_HEIGHT = 18;
			const float WEAPON_NAME_TEXT_WIDTH = pPanel->GetSizeX() - BUTTON_WIDTH - 10;
			const float WEAPON_NAME_TEXT_HEIGHT = 20;
			const float BUTTON_POS_X = pPanel->m_transform.GetPositionX() + pPanel->GetHalfSizeX() - 5 - (BUTTON_WIDTH / 2.0f);
			const float WEAPON_NAME_TEXT_POS_X = pPanel->m_transform.GetPositionX() - pPanel->GetHalfSizeX() + 5 + (WEAPON_NAME_TEXT_WIDTH / 2.0f);
			{
				UIObjectHandle hInputField = CreateInputField();
				InputField* pInputField = static_cast<InputField*>(hInputField.ToPtr());
				pInputField->m_transform.SetParent(&pPanel->m_transform);

				pInputField->AllowReturn(true);
				pInputField->SetSize(pPanel->GetSizeX() - 16, 20);
				pInputField->SetText(L"테스트 입력 필드");
				pInputField->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pInputField->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pInputField->m_transform.SetPosition(
					pPanel->m_transform.GetPositionX(),
					-30
				);
			}

			{
				UIObjectHandle hSlider = CreateSliderControl();
				SliderControl* pSlider = static_cast<SliderControl*>(hSlider.ToPtr());
				pSlider->m_transform.SetParent(&pPanel->m_transform);
				
				pSlider->SetRange(0, 10);
				pSlider->SetThumbColor(ColorsLinear::Green);
				pSlider->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pSlider->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pSlider->m_transform.SetPosition(
					pPanel->m_transform.GetPositionX(),
					-60
				);

				pScriptFPSMovement->m_hSlider = hSlider;
				pSlider->SetHandlerOnPosChange(MakeUIHandler(hScriptFPSMovement, &FirstPersonMovement::EventHandlerAmbientChange));
			}

			{
				UIObjectHandle hSlider = CreateSliderControl();
				SliderControl* pSlider = static_cast<SliderControl*>(hSlider.ToPtr());
				pSlider->m_transform.SetParent(&pPanel->m_transform);

				pSlider->SetSliderControlType(SliderControlType::Vertical);
				pSlider->SetThumbSize(16, 8);
				pSlider->SetThumbColor(ColorsLinear::Red);
				pSlider->SetTrackColor(ColorsLinear::YellowGreen);
				pSlider->SetRange(0, 3);
				pSlider->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pSlider->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pSlider->m_transform.SetPosition(
					pPanel->m_transform.GetPositionX() + 200,
					-60
				);

				pScriptFPSMovement->m_hSliderMSAAChanger = hSlider;
				pSlider->SetHandlerOnPosChange(MakeUIHandler(hScriptFPSMovement, &FirstPersonMovement::EventHandlerMSAAChange));
			}

			{
				UIObjectHandle hRadioButton1 = CreateRadioButton();
				RadioButton* pRadioButton1 = static_cast<RadioButton*>(hRadioButton1.ToPtr());
				pRadioButton1->m_transform.SetParent(&pPanel->m_transform);

				pRadioButton1->SetHandlerOnClick(MakeUIHandler(hScriptFPSMovement, &FirstPersonMovement::TestHandlerOnClick01));
				pRadioButton1->SetText(L"Radio Button 1");
				pRadioButton1->SetButtonColorRGB(ColorsLinear::DarkOrange);
				pRadioButton1->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pRadioButton1->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pRadioButton1->m_transform.SetPosition(
					pPanel->m_transform.GetPositionX(),
					-120
				);

				UIObjectHandle hRadioButton2 = CreateRadioButton();
				RadioButton* pRadioButton2 = static_cast<RadioButton*>(hRadioButton2.ToPtr());
				pRadioButton2->m_transform.SetParent(&pPanel->m_transform);

				pRadioButton2->SetHandlerOnClick(MakeUIHandler(hScriptFPSMovement, &FirstPersonMovement::TestHandlerOnClick02));
				pRadioButton2->SetText(L"Radio Button 2");
				pRadioButton2->SetButtonColorRGB(ColorsLinear::CadetBlue);
				pRadioButton2->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pRadioButton2->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pRadioButton2->m_transform.SetPosition(
					pPanel->m_transform.GetPositionX(),
					-140
				);

				UIObjectHandle hRadioButton3 = CreateRadioButton();
				RadioButton* pRadioButton3 = static_cast<RadioButton*>(hRadioButton3.ToPtr());
				pRadioButton3->m_transform.SetParent(&pPanel->m_transform);

				pRadioButton3->SetHandlerOnClick(MakeUIHandler(hScriptFPSMovement, &FirstPersonMovement::TestHandlerOnClick03));
				pRadioButton3->SetText(L"Radio Button 3");
				pRadioButton3->SetButtonColorRGB(ColorsLinear::Gray);
				pRadioButton3->SetDotColorRGB(ColorsLinear::Red);
				pRadioButton3->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pRadioButton3->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pRadioButton3->m_transform.SetPosition(
					pPanel->m_transform.GetPositionX(),
					-160
				);


				// 라디오버튼 그룹 생성
				pRadioButton1->Join(pRadioButton2);
				pRadioButton1->Join(pRadioButton3);
			}

			{
				UIObjectHandle hCheckbox = CreateCheckbox();
				Checkbox* pCheckbox = static_cast<Checkbox*>(hCheckbox.ToPtr());
				pCheckbox->m_transform.SetParent(&pPanel->m_transform);

				pCheckbox->SetBoxColorA(0.9f);
				pCheckbox->SetBoxColorRGB(ColorsLinear::Red);
				pCheckbox->SetText(L"빨간 체크박스 (Right Text)");

				pCheckbox->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pCheckbox->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pCheckbox->m_transform.SetPosition(
					pPanel->m_transform.GetPositionX() - WEAPON_CHANGE_PANEL_WIDTH / 2.0f +
					pCheckbox->GetCheckboxHalfSizeX() + 5.0f,
					-80
				);
			}

			{
				UIObjectHandle hCheckbox = CreateCheckbox();
				Checkbox* pCheckbox = static_cast<Checkbox*>(hCheckbox.ToPtr());
				pCheckbox->m_transform.SetParent(&pPanel->m_transform);

				pCheckbox->SetLeftText();
				pCheckbox->SetBoxColorRGB(ColorsLinear::Blue);
				pCheckbox->SetCheckColor(ColorsLinear::White);
				pCheckbox->SetText(L"Left Text Checkbox");

				pCheckbox->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pCheckbox->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pCheckbox->m_transform.SetPosition(
					pPanel->m_transform.GetPositionX() + WEAPON_CHANGE_PANEL_WIDTH / 2.0f - pCheckbox->GetCheckboxHalfSizeX() - 5.0f,
					-100
				);
			}

			{
				UIObjectHandle hText = CreateText();
				Text* pText = static_cast<Text*>(hText.ToPtr());
				pText->m_transform.SetParent(&pPanel->m_transform);

				pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_BOLD);
				pText->ApplyTextFormat();
				pText->SetSize(pPanel->GetSizeX() - 10, 20);
				pText->SetColor(ColorsLinear::White);
				pText->SetText(L"무기교체 (단축키: N)");
				pText->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pText->m_transform.SetPosition(
					pPanel->m_transform.GetPositionX(),
					pPanel->m_transform.GetPositionY() + pPanel->GetHalfSizeY() - 5 - pText->GetHalfSizeY()
				);
			}

			{
				UIObjectHandle hText = CreateText();
				Text* pText = static_cast<Text*>(hText.ToPtr());
				pText->m_transform.SetParent(&pPanel->m_transform);

				pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				pText->SetSize(WEAPON_NAME_TEXT_WIDTH, WEAPON_NAME_TEXT_HEIGHT);
				pText->SetColor(ColorsLinear::WhiteSmoke);
				pText->SetText(L"1. B.92Fs Black");
				pText->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pText->m_transform.SetPosition(WEAPON_NAME_TEXT_POS_X, 70);

				UIObjectHandle hBtn = CreateButton();
				Button* pButton = static_cast<Button*>(hBtn.ToPtr());
				pButton->m_transform.SetParent(&pPanel->m_transform);

				pButton->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);
				pButton->SetTextColor(ColorsLinear::WhiteSmoke);
				pButton->SetButtonColor(ColorsLinear::Chocolate);
				pButton->SetText(L"교체");
				pButton->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pButton->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pButton->m_transform.SetPosition(BUTTON_POS_X, 70);
			}

			{
				UIObjectHandle hText = CreateText();
				Text* pText = static_cast<Text*>(hText.ToPtr());
				pText->m_transform.SetParent(&pPanel->m_transform);

				pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				pText->SetSize(WEAPON_NAME_TEXT_WIDTH, WEAPON_NAME_TEXT_HEIGHT);
				pText->SetColor(ColorsLinear::WhiteSmoke);
				pText->SetText(L"2. M4A1 Carbine");
				pText->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pText->m_transform.SetPosition(WEAPON_NAME_TEXT_POS_X, 50);

				UIObjectHandle hBtn = CreateButton();
				Button* pButton = static_cast<Button*>(hBtn.ToPtr());
				pButton->m_transform.SetParent(&pPanel->m_transform);

				pButton->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);
				pButton->SetTextColor(ColorsLinear::Black);
				pButton->SetButtonColor(ColorsLinear::Orange);
				pButton->SetText(L"교체");
				pButton->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pButton->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pButton->m_transform.SetPosition(BUTTON_POS_X, 50);
			}

			{
				UIObjectHandle hText = CreateText();
				Text* pText = static_cast<Text*>(hText.ToPtr());
				pText->m_transform.SetParent(&pPanel->m_transform);

				pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				pText->SetSize(WEAPON_NAME_TEXT_WIDTH, WEAPON_NAME_TEXT_HEIGHT);
				pText->SetColor(ColorsLinear::WhiteSmoke);
				pText->SetText(L"3. M16");
				pText->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pText->m_transform.SetPosition(WEAPON_NAME_TEXT_POS_X, 30);

				UIObjectHandle hBtn = CreateButton();
				Button* pButton = static_cast<Button*>(hBtn.ToPtr());
				pButton->m_transform.SetParent(&pPanel->m_transform);

				pButton->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);
				pButton->SetTextColor(ColorsLinear::WhiteSmoke);
				pButton->SetButtonColor(ColorsLinear::DarkOliveGreen);
				pButton->SetText(L"교체");
				pButton->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);
				pButton->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
				pButton->m_transform.SetPosition(BUTTON_POS_X, 30);
			}
		}
	}


	// Resources
	std::shared_ptr<StaticMesh> meshClosedContainer = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\ClosedContainer\\ClosedContainer.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshClosedLongContainer = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\ClosedContainer\\ClosedLongContainer.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshOpenContainer1 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\OpenContainer\\OpenContainer1.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshOpenContainer2 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\OpenContainer\\OpenContainer2.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseFrame = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\HouseFrame\\HouseFrame.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseSideWall = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\HouseSideWall.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseFloor = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\HouseFloor\\HouseFloor.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshBox = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\box\\box.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseRedSideWall = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\HouseRedSideWall.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseBlueSideWall = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\HouseBlueSideWall.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshBlueTeamBase = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\BlueTeamBase.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshRedTeamBase = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\RedTeamBase.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseRoof = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\HouseRoof\\HouseRoof.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshDoorFrame = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\DoorFrame\\DoorFrame.obj").m_staticMeshes[0];

	std::shared_ptr<Material> matAsphault = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matAsphault->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.8f), 1.0f));
	XMStoreFloat4A(&matAsphault->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matAsphault->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\asphault\\AsphaultStreet_d.tga");
	matAsphault->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\asphault\\AsphaultStreet_n.tga");


	std::shared_ptr<Material> matBambooWoodSemigloss = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matBambooWoodSemigloss->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.4f), 1.0f));
	XMStoreFloat4A(&matBambooWoodSemigloss->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matBambooWoodSemigloss->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\wood\\bamboo-wood-semigloss-diffuse.png");
	matBambooWoodSemigloss->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\wood\\bamboo-wood-semigloss-normal.png");


	std::shared_ptr<Material> matBrick22 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matBrick22->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.75f), 1.0f));
	XMStoreFloat4A(&matBrick22->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matBrick22->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\Brick 22 - 256x256.png");


	std::shared_ptr<Material> matRoof24 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matRoof24->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.75f), 1.0f));
	XMStoreFloat4A(&matRoof24->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matRoof24->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\roof\\Roof 24 - 256x256.png");


	std::shared_ptr<Material> matClosedContainer = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matClosedContainer->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.7f), 1.0f));
	XMStoreFloat4A(&matClosedContainer->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
	matClosedContainer->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\ClosedContainer\\Diffuse.png");


	std::shared_ptr<Material> matOpenContainer = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matOpenContainer->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.7f), 1.0f));
	XMStoreFloat4A(&matOpenContainer->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
	matOpenContainer->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\OpenContainer\\Diffuse.png");


	std::shared_ptr<Material> matHouseFrame = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matHouseFrame->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::Brown, 0.4f), 1.0f));
	XMStoreFloat4A(&matHouseFrame->m_specular, XMVectorScale(ColorsLinear::White, 0.25f));
	matHouseFrame->m_specular.w = 8.0f;

	std::shared_ptr<Material> matWoodenBox = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matWoodenBox->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
	XMStoreFloat4A(&matWoodenBox->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matWoodenBox->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\box\\WoodenBox.png");


	std::shared_ptr<Material> matPaperBox = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matPaperBox->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.7f), 1.0f));
	XMStoreFloat4A(&matPaperBox->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matPaperBox->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\box\\PaperBox.png");


	// std::shared_ptr<Material> matRustedSteelHotspot = ResourceLoader::GetInstance()->CreateMaterial();
	// XMStoreFloat4A(&matRustedSteelHotspot->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.75f), 1.0f));
	// XMStoreFloat4A(&matRustedSteelHotspot->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.5f), 1.0f));
	// matRustedSteelHotspot->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metals\\RustedSteel\\RustedSteel_Diffuse.png");
	// matRustedSteelHotspot->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metals\\RustedSteel\\RustedSteel_Normal.png");


	std::shared_ptr<Material> matConcrete3 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matConcrete3->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
	XMStoreFloat4A(&matConcrete3->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	matConcrete3->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\concrete\\concrete3_diffuse.png");
	matConcrete3->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\concrete\\concrete3_normal.png");

	std::shared_ptr<Material> matSprayedWall1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matSprayedWall1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.5f), 1.0f));
	XMStoreFloat4A(&matSprayedWall1->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	matSprayedWall1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\wall\\sprayed-wall1_diffuse.png");
	matSprayedWall1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\wall\\sprayed-wall1_normal.png");

	std::shared_ptr<Material> matVentedMetalPanel1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matVentedMetalPanel1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.7f), 1.0f));
	XMStoreFloat4A(&matVentedMetalPanel1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.5f), 4.0f));
	matVentedMetalPanel1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metal\\vented-metal-panel1_diffuse.png");
	matVentedMetalPanel1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metal\\vented-metal-panel1_normal.png");

	// std::shared_ptr<Material> matMetalVentilation1 = ResourceLoader::GetInstance()->CreateMaterial();
	// XMStoreFloat4A(&matMetalVentilation1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.4f), 1.0f));
	// XMStoreFloat4A(&matMetalVentilation1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.5f), 1.0f));
	// matMetalVentilation1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metal\\metal-ventilation1-diffuse.png");
	// matMetalVentilation1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metal\\metal-ventilation1-normal.png");

	std::shared_ptr<Material> matNarrowbrick1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matNarrowbrick1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.8f), 1.0f));
	XMStoreFloat4A(&matNarrowbrick1->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	matNarrowbrick1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\narrowbrick1_diffuse.png");
	matNarrowbrick1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\narrowbrick1_normal.png");

	std::shared_ptr<Material> matRedbricks2b = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matRedbricks2b->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.8f), 1.0f));
	XMStoreFloat4A(&matRedbricks2b->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	matRedbricks2b->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\redbricks2b_diffuse.png");
	matRedbricks2b->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\redbricks2b_normal.png");

	// std::shared_ptr<Material> matModernBrick1 = ResourceLoader::GetInstance()->CreateMaterial();
	// XMStoreFloat4A(&matModernBrick1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	// XMStoreFloat4A(&matModernBrick1->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	// matModernBrick1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\modern-brick1_diffuse.png");
	// matModernBrick1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\modern-brick1_normal.png");

	std::shared_ptr<Material> matStoneTile4b = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matStoneTile4b->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.7f), 1.0f));
	XMStoreFloat4A(&matStoneTile4b->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matStoneTile4b->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\floor\\stone-tile4b_diffuse.png");
	matStoneTile4b->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\floor\\stone-tile4b_normal.png");
	
	std::shared_ptr<Material> matSolidLightPink = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matSolidLightPink->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::LightPink, 0.7f), 1.0f));
	XMStoreFloat4A(&matSolidLightPink->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));


	std::shared_ptr<Material> matDoorFrame = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matDoorFrame->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::Brown, 0.4f), 1.0f));
	XMStoreFloat4A(&matDoorFrame->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 2.0f));

	constexpr XMFLOAT3 FPSARM_POS(0.0f, -0.2f, -0.06f);
	constexpr XMFLOAT3 FPSM16_OFFSET(0.1f, -0.04f, 0.23f);
	constexpr XMFLOAT3 FPSM16_POS(FPSARM_POS.x + FPSM16_OFFSET.x, FPSARM_POS.y + FPSM16_OFFSET.y, FPSARM_POS.z + FPSM16_OFFSET.z);
	// FPS Arm
	{
		ModelData mdFPSArms = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\Characters\\Steven\\fps\\fpsarms.glb");
		std::shared_ptr<SkinnedMesh> meshFPSArms = mdFPSArms.m_skinnedMeshes[0];
		std::shared_ptr<Armature> armaFPSArms = mdFPSArms.m_armatures[0];

		GameObjectHandle hGameObject = CreateGameObject(L"FPS Arms");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetParent(&pMainCamera->m_transform);

		pGameObject->m_transform.SetPosition(FPSARM_POS);

		ComponentHandle<SkinnedMeshRenderer> hMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
		SkinnedMeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();

		auto matArms = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matArms->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 1.0f));
		XMStoreFloat4A(&matArms->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
		matArms->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\Characters\\Steven\\fps\\tex_diffuse.jpg");

		pMeshRenderer->SetMesh(meshFPSArms);
		pMeshRenderer->SetMaterial(0, matArms);

		pMeshRenderer->SetArmature(armaFPSArms);
		pMeshRenderer->PlayAnimation("arms_idle_m16a1", 1.0f, true, 0.0f);
		// pMeshRenderer->PlayAnimation("arms_reload_m16a1", 1.0f, true, 0.0f);
		// pMeshRenderer->PlayAnimation("arms_shoot_m16a1", 1.0f, true, 0.0f);
		// pMeshRenderer->PlayAnimation("arms_run_m16a1", 1.0f, true, 0.0f);
	}

	auto matSTANAG30Rds = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matSTANAG30Rds->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.05f), 1.0f));
	XMStoreFloat4A(&matSTANAG30Rds->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 2.0f));
	ModelData md = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\M16A1\\m16a1_pv.glb");
	std::shared_ptr<SkinnedMesh> mesh = md.m_skinnedMeshes[0];
	std::shared_ptr<Armature> armaM16 = md.m_armatures[0];
	// Animated Weapons(m16a1)
	{
		GameObjectHandle hGameObject = CreateGameObject(L"M16A1");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetParent(&pMainCamera->m_transform);
		
		pGameObject->m_transform.SetPosition(FPSM16_POS);
		
		ComponentHandle<SkinnedMeshRenderer> hMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
		SkinnedMeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		
		// 재질 설정
		auto matM16A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM16A1Receiver->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
		XMStoreFloat4A(&matM16A1Receiver->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 2.0f));
		matM16A1Receiver->m_diffuseMap =
			ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\receiver_diffuse.jpg");
		matM16A1Receiver->m_normalMap =
			ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\receiver_normal.jpg");
		
		auto matM16A1Furniture = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM16A1Furniture->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
		XMStoreFloat4A(&matM16A1Furniture->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 4.0f));
		matM16A1Furniture->m_diffuseMap =
			ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\furniture_diffuse.jpg");
		matM16A1Furniture->m_normalMap =
			ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\furniture_normal.jpg");
		
		pMeshRenderer->SetMesh(mesh);
		pMeshRenderer->SetMaterial(0, matM16A1Receiver);
		pMeshRenderer->SetMaterial(1, matM16A1Furniture);
		pMeshRenderer->SetMaterial(2, matSTANAG30Rds);
		
		
		pMeshRenderer->SetArmature(armaM16);
		pMeshRenderer->PlayAnimation("m16a1_idle", 1.0f, true, 0.0f);
		// pMeshRenderer->PlayAnimation("m16a1_reload", 1.0f, true, 0.0f);
		// pMeshRenderer->PlayAnimation("m16a1_shoot", 1.0f, true, 0.0f);
		// pMeshRenderer->PlayAnimation("m16a1_run", 1.0f, true, 0.0f);
	}

	// Animated Weapons(m4a1)
	{
		// ModelData md = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m4a1\\m4a1_pv.glb");
		// std::shared_ptr<SkinnedMesh> mesh = md.m_skinnedMeshes[0];
		// // std::shared_ptr<Armature> arma = md.armatures[0];
		// 
		// GameObjectHandle hGameObject = CreateGameObject(L"M4A1");
		// GameObject* pGameObject = hGameObject.ToPtr();
		// pGameObject->m_transform.SetParent(&pMainCamera->m_transform);
		// 
		// pGameObject->m_transform.SetPosition(FPSM16_POS);
		// 
		// ComponentHandle<SkinnedMeshRenderer> hMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
		// SkinnedMeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 
		// // 재질 설정
		// auto matM4A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
		// XMStoreFloat4A(&matM4A1Receiver->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		// XMStoreFloat4A(&matM4A1Receiver->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
		// matM4A1Receiver->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_diffuse.png");
		// matM4A1Receiver->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_normal.png");
		// auto matM4A1Furniture = ResourceLoader::GetInstance()->CreateMaterial();
		// XMStoreFloat4A(&matM4A1Furniture->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		// XMStoreFloat4A(&matM4A1Furniture->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 8.0f));
		// matM4A1Furniture->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_diffuse.png");
		// matM4A1Furniture->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_normal.png");
		// auto matRearSight = ResourceLoader::GetInstance()->CreateMaterial();
		// XMStoreFloat4A(&matRearSight->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		// XMStoreFloat4A(&matRearSight->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
		// matRearSight->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_diffuse.png");
		// matRearSight->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_normal.png");
		// 
		// pMeshRenderer->SetMesh(mesh);
		// pMeshRenderer->SetMaterial(0, matM4A1Receiver);
		// pMeshRenderer->SetMaterial(1, matM4A1Furniture);
		// pMeshRenderer->SetMaterial(2, matRearSight);
		// pMeshRenderer->SetMaterial(3, matSTANAG30Rds);
		// 
		// pMeshRenderer->SetArmature(armaM16);
		// // pMeshRenderer->PlayAnimation("m16a1_idle", 1.0f, true, 0.0f);
		// pMeshRenderer->PlayAnimation("m16a1_reload", 1.0f, true, 0.0f);
		// // pMeshRenderer->PlayAnimation("m16a1_shoot", 1.0f, true, 0.0f);
		// // pMeshRenderer->PlayAnimation("m16a1_run", 1.0f, true, 0.0f);
	}


	// 물리엔진 테스트 박스
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(-0.5f, 6.0f, 0.5f);
		// pGameObject->m_transform.SetRotationEuler(XMConvertToRadians(20.0f), 0, 0);
		// pGameObject->m_transform.SetRotationEuler(0, XMConvertToRadians(20.0f), 0);
		// pGameObject->m_transform.SetRotationEuler(0, 0, XMConvertToRadians(20.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matPaperBox);

		std::shared_ptr<BoxCollider> collider = std::make_shared<BoxCollider>(meshBox->GetAabb().Extents);
		ComponentHandle<Rigidbody> hRigidbody = pGameObject->AddComponent<Rigidbody>(collider, meshBox->GetAabb().Center);
		Rigidbody* pRigidbody = hRigidbody.ToPtr();
		pRigidbody->SetFreezePosition(true, false, true);
		pRigidbody->SetFreezeRotation(true, true, true);
		pRigidbody->SetFriction(0.5);

		// 계층구조 작동 테스트
		{
			GameObjectHandle hGameObject = CreateGameObject(L"Box");
			GameObject* pGameObject1 = hGameObject.ToPtr();

			pGameObject1->m_transform.SetParent(&pGameObject->m_transform);

			pGameObject1->m_transform.SetPosition(0.0f, 2.5f, 0.0f);
			pGameObject1->m_transform.SetRotationEuler(XMConvertToRadians(20.0f), 0, 0);
			// pGameObject1->m_transform.SetRotationEuler(0, XMConvertToRadians(20.0f), 0);
			// pGameObject1->m_transform.SetRotationEuler(0, 0, XMConvertToRadians(20.0f));

			ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject1->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			pMeshRenderer->SetMesh(meshBox);
			pMeshRenderer->SetMaterial(0, matPaperBox);
		}
	}

	// Kinematic 발판 테스트
	GameObjectHandle hKinematicFootboard;
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Footboard");
		hKinematicFootboard = hGameObject;

		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(-1.0f, 0.5f, 1.0f);
		
		// ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		// MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// pMeshRenderer->SetMesh(meshBox);
		// pMeshRenderer->SetMaterial(0, matPaperBox);
		
		std::shared_ptr<BoxCollider> collider = std::make_shared<BoxCollider>(XMFLOAT3(1.0f, 0.25f, 1.0f));
		ComponentHandle<Rigidbody> hRigidbody = pGameObject->AddComponent<Rigidbody>(collider);
		Rigidbody* pRigidbody = hRigidbody.ToPtr();
		pRigidbody->SetKinematic(true);
	}
	pScriptFPSMovement->m_hKinematicFootboard = hKinematicFootboard;


	// 캐릭터 Skinned Mesh Test
	{
		ModelData mdMaleBase = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\Characters\\Steven\\steven.glb");
		std::shared_ptr<SkinnedMesh> meshSteven = mdMaleBase.m_skinnedMeshes[0];
		std::shared_ptr<Armature> armaSteven = mdMaleBase.m_armatures[0];

		GameObjectHandle hGameObject = CreateGameObject(L"Steven");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorZero());

		ComponentHandle<SkinnedMeshRenderer> hMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
		SkinnedMeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();

		auto matBody = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matBody->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
		XMStoreFloat4A(&matBody->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
		matBody->m_specular.w = 4.0f;
		matBody->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\Characters\\Steven\\textures\\body.png");
		
		auto matSuit = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matSuit->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
		XMStoreFloat4A(&matSuit->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
		matSuit->m_specular.w = 4.0f;
		matSuit->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\Characters\\Steven\\textures\\suit_diffuse.png");
		matSuit->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\Characters\\Steven\\textures\\suit_normal.png");
		
		auto matShoes = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matShoes->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		XMStoreFloat4A(&matShoes->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		matShoes->m_specular.w = 4.0f;
		
		pMeshRenderer->SetMesh(meshSteven);
		pMeshRenderer->SetMaterial(0, matBody);
		pMeshRenderer->SetMaterial(1, matSuit);
		pMeshRenderer->SetMaterial(2, matShoes);

		pMeshRenderer->SetArmature(armaSteven);
		pMeshRenderer->PlayAnimation("run", 1.0f, true, 0.0f);
	}
	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Long Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-4.5371f, 0.0f, -15.518f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(-45.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedLongContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}

	// Mesh Game Objects
	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-9.5598f, 0.0f, -16.722f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(45.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}
	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Long Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-4.5371f, 0.0f, -15.518f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(-45.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedLongContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}
	// 가운데 박스 2개
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-6.45f, 0.0f, -16.15f, 0.0f));
		pGameObject->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(-45.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-6.45f, 1.0f, -16.15f, 0.0f));
		pGameObject->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(-45.0f), 0.0f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorScale(Vector3::One(), 0.8f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	// 오른쪽 박스 5개
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-2.5f, 0.0f, -20.8f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-2.2f, 1.0f, -20.8f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-2.4f, 0.0f, -19.8f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-2.2f, 1.0f, -19.8f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-2.2f, 2.0f, -19.8f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}

	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Long Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(9.0f, 0.0f, -17.7f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(90.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedLongContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}

	{
		GameObjectHandle hContainer = CreateGameObject(L"Open Container1");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-12.7f, 0.0f, -7.1f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(180.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshOpenContainer1);
		pMeshRenderer->SetMaterial(0, matOpenContainer);
	}

	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Long Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-1.4f, 0.0f, -7.6f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(-70.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedLongContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}

	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Long Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(12.7f, 0.0f, -9.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedLongContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}

	// 박스 4개
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(6.05f, 0.0f, -12.55f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.1f, 0.9f, 1.1f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(7.1f, 0.0f, -12.5f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.0f, 0.8f, 1.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(6.1f, 0.0f, -11.5f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.0f, 0.8f, 1.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(7.15f, 0.0f, -11.45f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.1f, 0.9f, 1.1f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}

	// 박스 2개
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(8.1f, 0.0f, -5.9f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.0f, 0.8f, 1.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(8.0f, 0.0f, -4.9f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.0f, 0.8f, 1.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}

	// 박스 4개
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-4.95f, 0.0f, -1.55f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.1f, 0.9f, 1.1f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-3.9f, 0.0f, -1.5f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.0f, 0.8f, 1.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-4.9f, 0.0f, -0.5f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.0f, 0.8f, 1.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-3.85f, 0.0f, -0.45f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.1f, 0.9f, 1.1f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}


	// 박스 3개
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(10.9f, 1.0f, 3.1f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(11.9f, 0.0f, 3.1f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(10.9f, 0.0f, 4.1f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}


	// 왼쪽 벽면 박스 2개
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-13.5f, 0.0f, 10.2f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-13.6f, 1.0f, 10.2f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(0.8f, 0.8f, 0.8f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}


	// 블루팀 기지 오른쪽 입구 앞 컨테이너 박스 3개
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-10.9f, 0.0f, 19.2f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-11.0f, 1.0f, 19.1f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(0.8f, 0.8f, 0.8f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-9.95f, 0.0f, 19.15f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(0.9f, 0.9f, 0.9f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}


	// 블루팀 기지 왼쪽 입구 앞 컨테이너 박스 3개
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(9.9f, 0.0f, 17.4f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(10.9f, 0.0f, 17.6f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(10.9f, 1.0f, 17.6f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}


	// 블루팀 입구 사이 박스 2개 x 2 (총 4개)
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-3.5f, 0.0f, 23.5f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.1f, 1.1f, 1.1f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-3.5f, 1.1f, 23.5f, 0.0f));
		pGameObject->m_transform.SetScale(XMVectorSet(1.1f, 1.1f, 1.1f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-1.2f, 0.0f, 21.8f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-1.2f, 1.0f, 21.8f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matWoodenBox);
	}


	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-9.6709f, 0.0f, 3.5936f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(68.856f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}
	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-6.0f, 0.0f, 6.3f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(-21.144f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}
	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-2.3291f, 0.0f, 9.0064f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(68.856f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}

	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(4.7f, 0.0f, 3.9f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(90.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}
	{
		GameObjectHandle hContainer = CreateGameObject(L"Open Container2");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(5.6f, 1.6f, 0.4f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(XMConvertToRadians(-30.0f), XMConvertToRadians(45.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshOpenContainer2);
		pMeshRenderer->SetMaterial(0, matOpenContainer);
	}
	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(9.1f, 0.0f, 5.7f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}
	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(10.9f, 0.0f, 1.3f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(90.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}

	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed LongContainer");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-10.0f, 0.0f, 17.4f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(-90.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedLongContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}
	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-12.7f, 0.0f, 21.8f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}

	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(1.7f, 0.0f, 16.5f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(75.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}

	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(0.5f, 0.0f, 23.6f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(90.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}

	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed Container");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(10.9f, 0.0f, 23.6f, 0.0f));
		pContainer->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(90.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}
	{
		GameObjectHandle hContainer = CreateGameObject(L"Closed LongContainer");
		GameObject* pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(12.7f, 0.0f, 18.3f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshClosedLongContainer);
		pMeshRenderer->SetMaterial(0, matClosedContainer);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, -0.3f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFrame);
		pMeshRenderer->SetMaterial(0, matHouseFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, 12.7f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFrame);
		pMeshRenderer->SetMaterial(0, matHouseFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, -13.3f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFrame);
		pMeshRenderer->SetMaterial(0, matHouseFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, 24.6f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFrame);
		pMeshRenderer->SetMaterial(0, matHouseFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, -21.6f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFrame);
		pMeshRenderer->SetMaterial(0, matHouseFrame);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Side Wall");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-14.1f, 0.0f, 0.0f, 0.0f));
		pGameObject->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(90.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseSideWall);
		pMeshRenderer->SetMaterial(0, matNarrowbrick1);
		pMeshRenderer->SetMaterial(1, matVentedMetalPanel1);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Side Wall");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(14.1f, 0.0f, 0.0f, 0.0f));
		pGameObject->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(-90.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseSideWall);
		pMeshRenderer->SetMaterial(0, matNarrowbrick1);
		pMeshRenderer->SetMaterial(1, matVentedMetalPanel1);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Red Side Wall");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, -22.0f, 0.0f));
		pGameObject->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(180.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseRedSideWall);
		pMeshRenderer->SetMaterial(0, matRedbricks2b);
		pMeshRenderer->SetMaterial(1, matSolidLightPink);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"HouseWallBlueSide");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, 25.0f, 0.0f));
		pGameObject->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(180.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseBlueSideWall);
		pMeshRenderer->SetMaterial(0, matRedbricks2b);
		pMeshRenderer->SetMaterial(1, matSprayedWall1);
		pMeshRenderer->SetMaterial(2, matBambooWoodSemigloss);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"Blue Team Base");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, 30.0f, 0.0f));
		pGameObject->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(180.0f), 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBlueTeamBase);
		pMeshRenderer->SetMaterial(0, matStoneTile4b);
		pMeshRenderer->SetMaterial(1, matConcrete3);
		pMeshRenderer->SetMaterial(2, matSprayedWall1);
		pMeshRenderer->SetMaterial(3, matBambooWoodSemigloss);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"Paper Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(8.4f, 0.0f, 25.6f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matPaperBox);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Paper Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(8.4f, 0.0f, 26.6f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBox);
		pMeshRenderer->SetMaterial(0, matPaperBox);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"Red Team Base");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, -28.5f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshRedTeamBase);
		pMeshRenderer->SetMaterial(0, matBrick22);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Roof");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseRoof);
		pMeshRenderer->SetMaterial(0, matRoof24);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"Door Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-6.7f, 0.0f, -22.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshDoorFrame);
		pMeshRenderer->SetMaterial(0, matDoorFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Door Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(6.7f, 0.0f, -22.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshDoorFrame);
		pMeshRenderer->SetMaterial(0, matDoorFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Door Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(-6.7f, 0.0f, 25.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshDoorFrame);
		pMeshRenderer->SetMaterial(0, matDoorFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Door Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(6.7f, 0.0f, 25.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshDoorFrame);
		pMeshRenderer->SetMaterial(0, matDoorFrame);
	}



	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Floor");
		GameObject* pGameObject = hGameObject.ToPtr();

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFloor);
		pMeshRenderer->SetMaterial(0, matConcrete3);
	}


	/*
	// TEST CODE
	// WEAPON
	// 
	// Primary weapon
	std::shared_ptr<StaticMesh> meshSTANAG30Rds = 
		ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\Magazine\\STANAG\\STANAG30Rds.obj").staticMeshes[0];
	auto matSTANAG30Rds = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matSTANAG30Rds->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.05f), 1.0f));
	XMStoreFloat4A(&matSTANAG30Rds->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 2.0f));
	{
		GameObjectHandle hPrimaryWeapon = CreateGameObject(L"Primary Weapon");
		pScriptFPSMovement->m_hWeapons[0] = hPrimaryWeapon;	// 1번 슬롯

		GameObject* pPrimaryWeapon = hPrimaryWeapon.ToPtr();
		pPrimaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);

		pPrimaryWeapon->m_transform.SetPosition(0.1f, -0.2f, 0.12f);
		ComponentHandle<MeshRenderer> hRifleMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hRifleMeshRenderer.ToPtr();
		// 메시 설정
		auto meshM16A1 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\M16A1\\M16A1.obj").staticMeshes[0];
		pMeshRenderer->SetMesh(meshM16A1);
		// 재질 설정
		auto matM16A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM16A1Receiver->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
		XMStoreFloat4A(&matM16A1Receiver->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 2.0f));
		matM16A1Receiver->m_diffuseMap = 
			ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\receiver_diffuse.jpg");
		matM16A1Receiver->m_normalMap = 
			ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\receiver_normal.jpg");

		auto matM16A1Furniture = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM16A1Furniture->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
		XMStoreFloat4A(&matM16A1Furniture->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 4.0f));
		matM16A1Furniture->m_diffuseMap = 
			ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\furniture_diffuse.jpg");
		matM16A1Furniture->m_normalMap = 
			ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\furniture_normal.jpg");
		pMeshRenderer->SetMaterial(0, matM16A1Receiver);
		pMeshRenderer->SetMaterial(1, matM16A1Furniture);

		{
			GameObjectHandle hMagazine = CreateGameObject(L"PW Magazine");
			GameObject* pPWMagazine = hMagazine.ToPtr();
			pPWMagazine->m_transform.SetParent(&pPrimaryWeapon->m_transform);

			pPWMagazine->m_transform.SetPosition(0.0, -0.032f, 0.068f);
			ComponentHandle<MeshRenderer> hMeshRenderer = pPWMagazine->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			pMeshRenderer->SetMesh(meshSTANAG30Rds);
			// 재질 설정
			pMeshRenderer->SetMaterial(0, matSTANAG30Rds);
		}
	}

	// Secondary weapon
	{
		GameObjectHandle hSecondaryWeapon = CreateGameObject(L"Secondary Weapon");
		pScriptFPSMovement->m_hWeapons[1] = hSecondaryWeapon;		// 2번 슬롯

		GameObject* pSecondaryWeapon = hSecondaryWeapon.ToPtr();
		pSecondaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);

		pSecondaryWeapon->SetActive(false);	// 안보이게 비활성화 상태로 초기화
		pSecondaryWeapon->m_transform.SetPosition(0.1f, -0.14f, 0.28f);
		ComponentHandle<MeshRenderer> hMeshRenderer = pSecondaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		auto meshM9A1 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m9a1\\m9a1_tv.obj").staticMeshes[0];
		pMeshRenderer->SetMesh(meshM9A1);
		// 재질 설정
		auto matM9A1 = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM9A1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
		XMStoreFloat4A(&matM9A1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
		matM9A1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m9a1\\textures\\diffuse.png");
		matM9A1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m9a1\\textures\\normal.png");
		pMeshRenderer->SetMaterial(0, matM9A1);

		{
			GameObjectHandle hWeaponLight = CreateGameObject(L"Weapon Light");
			GameObject* pWeaponLight = hWeaponLight.ToPtr();
			pWeaponLight->m_transform.SetParent(&pSecondaryWeapon->m_transform);

			pWeaponLight->m_transform.SetPosition(0.0f, -0.004f, 0.028f);
			ComponentHandle<MeshRenderer> hMeshRenderer = pWeaponLight->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			auto meshX300U = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\x300u\\x300u.obj").staticMeshes[0];
			pMeshRenderer->SetMesh(meshX300U);
			// 재질 설정
			auto matX300U = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&matX300U->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
			XMStoreFloat4A(&matX300U->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 4.0f));
			matX300U->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\x300u\\textures\\Body_Black_albedo.jpg");
			pMeshRenderer->SetMaterial(0, matX300U);
		}
	}


	{
		GameObjectHandle hPrimaryWeapon = CreateGameObject(L"Primary Weapon");
		pScriptFPSMovement->m_hWeapons[2] = hPrimaryWeapon;		// 3번 슬롯

		GameObject* pPrimaryWeapon = hPrimaryWeapon.ToPtr();
		pPrimaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);

		pPrimaryWeapon->SetActive(false);	// 안보이게 비활성화 상태로 초기화
		pPrimaryWeapon->m_transform.SetPosition(0.1f, -0.2f, 0.12f);
		ComponentHandle<MeshRenderer> hMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		auto meshM4A1 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m4a1\\m4a1_tv.obj").staticMeshes[0];
		pMeshRenderer->SetMesh(meshM4A1);
		// 재질 설정
		auto matM4A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM4A1Receiver->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		XMStoreFloat4A(&matM4A1Receiver->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
		matM4A1Receiver->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_diffuse.png");
		matM4A1Receiver->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_normal.png");
		auto matM4A1Furniture = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM4A1Furniture->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		XMStoreFloat4A(&matM4A1Furniture->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 8.0f));
		matM4A1Furniture->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_diffuse.png");
		matM4A1Furniture->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_normal.png");
		auto matRearSight = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matRearSight->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		XMStoreFloat4A(&matRearSight->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
		matRearSight->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_diffuse.png");
		matRearSight->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_normal.png");

		pMeshRenderer->SetMaterial(0, matM4A1Receiver);
		pMeshRenderer->SetMaterial(1, matM4A1Furniture);
		pMeshRenderer->SetMaterial(2, matRearSight);

		

		{
			GameObjectHandle hMagazine = CreateGameObject(L"PW Magazine");
			GameObject* pPWMagazine = hMagazine.ToPtr();
			pPWMagazine->m_transform.SetParent(&pPrimaryWeapon->m_transform);

			pPWMagazine->m_transform.SetPosition(0.0, -0.034f, 0.064f);
			ComponentHandle<MeshRenderer> hMeshRenderer = pPWMagazine->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			pMeshRenderer->SetMesh(meshSTANAG30Rds);
			// 재질 설정
			pMeshRenderer->SetMaterial(0, matSTANAG30Rds);
		}
	}


	{
		GameObjectHandle hPrimaryWeapon = CreateGameObject(L"Primary Weapon");
		pScriptFPSMovement->m_hWeapons[3] = hPrimaryWeapon;		// 4번 슬롯

		GameObject* pPrimaryWeapon = hPrimaryWeapon.ToPtr();
		pPrimaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);

		pPrimaryWeapon->SetActive(false);	// 안보이게 비활성화 상태로 초기화
		pPrimaryWeapon->m_transform.SetPosition(0.1f, -0.21f, 0.135f);
		ComponentHandle<MeshRenderer> hMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		auto mesh = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\M762\\m762.obj").staticMeshes[0];
		pMeshRenderer->SetMesh(mesh);
		// 재질 설정
		auto matReceiver = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matReceiver->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.5f), 1.0f));
		XMStoreFloat4A(&matReceiver->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 4.0f));
		matReceiver->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M762\\textures\\Receiver_Albedo2.png");
		matReceiver->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M762\\textures\\Receiver_Normal.png");
		auto matFurniture = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matFurniture->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		XMStoreFloat4A(&matFurniture->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 2.0f));
		matFurniture->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M762\\textures\\Furniture_Albedo.png");
		matFurniture->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M762\\textures\\Furniture_Normal.png");
		auto matRail = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matRail->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		XMStoreFloat4A(&matRail->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
		matRail->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M762\\textures\\Rail_Albedo.png");
		matRail->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M762\\textures\\Rail_Normal.png");
		
		pMeshRenderer->SetMaterial(0, matReceiver);
		pMeshRenderer->SetMaterial(1, matFurniture);
		pMeshRenderer->SetMaterial(2, matRail);
		{
			GameObjectHandle hMagazine = CreateGameObject(L"PW Magazine");
			GameObject* pPWMagazine = hMagazine.ToPtr();
			pPWMagazine->m_transform.SetParent(&pPrimaryWeapon->m_transform);

			pPWMagazine->m_transform.SetPosition(0.0, 0.0f, 0.169f);
			ComponentHandle<MeshRenderer> hMeshRenderer = pPWMagazine->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();

			// 메시 설정

			std::shared_ptr<StaticMesh> meshMagazine =
				ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\Magazine\\SteelStamped\\762SteelStampedMag.obj").staticMeshes[0];
			pMeshRenderer->SetMesh(meshMagazine);


			// 재질 설정
			auto matMagazine = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&matMagazine->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.05f), 1.0f));
			XMStoreFloat4A(&matMagazine->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 4.0f));
			pMeshRenderer->SetMaterial(0, matMagazine);
		}
	}
	*/


	// Skybox
	{
		Texture2D skybox = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\skybox\\sky27.dds", false);
		RenderSettings::GetInstance()->SetSkybox(skybox);

		RenderSettings::GetInstance()->SetAmbientLightColor(ColorsLinear::White);
		RenderSettings::GetInstance()->SetAmbientLightIntensity(0.025f);
	}
}
