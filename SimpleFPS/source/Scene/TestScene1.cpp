#include "TestScene1.h"
#include "..\Script\FirstPersonMovement.h"
#include "..\Script\Planet.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(TestScene1)

void TestScene1::OnLoadScene()
{
	std::vector<std::shared_ptr<Mesh>> meshes;

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
	

	{
		GameObjectHandle hSun = CreateGameObject(L"Sun");
		GameObject* pSun = hSun.ToPtr();
		pSun->m_transform.SetRotationEuler(XMVectorSet(XMConvertToRadians(45), XMConvertToRadians(30), 0.0f, 0.0f));
		ComponentHandle<DirectionalLight> hLight = pSun->AddComponent<DirectionalLight>();
		DirectionalLight* pLight = hLight.ToPtr();
		pLight->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	}

	GameObjectHandle hMainCamera = CreateGameObject(L"Camera");
	ComponentHandle<FirstPersonMovement> hFirstPersonMovement;
	GameObject* pMainCamera = hMainCamera.ToPtr();
	{
		pMainCamera->m_transform.SetPosition(XMVectorSet(0.0f, 1.7f, -5.0f, 0.0f));
		ComponentHandle<Camera> hMainCameraComponent = pMainCamera->AddComponent<Camera>();
		Camera* pMainCameraComponent = hMainCameraComponent.ToPtr();
		pMainCameraComponent->SetBackgroundColor(Colors::Gray);
		pMainCameraComponent->SetDepth(0);
		pMainCameraComponent->SetFieldOfView(85);
		pMainCameraComponent->SetClippingPlanes(0.03f, 300.0f);
		hFirstPersonMovement = pMainCamera->AddComponent<FirstPersonMovement>();		// 1인칭 카메라 조작
	}

	{
		GameObjectHandle hPrimaryWeapon = CreateGameObject(L"Primary Weapon");
		hFirstPersonMovement.ToPtr()->m_hWeapons[0] = hPrimaryWeapon;	// 1번 슬롯

		GameObject* pPrimaryWeapon = hPrimaryWeapon.ToPtr();
		pPrimaryWeapon->m_transform.SetPosition(XMVectorSet(0.12f, -0.18f, 0.22f, 0.0f));
		ComponentHandle<MeshRenderer> hRifleMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hRifleMeshRenderer.ToPtr();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\M16A1\\M16A1.obj");
		pMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		auto material0 = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&material0->m_diffuse, Math::Vector3::One());
		XMStoreFloat4A(&material0->m_ambient, Math::Vector3::OneHalf());
		XMStoreFloat4A(&material0->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.5f), 4.0f));
		material0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M16A1\\Textures\\M16A1Receiver_Diffuse.jpg");
	
		auto material1 = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&material1->m_diffuse, Math::Vector3::One());
		XMStoreFloat4A(&material1->m_ambient, Math::Vector3::OneHalf());
		XMStoreFloat4A(&material1->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.25f), 4.0f));
		material1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M16A1\\Textures\\M16A1Furniture_Diffuse.jpg");
		pMeshRenderer->SetMaterial(0, material0);
		pMeshRenderer->SetMaterial(1, material1);
	
		pPrimaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);
	
		{
			GameObjectHandle hMagazine = CreateGameObject(L"PW Magazine");
			GameObject* pPWMagazine = hMagazine.ToPtr();
			pPWMagazine->m_transform.SetPosition(XMVectorSet(0.0, -0.032f, 0.068f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pPWMagazine->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\Magazine\\STANAG\\STANAG30Rds.obj");
			pMeshRenderer->SetMesh(meshes[0]);
			// 재질 설정
			auto material0 = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&material0->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.15f));
			XMStoreFloat4A(&material0->m_ambient, XMVectorScale(Math::Vector3::One(), 0.15f));
			XMStoreFloat4A(&material0->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.8f), 4.0f));
			pMeshRenderer->SetMaterial(0, material0);
			
			pPWMagazine->m_transform.SetParent(&pPrimaryWeapon->m_transform);
		}
	}

	{
		GameObjectHandle hPrimaryWeapon = CreateGameObject(L"Primary Weapon");
		hFirstPersonMovement.ToPtr()->m_hWeapons[1] = hPrimaryWeapon;		// 2번 슬롯

		GameObject* pPrimaryWeapon = hPrimaryWeapon.ToPtr();
		pPrimaryWeapon->SetActive(false);	// 안보이게 비활성화 상태로 초기화
		pPrimaryWeapon->m_transform.SetPosition(XMVectorSet(0.12f, -0.18f, 0.22f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\M4A1\\M4A1.obj");
		pMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		auto material0 = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&material0->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.8f));
		XMStoreFloat4A(&material0->m_ambient, XMVectorScale(Math::Vector3::One(), 0.25f));
		XMStoreFloat4A(&material0->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.5f), 4.0f));
		material0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M4A1\\Textures\\diff.tga");
		pMeshRenderer->SetMaterial(0, material0);
	
		pPrimaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);
	
		{
			GameObjectHandle hRearSight = CreateGameObject(L"Rear Sight");
			GameObject* pRearSight = hRearSight.ToPtr();
			pRearSight->m_transform.SetPosition(XMVectorSet(0.0f, 0.104f, -0.037f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pRearSight->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\Sights\\KACRearSight\\KACRearSight.obj");
			pMeshRenderer->SetMesh(meshes[0]);
			// 재질 설정
			auto material0 = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&material0->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.8f));
			XMStoreFloat4A(&material0->m_ambient, XMVectorScale(Math::Vector3::One(), 0.25f));
			XMStoreFloat4A(&material0->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.5f), 4.0f));
			material0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\Sights\\KACRearSight\\Textures\\Albedo.png");
			pMeshRenderer->SetMaterial(0, material0);
	
			pRearSight->m_transform.SetParent(&pPrimaryWeapon->m_transform);
		}
	
		{
			GameObjectHandle hMagazine = CreateGameObject(L"PW Magazine");
			GameObject* pPWMagazine = hMagazine.ToPtr();
			pPWMagazine->m_transform.SetPosition(XMVectorSet(0.0, -0.03f, 0.07f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pPWMagazine->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\Magazine\\STANAG\\STANAG30Rds.obj");
			pMeshRenderer->SetMesh(meshes[0]);
			// 재질 설정
			auto material0 = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&material0->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.15f));
			XMStoreFloat4A(&material0->m_ambient, XMVectorScale(Math::Vector3::One(), 0.15f));
			XMStoreFloat4A(&material0->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.8f), 4.0f));
			pMeshRenderer->SetMaterial(0, material0);

			pPWMagazine->m_transform.SetParent(&pPrimaryWeapon->m_transform);
		}
	}

	{
		GameObjectHandle hPrimaryWeapon = CreateGameObject(L"Primary Weapon");
		hFirstPersonMovement.ToPtr()->m_hWeapons[2] = hPrimaryWeapon;	// 3번 슬롯

		GameObject* pPrimaryWeapon = hPrimaryWeapon.ToPtr();
		pPrimaryWeapon->SetActive(false);	// 안보이게 비활성화 상태로 초기화
		pPrimaryWeapon->m_transform.SetPosition(XMVectorSet(0.12f, -0.18f, 0.22f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pPrimaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\AKM\\AKM.obj");
		pMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		auto matReceiver = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matReceiver->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.8f));
		XMStoreFloat4A(&matReceiver->m_ambient, XMVectorScale(Math::Vector3::One(), 0.5f));
		XMStoreFloat4A(&matReceiver->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.5f), 4.0f));
		matReceiver->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\AKM\\Textures\\Receiver_Albedo.png");
	
		auto matFurniture = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matFurniture->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.8f));
		XMStoreFloat4A(&matFurniture->m_ambient, XMVectorScale(Math::Vector3::One(), 0.5f));
		XMStoreFloat4A(&matFurniture->m_specular, XMVectorSetW(XMVectorZero(), 1.0f));
		matFurniture->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\AKM\\Textures\\WoodenFurniture_Albedo.png");
		pMeshRenderer->SetMaterial(0, matReceiver);
		pMeshRenderer->SetMaterial(1, matFurniture);
	
		pPrimaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);
	
		{
			GameObjectHandle hMagazine = CreateGameObject(L"PW Magazine");
			GameObject* pPWMagazine = hMagazine.ToPtr();
			pPWMagazine->m_transform.SetPosition(XMVectorSet(0.0, -0.04f, 0.106f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pPWMagazine->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\Magazine\\Bakelite\\BakeliteMag30Rds.obj");
			pMeshRenderer->SetMesh(meshes[0]);
			// 재질 설정
			auto matMagazine = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&matMagazine->m_diffuse, XMVectorScale(Math::Vector3::One(), 0.8f));
			XMStoreFloat4A(&matMagazine->m_ambient, XMVectorScale(Math::Vector3::One(), 0.4f));
			XMStoreFloat4A(&matMagazine->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.6f), 8.0f));
			matMagazine->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\Magazine\\Bakelite\\Textures\\BakeliteMag30rds_Albedo.png");
			pMeshRenderer->SetMaterial(0, matMagazine);
	
			pPWMagazine->m_transform.SetParent(&pPrimaryWeapon->m_transform);
		}
	}


	{
		GameObjectHandle hSecondaryWeapon = CreateGameObject(L"Secondary Weapon");
		hFirstPersonMovement.ToPtr()->m_hWeapons[3] = hSecondaryWeapon;		// 4번 슬롯

		GameObject* pSecondaryWeapon = hSecondaryWeapon.ToPtr();
		pSecondaryWeapon->SetActive(false);	// 안보이게 비활성화 상태로 초기화
		pSecondaryWeapon->m_transform.SetPosition(XMVectorSet(0.12f, -0.18f, 0.32f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pSecondaryWeapon->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\M9A1\\M9A1.obj");
		pMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		auto material0 = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&material0->m_diffuse, Math::Vector3::One());
		XMStoreFloat4A(&material0->m_ambient, Math::Vector3::OneHalf());
		XMStoreFloat4A(&material0->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.5f), 4.0f));
		material0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\M9A1\\Textures\\M9A1_Base_Color.png");
		pMeshRenderer->SetMaterial(0, material0);
	
		pSecondaryWeapon->m_transform.SetParent(&pMainCamera->m_transform);
	
		{
			GameObjectHandle hWeaponLight = CreateGameObject(L"Weapon Light");
			GameObject* pWeaponLight = hWeaponLight.ToPtr();
			pWeaponLight->m_transform.SetPosition(XMVectorSet(0.0f, -0.004f, 0.028f, 0.0f));
			ComponentHandle<MeshRenderer> hMeshRenderer = pWeaponLight->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
			// 메시 설정
			meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Weapons\\X300U\\X300U.obj");
			pMeshRenderer->SetMesh(meshes[0]);
			// 재질 설정
			auto material0 = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&material0->m_diffuse, Math::Vector3::One());
			XMStoreFloat4A(&material0->m_ambient, Math::Vector3::OneHalf());
			XMStoreFloat4A(&material0->m_specular, XMVectorSetW(XMVectorScale(Math::Vector3::One(), 0.1f), 4.0f));
			material0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Weapons\\X300U\\Textures\\Body_Black_albedo.jpg");
			pMeshRenderer->SetMaterial(0, material0);
	
			pWeaponLight->m_transform.SetParent(&pSecondaryWeapon->m_transform);
		}
	}


	/*
	{
		GameObjectHandle hCamera2 = CreateGameObject(L"Camera2");
		GameObject* pCamera2 = hCamera2.ToPtr();
		pCamera2->m_transform.SetPosition(XMVectorSet(-5.5f, 2.5f, -2.5f, 0.0f));
		pCamera2->m_transform.SetRotationEuler(XMVectorSet(0.0f, XMConvertToRadians(90), 0.0f, 0.0f));
		ComponentHandle<Camera> hCamera2CamComponent = pCamera2->AddComponent<Camera>();
		Camera* pCamera2CamComponent = hCamera2CamComponent.ToPtr();
		pCamera2CamComponent->SetBackgroundColor(Colors::Green);
		pCamera2CamComponent->SetDepth(1);
		pCamera2CamComponent->SetFieldOfView(70);
		pCamera2CamComponent->SetMaximumTessellationExponent(4.0f);
		pCamera2CamComponent->SetMinimumTessellationExponent(0.0f);
		pCamera2CamComponent->SetMaximumDistanceEndTessellation(300.0f);
		pCamera2CamComponent->SetMinimumDistanceStartTessellation(25.0f);
		pCamera2CamComponent->SetViewportRect(0.7f, 0.7f, 0.25f, 0.25f);
	}
	*/


	{
		GameObjectHandle hPTBox;
		GameObject* pPTBox;
		hPTBox = CreateGameObject(L"PTBox");
		pPTBox = hPTBox.ToPtr();
		pPTBox->m_transform.SetPosition(XMVectorSet(1.0f, 0.0f, 0.5f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pPTBox->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();

		// meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Models\\test\\cube_pt.obj");
		meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\test\\cube_pt.obj");
		pMeshRenderer->SetMesh(meshes[0]);

		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
		material->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
		material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 4.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Maps\\mart\\Wall1_Diffuse.png");
		pMeshRenderer->SetMaterial(0, material);
	}


	{
		GameObjectHandle hContainer;
		GameObject* pContainer;
		hContainer = CreateGameObject(L"Container");
		pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, 6.0f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();

		meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Props\\OpenContainer\\OpenContainer2.obj");
		pMeshRenderer->SetMesh(meshes[0]);

		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_ambient = XMFLOAT4A(0.4f, 0.4f, 0.4f, 1.0f);
		material->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
		material->m_specular = XMFLOAT4A(0.75f, 0.75f, 0.75f, 4.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Props\\OpenContainer\\Diffuse.png");
		pMeshRenderer->SetMaterial(0, material);
	}

	{
		GameObjectHandle hContainer;
		GameObject* pContainer;
		hContainer = CreateGameObject(L"Container");
		pContainer = hContainer.ToPtr();
		pContainer->m_transform.SetPosition(XMVectorSet(-4.5f, 0.0f, 10.0f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pContainer->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();

		meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Models\\Props\\ClosedContainer\\ClosedContainer.obj");
		pMeshRenderer->SetMesh(meshes[0]);

		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_ambient = XMFLOAT4A(0.4f, 0.4f, 0.4f, 1.0f);
		material->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
		material->m_specular = XMFLOAT4A(0.75f, 0.75f, 0.75f, 4.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Models\\Props\\ClosedContainer\\Diffuse.png");
		pMeshRenderer->SetMaterial(0, material);
	}

	{
		GameObjectHandle hWall;
		GameObject* pWall;
		hWall = CreateGameObject(L"Wall");
		pWall = hWall.ToPtr();
		pWall->m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pWall->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();

		// meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Maps\\mart\\wall1.obj");
		meshes = ResourceLoader::GetInstance()->LoadMesh(L"Resource\\Maps\\mart\\wall1.obj");
		pMeshRenderer->SetMesh(meshes[0]);

		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
		material->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
		material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 4.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Maps\\mart\\Wall1_Diffuse.png");
		pMeshRenderer->SetMaterial(0, material);
	}

	{
		GameObjectHandle hRange = CreateGameObject(L"Range");
		GameObject* pRange = hRange.ToPtr();
		ComponentHandle<MeshRenderer> hRangeMeshRenderer = pRange->AddComponent<MeshRenderer>();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Maps\\stage1\\range.obj");
		MeshRenderer* pMeshRenderer = hRangeMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		auto material = ResourceLoader::GetInstance()->CreateMaterial();

		XMStoreFloat4A(&material->m_diffuse, XMVectorReplicate(0.8f));	// 모래 느낌
		material->m_diffuse.w = 1.0f;
		XMStoreFloat4A(&material->m_specular, XMVectorZero());	// 모래 느낌
		material->m_specular.w = 1.0f;
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Maps\\stage1\\Limestone.jpg");
		pMeshRenderer->SetMaterial(0, material);
	}

	Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\indigo.dds", false);
	Environment::GetInstance()->SetSkyboxCubeMap(skyboxCubeMap);
}
