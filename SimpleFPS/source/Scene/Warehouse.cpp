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
		XMStoreFloat4A(&pLight->m_ambient, Math::Vector3::One());
		XMStoreFloat4A(&pLight->m_diffuse, Math::Vector3::One());
		XMStoreFloat4A(&pLight->m_specular, Math::Vector3::One());
	}

	// Main Camera
	GameObject* pMainCamera = nullptr;
	FirstPersonMovement* pFirstPersonMovement = nullptr;
	{
		GameObjectHandle hMainCamera = CreateGameObject(L"Main Camera");

		pMainCamera = hMainCamera.ToPtr();
		pMainCamera->m_transform.SetPosition(XMVectorSet(0.0f, 1.65f, -5.0f, 0.0f));

		ComponentHandle<Camera> hCameraComponent = pMainCamera->AddComponent<Camera>();
		Camera* pCameraComponent = hCameraComponent.ToPtr();
		pCameraComponent->SetBackgroundColor(Colors::Gray);
		pCameraComponent->SetDepth(0);
		pCameraComponent->SetFieldOfView(85);
		pCameraComponent->SetClippingPlanes(0.03f, 300.0f);

		ComponentHandle<FirstPersonMovement> hFirstPersonMovement = pMainCamera->AddComponent<FirstPersonMovement>();		// 1인칭 카메라 조작
		pFirstPersonMovement = hFirstPersonMovement.ToPtr();
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
		pWeaponIndicatorBgr->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Sprites\\WeaponIndicator.png"));
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
	std::shared_ptr<Mesh> meshClosedContainer = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Props\\ClosedContainer\\ClosedContainer.obj")[0];
	std::shared_ptr<Mesh> meshClosedLongContainer = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Props\\ClosedContainer\\ClosedLongContainer.obj")[0];
	std::shared_ptr<Mesh> meshOpenContainer1 = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Props\\OpenContainer\\OpenContainer1.obj")[0];
	std::shared_ptr<Mesh> meshOpenContainer2 = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Props\\OpenContainer\\OpenContainer2.obj")[0];
	std::shared_ptr<Mesh> meshHouseFrame = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Maps\\Warehouse\\HouseFrame\\HouseFrame.obj")[0];
	std::shared_ptr<Mesh> meshHouseSideWall = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Maps\\Warehouse\\HouseSideWall\\HouseSideWall.obj")[0];
	std::shared_ptr<Mesh> meshHouseFloor = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Maps\\Warehouse\\HouseFloor\\HouseFloor.obj")[0];
	std::shared_ptr<Mesh> meshBox = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Props\\Box\\Box.obj")[0];
	std::shared_ptr<Mesh> meshHouseWallSouth = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Maps\\Warehouse\\HouseWallSouth\\HouseWallSouth.obj")[0];
	std::shared_ptr<Mesh> meshHouseWallNorth = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Maps\\Warehouse\\HouseWallNorth\\HouseWallNorth.obj")[0];
	std::shared_ptr<Mesh> meshBlueTeamBase = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Maps\\Warehouse\\BlueTeamBase\\BlueTeamBase.obj")[0];
	std::shared_ptr<Mesh> meshRedTeamBase = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Maps\\Warehouse\\RedTeamBase\\RedTeamBase.obj")[0];
	std::shared_ptr<Mesh> meshHouseRoof = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Maps\\Warehouse\\HouseRoof\\HouseRoof.obj")[0];
	std::shared_ptr<Mesh> meshDoorFrame = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Maps\\Warehouse\\DoorFrame\\DoorFrame.obj")[0];


	Texture2D tex01murocrep512 = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\concrete\\01murocrep512.png");
	Texture2D tex03intonacorovinato2 = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\concrete\\03intonacorovinato2.png");
	Texture2D texWood4 = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\wood\\Wood4.png");
	Texture2D texWood5 = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\wood\\Wood5.png");
	Texture2D texBrick22 = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\bricks\\Brick 22 - 256x256.png");
	Texture2D texRoof24 = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Textures\\roof\\Roof 24 - 256x256.png");


	std::shared_ptr<Material> matYellowedConcrete = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matYellowedConcrete->m_ambient, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matYellowedConcrete->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matYellowedConcrete->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matYellowedConcrete->m_diffuseMap = tex03intonacorovinato2;


	std::shared_ptr<Material> matConcrete1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matConcrete1->m_ambient, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matConcrete1->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matConcrete1->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matConcrete1->m_diffuseMap = tex01murocrep512;


	std::shared_ptr<Material> matWood4 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matWood4->m_ambient, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matWood4->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matWood4->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matWood4->m_diffuseMap = texWood4;


	std::shared_ptr<Material> matWood5 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matWood5->m_ambient, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matWood5->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matWood5->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matWood5->m_diffuseMap = texWood5;


	std::shared_ptr<Material> matBrick22 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matBrick22->m_ambient, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matBrick22->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matBrick22->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matBrick22->m_diffuseMap = texBrick22;


	std::shared_ptr<Material> matRoof24 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matRoof24->m_ambient, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matRoof24->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
	XMStoreFloat4A(&matRoof24->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matRoof24->m_diffuseMap = texRoof24;


	std::shared_ptr<Material> matClosedContainer = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matClosedContainer->m_ambient, Math::Vector3::OneHalf());
	XMStoreFloat4A(&matClosedContainer->m_diffuse, Math::Vector3::OneHalf());
	XMStoreFloat4A(&matClosedContainer->m_specular, XMVectorSetW(Math::Vector3::OneHalf(), 4.0f));
	matClosedContainer->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Props\\ClosedContainer\\Diffuse.png");


	std::shared_ptr<Material> matOpenContainer = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matOpenContainer->m_ambient, Math::Vector3::OneHalf());
	XMStoreFloat4A(&matOpenContainer->m_diffuse, Math::Vector3::OneHalf());
	XMStoreFloat4A(&matOpenContainer->m_specular, XMVectorSetW(Math::Vector3::OneHalf(), 4.0f));
	matOpenContainer->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Props\\OpenContainer\\Diffuse.png");


	std::shared_ptr<Material> matHouseFrame = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matHouseFrame->m_ambient, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matHouseFrame->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matHouseFrame->m_specular, XMVectorSetW(Math::Vector3::OneHalf(), 32.0f));


	std::shared_ptr<Material> matHouseSideWall = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matHouseSideWall->m_ambient, Math::Vector3::One());
	XMStoreFloat4A(&matHouseSideWall->m_diffuse, Math::Vector3::One());
	XMStoreFloat4A(&matHouseSideWall->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matHouseSideWall->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Maps\\Warehouse\\HouseSideWall\\Textures\\Diffuse.png");


	std::shared_ptr<Material> matWoodenBox = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matWoodenBox->m_ambient, Math::Vector3::OneHalf());
	XMStoreFloat4A(&matWoodenBox->m_diffuse, Math::Vector3::One());
	XMStoreFloat4A(&matWoodenBox->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matWoodenBox->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Props\\Box\\WoodenBox.png");


	std::shared_ptr<Material> matPaperBox = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matPaperBox->m_ambient, Math::Vector3::OneHalf());
	XMStoreFloat4A(&matPaperBox->m_diffuse, Math::Vector3::One());
	XMStoreFloat4A(&matPaperBox->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matPaperBox->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Props\\Box\\PaperBox.png");


	std::shared_ptr<Material> matHouseWallSouth0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matHouseWallSouth0->m_ambient, Math::Vector3::OneHalf());
	XMStoreFloat4A(&matHouseWallSouth0->m_diffuse, Math::Vector3::OneHalf());
	XMStoreFloat4A(&matHouseWallSouth0->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
	matHouseWallSouth0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Maps\\Warehouse\\HouseWallSouth\\Diffuse.png");


	std::shared_ptr<Material> matHouseWallSouth1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matHouseWallSouth1->m_ambient, XMVectorScale(Math::Vector3::One(), 0.25f));
	XMStoreFloat4A(&matHouseWallSouth1->m_diffuse, Math::Vector3::OneHalf());
	XMStoreFloat4A(&matHouseWallSouth1->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));


	std::shared_ptr<Material> matHouseWallNorth[4];
	{
		matHouseWallNorth[0] = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matHouseWallNorth[0]->m_ambient, Math::Vector3::OneHalf());
		XMStoreFloat4A(&matHouseWallNorth[0]->m_diffuse, Math::Vector3::OneHalf());
		XMStoreFloat4A(&matHouseWallNorth[0]->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
		matHouseWallNorth[0]->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Maps\\Warehouse\\HouseWallNorth\\InsideDiffuse.png");

		matHouseWallNorth[1] = matYellowedConcrete;
		matHouseWallNorth[2] = matWood4;
		matHouseWallNorth[3] = matWood5;
	}
	

	std::shared_ptr<Material> matBlueTeamBase[5];
	{
		matBlueTeamBase[0] = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matBlueTeamBase[0]->m_ambient, XMVectorScale(Math::Vector3::One(), 0.75f));
		XMStoreFloat4A(&matBlueTeamBase[0]->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
		XMStoreFloat4A(&matBlueTeamBase[0]->m_specular, XMVectorSetW(Math::Vector3::Zero(), 4.0f));
		matBlueTeamBase[0]->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Maps\\Warehouse\\BlueTeamBase\\FloorDiffuse.png");

		matBlueTeamBase[1] = matConcrete1;
		matBlueTeamBase[2] = matYellowedConcrete;
		matBlueTeamBase[3] = matWood4;
		matBlueTeamBase[4] = matWood5;
	}
	

	std::shared_ptr<Material> matRedTeamBase[1];
	{
		matRedTeamBase[0] = matBrick22;
	}


	std::shared_ptr<Material> matDoorFrame = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matDoorFrame->m_ambient, XMVectorSet(0.1f, 0.025f, 0.0f, 0.0f));
	XMStoreFloat4A(&matDoorFrame->m_diffuse, XMVectorSet(0.1f, 0.025f, 0.0f, 0.0f));
	XMStoreFloat4A(&matDoorFrame->m_specular, XMVectorSetW(Math::Vector3::OneHalf(), 2.0f));



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

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseSideWall);
		pMeshRenderer->SetMaterial(0, matHouseSideWall);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Side Wall");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(14.1f, 0.0f, 0.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseSideWall);
		pMeshRenderer->SetMaterial(0, matHouseSideWall);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Wall South");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, -22.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseWallSouth);
		pMeshRenderer->SetMaterial(0, matHouseWallSouth0);
		pMeshRenderer->SetMaterial(1, matHouseWallSouth1);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Wall North");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, 25.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseWallNorth);
		pMeshRenderer->SetMaterial(0, matHouseWallNorth[0]);
		pMeshRenderer->SetMaterial(1, matHouseWallNorth[1]);
		pMeshRenderer->SetMaterial(2, matHouseWallNorth[2]);
		pMeshRenderer->SetMaterial(3, matHouseWallNorth[3]);
	}

	{
		GameObjectHandle hGameObject = CreateGameObject(L"Blue Team Base");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, 30.0f, 0.0f));

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshBlueTeamBase);
		pMeshRenderer->SetMaterial(0, matBlueTeamBase[0]);
		pMeshRenderer->SetMaterial(1, matBlueTeamBase[1]);
		pMeshRenderer->SetMaterial(2, matBlueTeamBase[2]);
		pMeshRenderer->SetMaterial(3, matBlueTeamBase[3]);
		pMeshRenderer->SetMaterial(4, matBlueTeamBase[4]);
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
		pMeshRenderer->SetMesh(meshHouseWallSouth);
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
		pMeshRenderer->SetMesh(meshHouseWallSouth);
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
		pMeshRenderer->SetMaterial(0, matConcrete1);
	}


	// TEST CODE
	// WEAPON
	// 
	// Primary weapon
	std::shared_ptr<Mesh> meshSTANAG30Rds = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\Magazine\\STANAG\\STANAG30Rds.obj")[0];
	auto matSTANAG30Rds = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matSTANAG30Rds->m_ambient, XMVectorScale(Math::Vector3::One(), 0.15f));
	XMStoreFloat4A(&matSTANAG30Rds->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.2f));
	XMStoreFloat4A(&matSTANAG30Rds->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.9f), 2.0f));
	{
		GameObjectHandle hPrimaryWeapon = CreateGameObject(L"Primary Weapon");
		pFirstPersonMovement->m_hWeapons[0] = hPrimaryWeapon;	// 1번 슬롯

		GameObject* pPrimaryWeapon = hPrimaryWeapon.ToPtr();
		pPrimaryWeapon->m_transform.SetPosition(XMVectorSet(0.12f, -0.18f, 0.22f, 0.0f));
		ComponentHandle<MeshRenderer> hRifleMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hRifleMeshRenderer.ToPtr();
		// 메시 설정
		auto meshM16A1 = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\M16A1\\M16A1.obj")[0];
		pMeshRenderer->SetMesh(meshM16A1);
		// 재질 설정
		auto matM16A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM16A1Receiver->m_ambient, Math::Vector3::OneHalf());
		XMStoreFloat4A(&matM16A1Receiver->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.5f));
		XMStoreFloat4A(&matM16A1Receiver->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 4.0f));
		matM16A1Receiver->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M16A1\\Textures\\M16A1Receiver_Diffuse.jpg");

		auto matM16A1Furniture = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM16A1Furniture->m_ambient, Math::Vector3::OneHalf());
		XMStoreFloat4A(&matM16A1Furniture->m_diffuse, Math::Vector3::One());
		XMStoreFloat4A(&matM16A1Furniture->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 4.0f));
		matM16A1Furniture->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M16A1\\Textures\\M16A1Furniture_Diffuse.jpg");
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
		pSecondaryWeapon->m_transform.SetPosition(XMVectorSet(0.12f, -0.18f, 0.32f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pSecondaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		auto meshM9A1 = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\M9A1\\M9A1.obj")[0];
		pMeshRenderer->SetMesh(meshM9A1);
		// 재질 설정
		auto matM9A1 = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM9A1->m_ambient, XMVectorScale(Math::Vector3::One(), 0.5f));
		XMStoreFloat4A(&matM9A1->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.5f));
		XMStoreFloat4A(&matM9A1->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 4.0f));
		matM9A1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M9A1\\Textures\\M9A1_Base_Color.png");
		pMeshRenderer->SetMaterial(0, matM9A1);

		pSecondaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);

		{
			GameObjectHandle hWeaponLight = CreateGameObject(L"Weapon Light");
			GameObject* pWeaponLight = hWeaponLight.ToPtr();
			pWeaponLight->m_transform.SetPosition(XMVectorSet(0.0f, -0.004f, 0.028f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pWeaponLight->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			auto meshX300U = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\X300U\\X300U.obj")[0];
			pMeshRenderer->SetMesh(meshX300U);
			// 재질 설정
			auto matX300U = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&matX300U->m_ambient, XMVectorScale(Math::Vector3::One(), 0.75f));
			XMStoreFloat4A(&matX300U->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.75f));
			XMStoreFloat4A(&matX300U->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 4.0f));
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
		pPrimaryWeapon->m_transform.SetPosition(XMVectorSet(0.12f, -0.18f, 0.22f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		auto meshM4A1 = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\M4A1\\M4A1.obj")[0];
		pMeshRenderer->SetMesh(meshM4A1);
		// 재질 설정
		auto matM4A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matM4A1Receiver->m_ambient, Math::Vector3::OneHalf());
		XMStoreFloat4A(&matM4A1Receiver->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.5f));
		XMStoreFloat4A(&matM4A1Receiver->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 4.0f));
		matM4A1Receiver->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M4A1\\Textures\\ReceiverDiffuse.png");
		auto matKACRearSight = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matKACRearSight->m_ambient, Math::Vector3::OneHalf());
		XMStoreFloat4A(&matKACRearSight->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.5f));
		XMStoreFloat4A(&matKACRearSight->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.05f), 4.0f));
		matKACRearSight->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M4A1\\Textures\\KACRearSightDiffuse.png");
		pMeshRenderer->SetMaterial(0, matM4A1Receiver);
		pMeshRenderer->SetMaterial(1, matKACRearSight);

		pPrimaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);

		{
			GameObjectHandle hMagazine = CreateGameObject(L"PW Magazine");
			GameObject* pPWMagazine = hMagazine.ToPtr();
			pPWMagazine->m_transform.SetPosition(XMVectorSet(0.0, -0.03f, 0.07f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pPWMagazine->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			pMeshRenderer->SetMesh(meshSTANAG30Rds);
			// 재질 설정
			pMeshRenderer->SetMaterial(0, matSTANAG30Rds);

			pPWMagazine->m_transform.SetParent(&pPrimaryWeapon->m_transform);
		}
	}


	// Skybox
	{
		Texture2D skybox = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\sunset.dds", false);
		RenderSettings::GetInstance()->SetSkybox(skybox);
	}
}
