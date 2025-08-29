#include "TestScene1.h"
#include "..\Script\FirstPersonMovement.h"
#include "..\Script\Planet.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(TestScene1)

void TestScene1::OnLoadScene()
{
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::shared_ptr<Material> parkinglotMat;

	{
		GameObjectHandle hTerrainObject = CreateGameObject(L"Terrain Object");
		GameObject* pTerrainObject = hTerrainObject.ToPtr();
		ComponentHandle<Terrain> hTerrain = pTerrainObject->AddComponent<Terrain>();
		uint16_t* pTempData = new uint16_t[1025 * 1025]();
		for (size_t i = 0; i < 1025 * 1025; ++i)
		{
			pTempData[i] = rand() % 4;
		}

		Texture2D heightMap;

		if (!ResourceLoader::GetInstance()->CreateHeightMapFromRawData(heightMap, pTempData, SIZE{ 1025, 1025 }))
			*reinterpret_cast<int*>(0) = 0;
		if (!hTerrain.ToPtr()->SetHeightMap(heightMap, 1.0f, 0.01f))
			*reinterpret_cast<int*>(0) = 0;
		Texture2D diffuseMapLayer = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Terrain\\vikenditerrain_d.dds");
		Texture2D normalMapLayer;
		hTerrain.ToPtr()->SetTextureLayer(diffuseMapLayer, normalMapLayer);
		delete[] pTempData;
	}

	UIObjectHandle hText1 = CreateText();
	Text* pText = static_cast<Text*>(hText1.ToPtr());
	pText->SetText(L"+");
	pText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
	pText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
	pText->SetColor(Colors::Red);
	pText->GetTextFormat().SetSize(20);
	pText->Refresh();

	UIObjectHandle hText2 = CreateText();
	Text* pText2 = static_cast<Text*>(hText2.ToPtr());
	pText2->SetText(L"30초 후 경기 구역이 제한됩니다!");
	pText2->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
	pText2->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::BOTTOM);
	pText2->m_transform.m_position.y = +200.0f;
	pText2->SetSize(XMFLOAT2(400, 50));
	pText2->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pText2->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pText2->SetColor(Colors::GreenYellow);
	pText2->GetTextFormat().SetSize(20);
	pText2->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_DEMI_BOLD);
	pText2->Refresh();

	{
		GameObjectHandle hSun = CreateGameObject(L"Sun");
		GameObject* pSun = hSun.ToPtr();
		pSun->m_transform.SetRotation(XMFLOAT3(XMConvertToRadians(45), XMConvertToRadians(30), 0));
		ComponentHandle<DirectionalLight> hLight = pSun->AddComponent<DirectionalLight>();
		DirectionalLight* pLight = hLight.ToPtr();
		pLight->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	}

	GameObjectHandle hMainCamera = CreateGameObject(L"Camera");
	GameObject* pMainCamera = hMainCamera.ToPtr();
	{
		pMainCamera->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, -5.0f));
		ComponentHandle<Camera> hMainCameraComponent = pMainCamera->AddComponent<Camera>();
		Camera* pMainCameraComponent = hMainCameraComponent.ToPtr();
		pMainCameraComponent->SetBackgroundColor(Colors::Gray);
		pMainCameraComponent->SetDepth(0);
		pMainCameraComponent->SetFieldOfView(92);
		pMainCameraComponent->SetClippingPlanes(0.1f, 500.0f);
		pMainCamera->AddComponent<FirstPersonMovement>();		// 1인칭 카메라 조작
	}

	{
		GameObjectHandle hRifle = CreateGameObject(L"Rifle");
		GameObject* pRifle = hRifle.ToPtr();
		pRifle->m_transform.SetPosition(XMFLOAT3A(0.0f, -0.2f, 0.15f));
		ComponentHandle<MeshRenderer> hRifleMeshRenderer = pRifle->AddComponent<MeshRenderer>();
		MeshRenderer* pRifleMeshRenderer = hRifleMeshRenderer.ToPtr();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Model\\weapons\\M16A1\\m16a1.obj");
		pRifleMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		auto material0 = ResourceLoader::GetInstance()->CreateMaterial();
		material0->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		material0->m_ambient = XMFLOAT4A(material0->m_diffuse.x * 0.2f, material0->m_diffuse.y * 0.2f, material0->m_diffuse.z * 0.2f, 1.0f);
		material0->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
		material0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Model\\weapons\\M16A1\\textures\\m16a1_0_Diffuse.jpg");

		auto material1 = ResourceLoader::GetInstance()->CreateMaterial();
		material1->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		material1->m_ambient = XMFLOAT4A(material1->m_diffuse.x * 0.2f, material1->m_diffuse.y * 0.2f, material1->m_diffuse.z * 0.2f, 1.0f);
		material1->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
		material1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Model\\weapons\\M16A1\\textures\\m16a1_1_Diffuse.jpg");
		pRifleMeshRenderer->SetMaterial(0, material0);
		pRifleMeshRenderer->SetMaterial(1, material1);

		pRifle->m_transform.SetParent(&pMainCamera->m_transform);
	}


	{
		GameObjectHandle hCamera2 = CreateGameObject(L"Camera2");
		GameObject* pCamera2 = hCamera2.ToPtr();
		pCamera2->m_transform.SetPosition(XMFLOAT3A(-5.5f, 2.5f, -2.5f));
		pCamera2->m_transform.SetRotation(XMFLOAT3A(0.0f, XMConvertToRadians(90), 0.0f));
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


	
	{
		GameObjectHandle hWall;
		GameObject* pWall;
		hWall = CreateGameObject(L"Wall");
		pWall = hWall.ToPtr();
		pWall->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, 0.0f));
		ComponentHandle< MeshRenderer> hKartMeshRenderer = pWall->AddComponent<MeshRenderer>();
		MeshRenderer* pKartMeshRenderer = hKartMeshRenderer.ToPtr();

		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Maps\\mart\\wall1.obj");
		pKartMeshRenderer->SetMesh(meshes[0]);

		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
		material->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
		material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 4.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Maps\\mart\\Wall1_Diffuse.png");
		pKartMeshRenderer->SetMaterial(0, material);
	}


	{
		GameObjectHandle hPlanet = CreateGameObject(L"AlienPlanet");
		GameObject* pPlanet = hPlanet.ToPtr();
		pPlanet->m_transform.SetPosition(XMFLOAT3A(-5.0f, 0.0f, 0.0f));
		auto hPlanetScript = pPlanet->AddComponent<Planet>();
		auto pPlanetScript = hPlanetScript.ToPtr();
		pPlanetScript->m_hCenter = hMainCamera;
		ComponentHandle<MeshRenderer> hPlanetMeshRenderer = pPlanet->AddComponent<MeshRenderer>();
		MeshRenderer* pPlanetMeshRenderer = hPlanetMeshRenderer.ToPtr();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Model\\planet\\RinglessPlanet.obj");
		pPlanetMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		material->m_ambient = XMFLOAT4A(material->m_diffuse.x * 0.2f, material->m_diffuse.y * 0.2f, material->m_diffuse.z * 0.2f, 1.0f);
		material->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Model\\planet\\RinglessPlanetB_Diffuse.png");
		pPlanetMeshRenderer->SetMaterial(0, material);
	}


	parkinglotMat = ResourceLoader::GetInstance()->CreateMaterial();
	parkinglotMat->m_diffuse = XMFLOAT4A(0.9f, 0.9f, 0.9f, 1.0f);
	parkinglotMat->m_ambient = XMFLOAT4A(0.15f, 0.15f, 0.15f, 1.0f);
	parkinglotMat->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
	parkinglotMat->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\maps\\mart\\ParkingLotInterior_Diffuse.png");
	{
		GameObjectHandle hPillar = CreateGameObject(L"Pillar");
		GameObject* pPillar = hPillar.ToPtr();
		ComponentHandle<MeshRenderer> hPillarMeshRenderer = pPillar->AddComponent<MeshRenderer>();
		MeshRenderer* pPillarMeshRenderer = hPillarMeshRenderer.ToPtr();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\maps\\mart\\pillar.obj");
		pPillarMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		pPillarMeshRenderer->SetMaterial(0, parkinglotMat);
	}


	{
		GameObjectHandle hBollard = CreateGameObject(L"Bollard");
		GameObject* pBollard = hBollard.ToPtr();
		pBollard->m_transform.SetPosition(XMFLOAT3A(-2.0f, 0.0f, 0.0f));
		ComponentHandle<MeshRenderer> hBollardMeshRenderer = pBollard->AddComponent<MeshRenderer>();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\maps\\mart\\bollard.obj");
		MeshRenderer* pBollardMeshRenderer = hBollardMeshRenderer.ToPtr();
		pBollardMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		pBollardMeshRenderer->SetMaterial(0, parkinglotMat);
	}

	Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\snowcube.dds", false);
	Environment::GetInstance()->SetSkyboxCubeMap(skyboxCubeMap);
}
