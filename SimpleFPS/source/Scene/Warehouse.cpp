#include "Warehouse.h"
#include "..\Script\FirstPersonMovement.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(Warehouse);

void Warehouse::OnLoadScene()
{
	// Lights
	{
		GameObjectHandle hSun = CreateGameObject(L"Sun");
		GameObject* pSun = hSun.ToPtr();
		pSun->m_transform.SetRotationEuler(XMVectorSet(XMConvertToRadians(45), XMConvertToRadians(150), 0.0f, 0.0f));
		ComponentHandle<DirectionalLight> hLight = pSun->AddComponent<DirectionalLight>();
		DirectionalLight* pLight = hLight.ToPtr();
		XMStoreFloat4A(&pLight->m_ambient, XMVectorScale(Math::Vector3::One(), 0.1f));
		XMStoreFloat4A(&pLight->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.6f));
		XMStoreFloat4A(&pLight->m_specular, Math::Vector3::One());
	}

	GameObjectHandle hFlashLight = CreateGameObject(L"손전등");
	{
		GameObject* pFL = hFlashLight.ToPtr();
		pFL->m_transform.SetPosition(XMVectorSet(0.0f, 1.0f, 1.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMR = pFL->AddComponent<MeshRenderer>();
		auto mesh = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Props\\flash.obj").staticMeshes[0];
		hMR.ToPtr()->SetMesh(mesh);

		ComponentHandle<SpotLight> hSpotLight = pFL->AddComponent<SpotLight>();
		SpotLight* pSpotLight = hSpotLight.ToPtr();
		XMStoreFloat4A(&pSpotLight->m_ambient, XMVectorScale(ColorsLinear::LightYellow, 0.02f));
		XMStoreFloat4A(&pSpotLight->m_diffuse, XMVectorScale(ColorsLinear::LightYellow, 1.0f));
		XMStoreFloat4A(&pSpotLight->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::LightYellow, 1.0f), 1.0f));
		XMStoreFloat3(&pSpotLight->m_att, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		pSpotLight->m_range = 20.0f;
		pSpotLight->m_spotExp = 32.0f;

		// ComponentHandle<DirectionalLight> hDL = pFL->AddComponent<DirectionalLight>();
		// DirectionalLight* pDL = hDL.ToPtr();
		// XMStoreFloat4A(&pDL->m_ambient, XMVectorScale(ColorsLinear::GreenYellow, 0.02f));
		// XMStoreFloat4A(&pDL->m_diffuse, XMVectorScale(ColorsLinear::GreenYellow, 0.6f));
		// XMStoreFloat4A(&pDL->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::GreenYellow, 0.2f), 2.0f));
		// 
		// ComponentHandle<PointLight> hPointLight = pFL->AddComponent<PointLight>();
		// PointLight* pPointLight = hPointLight.ToPtr();
		// XMStoreFloat4A(&pPointLight->m_ambient, XMVectorScale(ColorsLinear::GreenYellow, 0.02f));
		// XMStoreFloat4A(&pPointLight->m_diffuse, XMVectorScale(ColorsLinear::GreenYellow, 0.6f));
		// XMStoreFloat4A(&pPointLight->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::GreenYellow, 0.2f), 2.0f));
		// XMStoreFloat3(&pPointLight->m_att, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		// pPointLight->m_range = 3.0f;
	}

	// Main Camera
	GameObject* pMainCamera = nullptr;
	FirstPersonMovement* pFirstPersonMovement = nullptr;
	{
		GameObjectHandle hMainCamera = CreateGameObject(L"Main Camera");

		pMainCamera = hMainCamera.ToPtr();
		pMainCamera->m_transform.SetPosition(XMVectorSet(0.0f, 1.6f, -5.0f, 0.0f));

		ComponentHandle<Camera> hCameraComponent = pMainCamera->AddComponent<Camera>();
		Camera* pCameraComponent = hCameraComponent.ToPtr();
		pCameraComponent->SetBackgroundColor(ColorsLinear::Gray);
		pCameraComponent->SetDepth(0);
		pCameraComponent->SetFieldOfView(85);
		pCameraComponent->SetClippingPlanes(0.03f, 300.0f);

		ComponentHandle<FirstPersonMovement> hFirstPersonMovement = pMainCamera->AddComponent<FirstPersonMovement>();		// 1인칭 카메라 조작
		pFirstPersonMovement = hFirstPersonMovement.ToPtr();
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"광원 테스트");

		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetParent(&pMainCamera->m_transform);

		// ComponentHandle<SpotLight> hSpotLight = pGameObject->AddComponent<SpotLight>();
		// SpotLight* pSpotLight = hSpotLight.ToPtr();
		// XMStoreFloat4A(&pSpotLight->m_ambient, XMVectorScale(ColorsLinear::LightBlue, 0.02f));
		// XMStoreFloat4A(&pSpotLight->m_diffuse, XMVectorScale(ColorsLinear::LightBlue, 5.0f));
		// XMStoreFloat4A(&pSpotLight->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::LightBlue, 1.0f), 0.0f));
		// XMStoreFloat3(&pSpotLight->m_att, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		// pSpotLight->m_range = 15.0f;
		// pSpotLight->m_spotExp = 32.0f;

		// ComponentHandle<DirectionalLight> hDL = pGameObject->AddComponent<DirectionalLight>();
		// DirectionalLight* pDL = hDL.ToPtr();
		// XMStoreFloat4A(&pDL->m_ambient, XMVectorScale(ColorsLinear::GreenYellow, 0.02f));
		// XMStoreFloat4A(&pDL->m_diffuse, XMVectorScale(ColorsLinear::GreenYellow, 0.6f));
		// XMStoreFloat4A(&pDL->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::GreenYellow, 0.2f), 2.0f));
		
		ComponentHandle<PointLight> hPointLight = pGameObject->AddComponent<PointLight>();
		PointLight* pPointLight = hPointLight.ToPtr();
		XMStoreFloat4A(&pPointLight->m_ambient, XMVectorScale(ColorsLinear::GreenYellow, 0.02f));
		XMStoreFloat4A(&pPointLight->m_diffuse, XMVectorScale(ColorsLinear::GreenYellow, 1.0f));
		XMStoreFloat4A(&pPointLight->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::GreenYellow, 0.2f), 2.0f));
		XMStoreFloat3(&pPointLight->m_att, XMVectorSet(0.0f, 0.05f, 0.0f, 0.0f));
		pPointLight->m_range = 10.0f;
	}
	
	{
		// Adapter Info UI
		{
			UIObjectHandle hText = CreateText();
			Text* pText = static_cast<Text*>(hText.ToPtr());
			pText->SetSize(XMFLOAT2(256, 16));
			std::wstring text = GraphicDevice::GetInstance()->GetAdapterDescription();
			pText->SetText(std::move(text));
			pText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::LEFT);
			pText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::TOP);
			pText->m_transform.m_position.x += 128 + 2;
			pText->m_transform.m_position.y -= 8;
			pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pText->SetColor(Colors::Orange);
			pText->GetTextFormat().SetSize(12);
			pText->GetTextFormat().SetFontFamilyName(L"Consolas");
			pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
			pText->Apply();
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
			pText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::LEFT);
			pText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::TOP);
			pText->m_transform.m_position.x += 128 + 2;
			pText->m_transform.m_position.y -= (8 + 16 * 1);
			pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pText->SetColor(Colors::Orange);
			pText->GetTextFormat().SetSize(12);
			pText->GetTextFormat().SetFontFamilyName(L"Consolas");
			pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
			pText->Apply();
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
			pText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::LEFT);
			pText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::TOP);
			pText->m_transform.m_position.x += 128 + 2;
			pText->m_transform.m_position.y -= (8 + 16 * 2);
			pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pText->SetColor(Colors::Orange);
			pText->GetTextFormat().SetSize(12);
			pText->GetTextFormat().SetFontFamilyName(L"Consolas");
			pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
			pText->Apply();
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
			pText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::LEFT);
			pText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::TOP);
			pText->m_transform.m_position.x += 128 + 2;
			pText->m_transform.m_position.y -= (8 + 16 * 3);
			pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pText->SetColor(Colors::Orange);
			pText->GetTextFormat().SetSize(12);
			pText->GetTextFormat().SetFontFamilyName(L"Consolas");
			pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
			pText->Apply();
		}
	}

	// UI
	{
		UIObjectHandle hCrosshair = CreateImage();
		Image* pCrosshair = static_cast<Image*>(hCrosshair.ToPtr());
		pCrosshair->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Sprites\\ch.dds"));
		pCrosshair->SetNativeSize(true);
		pCrosshair->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pCrosshair->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);

		UIObjectHandle hHealthBgr = CreateImage();
		Image* pHealthBgr = static_cast<Image*>(hHealthBgr.ToPtr());
		pHealthBgr->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Sprites\\Health.png"));
		pHealthBgr->SetNativeSize(true);
		pHealthBgr->m_transform.m_position.x = pHealthBgr->GetHalfSize().x + 4;
		pHealthBgr->m_transform.m_position.y = pHealthBgr->GetHalfSize().y + 4;
		pHealthBgr->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::LEFT);
		pHealthBgr->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::BOTTOM);

		UIObjectHandle hWeaponIndicatorBgr = CreateImage();
		Image* pWeaponIndicatorBgr = static_cast<Image*>(hWeaponIndicatorBgr.ToPtr());
		pWeaponIndicatorBgr->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Sprites\\WeaponInfoIndicator.png"));
		pWeaponIndicatorBgr->SetNativeSize(true);
		pWeaponIndicatorBgr->m_transform.m_position.x = -pWeaponIndicatorBgr->GetHalfSize().x - 4;
		pWeaponIndicatorBgr->m_transform.m_position.y = pWeaponIndicatorBgr->GetHalfSize().y + 4;
		pWeaponIndicatorBgr->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::RIGHT);
		pWeaponIndicatorBgr->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::BOTTOM);

		{
			UIObjectHandle hHPText = CreateText();
			Text* pHPText = static_cast<Text*>(hHPText.ToPtr());
			pHPText->SetText(L"100");
			pHPText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::LEFT);
			pHPText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::BOTTOM);
			pHPText->m_transform.m_position = pHealthBgr->m_transform.m_position;
			pHPText->m_transform.m_position.x -= 56;
			pHPText->SetSize(XMFLOAT2(128, 48));
			pHPText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pHPText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pHPText->SetColor(Colors::White);
			pHPText->GetTextFormat().SetSize(28);
			pHPText->GetTextFormat().SetFontFamilyName(L"Impact");
			pHPText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
			pHPText->Apply();
		}

		{
			UIObjectHandle hAPText = CreateText();
			Text* pAPText = static_cast<Text*>(hAPText.ToPtr());
			pAPText->SetText(L"100");
			pAPText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::LEFT);
			pAPText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::BOTTOM);
			pAPText->m_transform.m_position = pHealthBgr->m_transform.m_position;
			pAPText->m_transform.m_position.x += 120;
			pAPText->SetSize(XMFLOAT2(128, 48));
			pAPText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pAPText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pAPText->SetColor(Colors::White);
			pAPText->GetTextFormat().SetSize(28);
			pAPText->GetTextFormat().SetFontFamilyName(L"Impact");
			pAPText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
			pAPText->Apply();
		}

		{
			UIObjectHandle hPointText = CreateText();
			Text* pPointText = static_cast<Text*>(hPointText.ToPtr());
			pPointText->SetText(L"126P");
			pPointText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::RIGHT);
			pPointText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::BOTTOM);
			pPointText->m_transform.m_position = pWeaponIndicatorBgr->m_transform.m_position;
			pPointText->m_transform.m_position.x -= 45;
			pPointText->m_transform.m_position.y += 34;
			pPointText->SetSize(XMFLOAT2(128, 32));
			pPointText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pPointText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pPointText->SetColor(Colors::White);
			pPointText->GetTextFormat().SetSize(22);
			pPointText->GetTextFormat().SetFontFamilyName(L"Agency FB");
			pPointText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
			pPointText->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
			pPointText->Apply();
		}

		{
			UIObjectHandle hWeaponNameText = CreateText();
			Text* pWeaponNameText = static_cast<Text*>(hWeaponNameText.ToPtr());
			pWeaponNameText->SetText(L"M16");
			pWeaponNameText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::RIGHT);
			pWeaponNameText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::BOTTOM);
			pWeaponNameText->m_transform.m_position = pWeaponIndicatorBgr->m_transform.m_position;
			pWeaponNameText->m_transform.m_position.y += 6;
			pWeaponNameText->SetSize(XMFLOAT2(200, 32));
			pWeaponNameText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pWeaponNameText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pWeaponNameText->SetColor(Colors::White);
			pWeaponNameText->GetTextFormat().SetSize(22);
			pWeaponNameText->GetTextFormat().SetFontFamilyName(L"Agency FB");
			pWeaponNameText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
			pWeaponNameText->Apply();
		}

		{
			UIObjectHandle hWeaponNameText = CreateText();
			Text* pWeaponNameText = static_cast<Text*>(hWeaponNameText.ToPtr());
			pWeaponNameText->SetText(L"30 / 90");
			pWeaponNameText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::RIGHT);
			pWeaponNameText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::BOTTOM);
			pWeaponNameText->m_transform.m_position = pWeaponIndicatorBgr->m_transform.m_position;
			pWeaponNameText->m_transform.m_position.x += 16;
			pWeaponNameText->m_transform.m_position.y -= 28;
			pWeaponNameText->SetSize(XMFLOAT2(128, 40));
			pWeaponNameText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pWeaponNameText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pWeaponNameText->SetColor(Colors::White);
			pWeaponNameText->GetTextFormat().SetSize(34);
			pWeaponNameText->GetTextFormat().SetFontFamilyName(L"Agency FB");
			pWeaponNameText->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
			pWeaponNameText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
			pWeaponNameText->Apply();
		}
	}


	// Resources
	std::shared_ptr<StaticMesh> meshClosedContainer = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Props\\ClosedContainer\\ClosedContainer.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshClosedLongContainer = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Props\\ClosedContainer\\ClosedLongContainer.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshOpenContainer1 = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Props\\OpenContainer\\OpenContainer1.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshOpenContainer2 = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Props\\OpenContainer\\OpenContainer2.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseFrame = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Maps\\Warehouse\\HouseFrame\\HouseFrame.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseSideWall = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Maps\\Warehouse\\HouseSideWall.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseFloor = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Maps\\Warehouse\\HouseFloor\\HouseFloor.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshBox = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Props\\Box\\Box.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseRedSideWall = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Maps\\Warehouse\\HouseRedSideWall.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseBlueSideWall = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Maps\\Warehouse\\HouseBlueSideWall.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshBlueTeamBase = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Maps\\Warehouse\\BlueTeamBase.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshRedTeamBase = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Maps\\Warehouse\\RedTeamBase.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseRoof = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Maps\\Warehouse\\HouseRoof\\HouseRoof.obj").staticMeshes[0];
	std::shared_ptr<StaticMesh> meshDoorFrame = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Maps\\Warehouse\\DoorFrame\\DoorFrame.obj").staticMeshes[0];

	Texture2D texBrick22 = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\bricks\\Brick 22 - 256x256.png");
	Texture2D texRoof24 = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\roof\\Roof 24 - 256x256.png");

	std::shared_ptr<Material> matAsphault = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matAsphault->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matAsphault->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.8f));
	XMStoreFloat4A(&matAsphault->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matAsphault->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\asphault\\AsphaultStreet_d.tga");
	matAsphault->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\asphault\\AsphaultStreet_n.tga");

	std::shared_ptr<Material> matBambooWoodSemigloss = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matBambooWoodSemigloss->m_ambient, XMVectorScale(Math::Vector3::One(), 0.1f));
	XMStoreFloat4A(&matBambooWoodSemigloss->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matBambooWoodSemigloss->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matBambooWoodSemigloss->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\wood\\bamboo-wood-semigloss-diffuse.png");
	matBambooWoodSemigloss->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\wood\\bamboo-wood-semigloss-normal.png");


	std::shared_ptr<Material> matBrick22 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matBrick22->m_ambient, XMVectorScale(Math::Vector3::One(), 0.25f));
	XMStoreFloat4A(&matBrick22->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matBrick22->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matBrick22->m_diffuseMap = texBrick22;


	std::shared_ptr<Material> matRoof24 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matRoof24->m_ambient, XMVectorScale(Math::Vector3::One(), 0.25f));
	XMStoreFloat4A(&matRoof24->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matRoof24->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matRoof24->m_diffuseMap = texRoof24;


	std::shared_ptr<Material> matClosedContainer = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matClosedContainer->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matClosedContainer->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.7f));
	XMStoreFloat4A(&matClosedContainer->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.2f), 4.0f));
	matClosedContainer->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Props\\ClosedContainer\\Diffuse.png");


	std::shared_ptr<Material> matOpenContainer = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matOpenContainer->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matOpenContainer->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.7f));
	XMStoreFloat4A(&matOpenContainer->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.2f), 4.0f));
	matOpenContainer->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Props\\OpenContainer\\Diffuse.png");


	std::shared_ptr<Material> matHouseFrame = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matHouseFrame->m_ambient, XMVectorScale(Colors::Brown, 0.1f));
	XMStoreFloat4A(&matHouseFrame->m_diffuse, XMVectorScale(Colors::Brown, 0.25f));
	XMStoreFloat4A(&matHouseFrame->m_specular, XMVectorScale(Math::Vector3::One(), 0.25f));
	matHouseFrame->m_specular.w = 8.0f;

	std::shared_ptr<Material> matWoodenBox = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matWoodenBox->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matWoodenBox->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.6f));
	XMStoreFloat4A(&matWoodenBox->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matWoodenBox->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Props\\Box\\WoodenBox.png");


	std::shared_ptr<Material> matPaperBox = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matPaperBox->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matPaperBox->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.7f));
	XMStoreFloat4A(&matPaperBox->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matPaperBox->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Props\\Box\\PaperBox.png");


	// std::shared_ptr<Material> matRustedSteelHotspot = ResourceLoader::GetInstance()->CreateMaterial();
	// XMStoreFloat4A(&matRustedSteelHotspot->m_ambient, XMVectorScale(Math::Vector3::One(), 0.25f));
	// XMStoreFloat4A(&matRustedSteelHotspot->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
	// XMStoreFloat4A(&matRustedSteelHotspot->m_specular, XMVectorSetW(Math::Vector3::OneHalf(), 4.0f));
	// matRustedSteelHotspot->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\metals\\RustedSteel\\RustedSteel_Diffuse.png");
	// matRustedSteelHotspot->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\metals\\RustedSteel\\RustedSteel_Normal.png");


	std::shared_ptr<Material> matConcrete3 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matConcrete3->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matConcrete3->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.6f));
	XMStoreFloat4A(&matConcrete3->m_specular, XMVectorSetW(Math::Vector3::Zero(), 1.0f));
	matConcrete3->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\concrete\\concrete3_diffuse.png");
	matConcrete3->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\concrete\\concrete3_normal.png");

	std::shared_ptr<Material> matSprayedWall1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matSprayedWall1->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matSprayedWall1->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.5f));
	XMStoreFloat4A(&matSprayedWall1->m_specular, XMVectorSetW(Math::Vector3::Zero(), 1.0f));
	matSprayedWall1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\wall\\sprayed-wall1_diffuse.png");
	matSprayedWall1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\wall\\sprayed-wall1_normal.png");

	std::shared_ptr<Material> matVentedMetalPanel1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matVentedMetalPanel1->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matVentedMetalPanel1->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.4f));
	XMStoreFloat4A(&matVentedMetalPanel1->m_specular, XMVectorSetW(Math::Vector3::OneHalf(), 1.0f));
	matVentedMetalPanel1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\metal\\vented-metal-panel1_diffuse.png");
	matVentedMetalPanel1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\metal\\vented-metal-panel1_normal.png");

	std::shared_ptr<Material> matNarrowbrick1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matNarrowbrick1->m_ambient, XMVectorScale(Math::Vector3::One(), 0.15f));
	XMStoreFloat4A(&matNarrowbrick1->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.8f));
	XMStoreFloat4A(&matNarrowbrick1->m_specular, XMVectorSetW(Math::Vector3::Zero(), 1.0f));
	matNarrowbrick1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\bricks\\narrowbrick1_diffuse.png");
	matNarrowbrick1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\bricks\\narrowbrick1_normal.png");

	std::shared_ptr<Material> matRedbricks2b = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matRedbricks2b->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matRedbricks2b->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.8f));
	XMStoreFloat4A(&matRedbricks2b->m_specular, XMVectorSetW(Math::Vector3::Zero(), 1.0f));
	matRedbricks2b->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\bricks\\redbricks2b_diffuse.png");
	matRedbricks2b->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\bricks\\redbricks2b_normal.png");

	// std::shared_ptr<Material> matModernBrick1 = ResourceLoader::GetInstance()->CreateMaterial();
	// XMStoreFloat4A(&matModernBrick1->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	// XMStoreFloat4A(&matModernBrick1->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.2f));
	// XMStoreFloat4A(&matModernBrick1->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	// matModernBrick1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\bricks\\modern-brick1_diffuse.png");
	// matModernBrick1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\bricks\\modern-brick1_normal.png");

	std::shared_ptr<Material> matStoneTile4b = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matStoneTile4b->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matStoneTile4b->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.7f));
	XMStoreFloat4A(&matStoneTile4b->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matStoneTile4b->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\floor\\stone-tile4b_diffuse.png");
	matStoneTile4b->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\floor\\stone-tile4b_normal.png");
	
	std::shared_ptr<Material> matSolidPink = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matSolidPink->m_ambient, XMVectorScale(Colors::LightPink, 0.2f));
	XMStoreFloat4A(&matSolidPink->m_diffuse, XMVectorScale(Colors::LightPink, 0.5f));
	XMStoreFloat4A(&matSolidPink->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));


	std::shared_ptr<Material> matRedTeamBase[1];
	{
		matRedTeamBase[0] = matBrick22;
	}


	std::shared_ptr<Material> matDoorFrame = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matDoorFrame->m_ambient, XMVectorScale(Colors::Brown, 0.15f));
	XMStoreFloat4A(&matDoorFrame->m_diffuse, XMVectorScale(Colors::Brown, 0.4f));
	XMStoreFloat4A(&matDoorFrame->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.1f), 2.0f));



	// 캐릭터 Skinned Mesh Test
	{
		ModelData mdMaleBase = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Characters\\Steven\\steven.glb");
		std::shared_ptr<SkinnedMesh> meshSteven = mdMaleBase.skinnedMeshes[0];
		std::shared_ptr<Armature> armaSteven = mdMaleBase.armatures[0];

		GameObjectHandle hGameObject = CreateGameObject(L"Steven");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorZero());

		ComponentHandle<SkinnedMeshRenderer> hMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
		SkinnedMeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();

		auto matBody = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matBody->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
		XMStoreFloat4A(&matBody->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.6f));
		XMStoreFloat4A(&matBody->m_specular, XMVectorScale(Math::Vector3::One(), 0.1f));
		matBody->m_specular.w = 4.0f;
		matBody->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Characters\\Steven\\textures\\body.png");
		
		auto matSuit = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matSuit->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
		XMStoreFloat4A(&matSuit->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.6f));
		XMStoreFloat4A(&matSuit->m_specular, Math::Vector3::Zero());
		matSuit->m_specular.w = 4.0f;
		matSuit->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Characters\\Steven\\textures\\suit_diffuse.png");
		matSuit->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Characters\\Steven\\textures\\suit_normal.png");
		
		auto matShoes = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matShoes->m_ambient, XMVectorScale(Math::Vector3::One(), 0.05f));
		XMStoreFloat4A(&matShoes->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.15f));
		XMStoreFloat4A(&matShoes->m_specular, XMVectorScale(Math::Vector3::One(), 0.1f));
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
		pGameObject->m_transform.SetScale(XMVectorScale(Math::Vector3::One(), 0.8f));

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
		pMeshRenderer->SetMaterial(1, matSolidPink);
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
		pMeshRenderer->SetMaterial(0, matRedTeamBase[0]);
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


	// TEST CODE
	// WEAPON
	// 
	// Primary weapon
	std::shared_ptr<StaticMesh> meshSTANAG30Rds = 
		ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Weapons\\Magazine\\STANAG\\STANAG30Rds.obj").staticMeshes[0];
	auto matSTANAG30Rds = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matSTANAG30Rds->m_ambient, XMVectorScale(Math::Vector3::One(), 0.05f));
	XMStoreFloat4A(&matSTANAG30Rds->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.05f));
	XMStoreFloat4A(&matSTANAG30Rds->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.05f), 2.0f));
	{
		GameObjectHandle hPrimaryWeapon = CreateGameObject(L"Primary Weapon");
		pFirstPersonMovement->m_hWeapons[0] = hPrimaryWeapon;	// 1번 슬롯

		GameObject* pPrimaryWeapon = hPrimaryWeapon.ToPtr();
		pPrimaryWeapon->m_transform.SetPosition(XMVectorSet(0.1f, -0.185f, 0.205f, 0.0f));
		ComponentHandle<MeshRenderer> hRifleMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hRifleMeshRenderer.ToPtr();
		// 메시 설정
		auto meshM16A1 = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Weapons\\M16A1\\M16A1.obj").staticMeshes[0];
		pMeshRenderer->SetMesh(meshM16A1);
		// 재질 설정
		auto matM16A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM16A1Receiver->m_ambient, XMVectorScale(Math::Vector3::One(), 0.15f));
		XMStoreFloat4A(&matM16A1Receiver->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.3f));
		XMStoreFloat4A(&matM16A1Receiver->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.15f), 2.0f));
		matM16A1Receiver->m_diffuseMap = 
			ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M16A1\\Textures\\M16A1Receiver_Diffuse.jpg");
		matM16A1Receiver->m_normalMap = 
			ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M16A1\\Textures\\M16A1Receiver_Normal.jpg");

		auto matM16A1Furniture = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM16A1Furniture->m_ambient, XMVectorScale(Math::Vector3::One(), 0.1f));
		XMStoreFloat4A(&matM16A1Furniture->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.1f));
		XMStoreFloat4A(&matM16A1Furniture->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.1f), 4.0f));
		matM16A1Furniture->m_diffuseMap = 
			ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M16A1\\Textures\\M16A1Furniture_Diffuse.jpg");
		matM16A1Furniture->m_normalMap = 
			ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M16A1\\Textures\\M16A1Furniture_Normal.jpg");
		pMeshRenderer->SetMaterial(0, matM16A1Receiver);
		pMeshRenderer->SetMaterial(1, matM16A1Furniture);

		pPrimaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);

		{
			GameObjectHandle hMagazine = CreateGameObject(L"PW Magazine");
			GameObject* pPWMagazine = hMagazine.ToPtr();
			pPWMagazine->m_transform.SetPosition(XMVectorSet(0.0, -0.032f, 0.068f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pPWMagazine->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			pMeshRenderer->SetMesh(meshSTANAG30Rds);
			// 재질 설정
			pMeshRenderer->SetMaterial(0, matSTANAG30Rds);

			pPWMagazine->m_transform.SetParent(&pPrimaryWeapon->m_transform);
		}
	}

	// Secondary weapon
	{
		GameObjectHandle hSecondaryWeapon = CreateGameObject(L"Secondary Weapon");
		pFirstPersonMovement->m_hWeapons[1] = hSecondaryWeapon;		// 2번 슬롯

		GameObject* pSecondaryWeapon = hSecondaryWeapon.ToPtr();
		pSecondaryWeapon->SetActive(false);	// 안보이게 비활성화 상태로 초기화
		pSecondaryWeapon->m_transform.SetPosition(XMVectorSet(0.1f, -0.14f, 0.28f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pSecondaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		auto meshM9A1 = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Weapons\\M9A1\\M9A1.obj").staticMeshes[0];
		pMeshRenderer->SetMesh(meshM9A1);
		// 재질 설정
		auto matM9A1 = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM9A1->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
		XMStoreFloat4A(&matM9A1->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.2f));
		XMStoreFloat4A(&matM9A1->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 4.0f));
		matM9A1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M9A1\\Textures\\M9A1_Diffuse.png");
		matM9A1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M9A1\\Textures\\M9A1_Normal.png");
		pMeshRenderer->SetMaterial(0, matM9A1);

		pSecondaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);

		{
			GameObjectHandle hWeaponLight = CreateGameObject(L"Weapon Light");
			GameObject* pWeaponLight = hWeaponLight.ToPtr();
			pWeaponLight->m_transform.SetPosition(XMVectorSet(0.0f, -0.004f, 0.028f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pWeaponLight->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			auto meshX300U = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Weapons\\X300U\\X300U.obj").staticMeshes[0];
			pMeshRenderer->SetMesh(meshX300U);
			// 재질 설정
			auto matX300U = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&matX300U->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
			XMStoreFloat4A(&matX300U->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.2f));
			XMStoreFloat4A(&matX300U->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.15f), 4.0f));
			matX300U->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\X300U\\Textures\\Body_Black_albedo.jpg");
			pMeshRenderer->SetMaterial(0, matX300U);

			pWeaponLight->m_transform.SetParent(&pSecondaryWeapon->m_transform);
		}
	}


	{
		GameObjectHandle hPrimaryWeapon = CreateGameObject(L"Primary Weapon");
		pFirstPersonMovement->m_hWeapons[2] = hPrimaryWeapon;		// 3번 슬롯

		GameObject* pPrimaryWeapon = hPrimaryWeapon.ToPtr();
		pPrimaryWeapon->SetActive(false);	// 안보이게 비활성화 상태로 초기화
		pPrimaryWeapon->m_transform.SetPosition(XMVectorSet(0.1f, -0.18f, 0.2f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		auto meshM4A1 = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Weapons\\M4A1\\M4A1.obj").staticMeshes[0];
		pMeshRenderer->SetMesh(meshM4A1);
		// 재질 설정
		auto matM4A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM4A1Receiver->m_ambient, XMVectorScale(Math::Vector3::One(), 0.85f));
		XMStoreFloat4A(&matM4A1Receiver->m_diffuse, XMVectorScale(Math::Vector3::One(), 1.0f));
		XMStoreFloat4A(&matM4A1Receiver->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 3.0f));
		matM4A1Receiver->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M4A1\\Textures\\Receiver_Diffuse.png");
		matM4A1Receiver->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M4A1\\Textures\\Receiver_Normal.png");
		auto matM4A1Furniture = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM4A1Furniture->m_ambient, XMVectorScale(Math::Vector3::One(), 0.1f));
		XMStoreFloat4A(&matM4A1Furniture->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.1f));
		XMStoreFloat4A(&matM4A1Furniture->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 4.0f));
		matM4A1Furniture->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M4A1\\Textures\\Furniture_Diffuse.png");
		matM4A1Furniture->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M4A1\\Textures\\Furniture_Normal.png");
		auto matKACRearSight = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matKACRearSight->m_ambient, XMVectorScale(Math::Vector3::One(), 0.1f));
		XMStoreFloat4A(&matKACRearSight->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.2f));
		XMStoreFloat4A(&matKACRearSight->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 4.0f));
		matKACRearSight->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M4A1\\Textures\\RearSight_Diffuse.png");
		matKACRearSight->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M4A1\\Textures\\RearSight_Normal.png");
		pMeshRenderer->SetMaterial(0, matM4A1Receiver);
		pMeshRenderer->SetMaterial(1, matM4A1Furniture);
		pMeshRenderer->SetMaterial(2, matKACRearSight);

		pPrimaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);

		{
			GameObjectHandle hMagazine = CreateGameObject(L"PW Magazine");
			GameObject* pPWMagazine = hMagazine.ToPtr();
			pPWMagazine->m_transform.SetPosition(XMVectorSet(0.0, -0.034f, 0.064f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pPWMagazine->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			pMeshRenderer->SetMesh(meshSTANAG30Rds);
			// 재질 설정
			pMeshRenderer->SetMaterial(0, matSTANAG30Rds);

			pPWMagazine->m_transform.SetParent(&pPrimaryWeapon->m_transform);
		}
	}


	{
		GameObjectHandle hPrimaryWeapon = CreateGameObject(L"Primary Weapon");
		pFirstPersonMovement->m_hWeapons[3] = hPrimaryWeapon;		// 3번 슬롯

		GameObject* pPrimaryWeapon = hPrimaryWeapon.ToPtr();
		pPrimaryWeapon->SetActive(false);	// 안보이게 비활성화 상태로 초기화
		pPrimaryWeapon->m_transform.SetPosition(XMVectorSet(0.1f, -0.205f, 0.14f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		auto mesh = ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Weapons\\M762\\m762.obj").staticMeshes[0];
		pMeshRenderer->SetMesh(mesh);
		// 재질 설정
		auto matReceiver = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matReceiver->m_ambient, XMVectorScale(Math::Vector3::One(), 0.35f));
		XMStoreFloat4A(&matReceiver->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.35f));
		XMStoreFloat4A(&matReceiver->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.1f), 4.0f));
		matReceiver->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M762\\Textures\\Receiver_Albedo.png");
		matReceiver->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M762\\Textures\\Receiver_Normal.png");
		auto matFurniture = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matFurniture->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
		XMStoreFloat4A(&matFurniture->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.2f));
		XMStoreFloat4A(&matFurniture->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.15f), 2.0f));
		matFurniture->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M762\\Textures\\Furniture_Albedo.png");
		matFurniture->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M762\\Textures\\Furniture_Normal.png");
		auto matRail = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matRail->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
		XMStoreFloat4A(&matRail->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.2f));
		XMStoreFloat4A(&matRail->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.1f), 4.0f));
		matRail->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M762\\Textures\\Rail_Albedo.png");
		matRail->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M762\\Textures\\Rail_Normal.png");
		
		pMeshRenderer->SetMaterial(0, matReceiver);
		pMeshRenderer->SetMaterial(1, matFurniture);
		pMeshRenderer->SetMaterial(2, matRail);

		pPrimaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);
		{
			GameObjectHandle hMagazine = CreateGameObject(L"PW Magazine");
			GameObject* pPWMagazine = hMagazine.ToPtr();
			pPWMagazine->m_transform.SetPosition(XMVectorSet(0.0, 0.0f, 0.169f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pPWMagazine->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();

			// 메시 설정

			std::shared_ptr<StaticMesh> meshMagazine =
				ResourceLoader::GetInstance()->LoadModel(L"Resource\\Models\\Weapons\\Magazine\\SteelStamped\\762SteelStampedMag.obj").staticMeshes[0];
			pMeshRenderer->SetMesh(meshMagazine);


			// 재질 설정
			auto matMagazine = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&matMagazine->m_ambient, XMVectorScale(Math::Vector3::One(), 0.05f));
			XMStoreFloat4A(&matMagazine->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.05f));
			XMStoreFloat4A(&matMagazine->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.1f), 4.0f));
			pMeshRenderer->SetMaterial(0, matMagazine);

			pPWMagazine->m_transform.SetParent(&pPrimaryWeapon->m_transform);
		}
	}



	// Skybox
	{
		Texture2D skybox = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\sky27.dds", false);
		RenderSettings::GetInstance()->SetSkybox(skybox);
	}
}
