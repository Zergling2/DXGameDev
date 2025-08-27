#include "TestScene1.h"
#include "..\Script\TestScript.h"
#include "..\Script\SunScript.h"
#include "..\Script\FirstPersonCamera.h"
#include "..\Script\SceneChange.h"
#include "..\Script\Planet.h"

using namespace ze;
using namespace pubg;

ZE_IMPLEMENT_SCENE(TestScene1);

void TestScene1::OnLoadScene()
{
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::shared_ptr<Material> parkinglotMat;

	{
		GameObjectHandle hTerrainObject = CreateGameObject(L"Terrain Object");
		GameObject* pTerrainObject = hTerrainObject.ToPtr();
		ComponentHandle<Terrain> hTerrain = pTerrainObject->AddComponent<Terrain>();
		uint16_t* pTempData = new uint16_t[129 * 129];
		// ZeroMemory(pTempData, sizeof(uint16_t) * 129 * 129);
		for (int i = 0; i < 129; ++i)
		{
			for (int j = 0; j < 129; ++j)
			{
				pTempData[i * 129 + j] = rand() % 32;
			}
		}
		Texture2D heightMap;
		if (!ResourceLoader::GetInstance()->CreateHeightMapFromRawData(heightMap, pTempData, SIZE{ 129, 129 }))
			*reinterpret_cast<int*>(0) = 0;
		if (!hTerrain.ToPtr()->SetHeightMap(heightMap, 0.5f, 0.01f))
			*reinterpret_cast<int*>(0) = 0;
		Texture2D diffuseMapLayer = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Terrain\\vikenditerrain_d.dds");
		Texture2D normalMapLayer;
		hTerrain.ToPtr()->SetTextureLayer(diffuseMapLayer, normalMapLayer);
		delete[] pTempData;
	}

	

	UIObjectHandle hPanel = CreatePanel();
	Panel* pPanel = static_cast<Panel*>(hPanel.ToPtr());
	pPanel->SetSize(XMFLOAT2(600, 400));
	pPanel->SetColor(XMVectorSet(0.0f, 0.5f, 0.25f, 0.5f));
	pPanel->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
	pPanel->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
	pPanel->m_transform.m_position.x = 0.0f;
	pPanel->m_transform.m_position.y = 0.0f;
	pPanel->SetShape(PANEL_SHAPE::ROUNDED_RECTANGLE);
	{
		UIObjectHandle hButton = CreateButton();
		Button* pButton = static_cast<Button*>(hButton.ToPtr());
		pButton->SetSize(XMFLOAT2(80, 30));
		pButton->SetButtonColor(XMVectorSet(0.25f, 0.25f, 0.75f, 0.5f));
		pButton->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pButton->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pButton->m_transform.m_position.x = +90.0f;
		pButton->m_transform.m_position.y = +30.0f;
		pButton->SetText(L"Blue 버튼");
		pButton->SetColor(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		pButton->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pButton->m_transform.SetParent(&pPanel->m_transform);
	}
	{
		UIObjectHandle hButton = CreateButton();
		Button* pButton = static_cast<Button*>(hButton.ToPtr());
		pButton->SetSize(XMFLOAT2(80, 30));
		pButton->SetButtonColor(XMVectorSet(0.25f, 0.75f, 0.25f, 0.5f));
		pButton->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pButton->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pButton->m_transform.m_position.x = -200.0f;
		pButton->m_transform.m_position.y = +30.0f;
		pButton->SetText(L"Green 버튼");
		pButton->SetColor(XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		pButton->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pButton->m_transform.SetParent(&pPanel->m_transform);

		pButton->GetTextFormat().SetSize(16);
		pButton->Refresh();
	}
	{
		UIObjectHandle hButton = CreateButton();
		Button* pButton = static_cast<Button*>(hButton.ToPtr());
		pButton->SetSize(XMFLOAT2(80, 30));
		pButton->SetButtonColor(XMVectorSet(0.75f, 0.25f, 0.25f, 0.5f));
		pButton->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pButton->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pButton->m_transform.m_position.x = -110.0f;
		pButton->m_transform.m_position.y = -230.0f;
		pButton->SetText(L"Red 버튼");
		pButton->SetColor(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		pButton->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		pButton->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		pButton->m_transform.SetParent(&pPanel->m_transform);
	}
	pPanel->SetActive(false);

	{
		UIObjectHandle hImage = CreateImage();
		Image* pImage = static_cast<Image*>(hImage.ToPtr());
		Texture2D tex = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\test.png");
		pImage->SetTexture(tex);
		pImage->SetNativeSize(true);
		pImage->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::RIGHT);
		pImage->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::TOP);
		pImage->m_transform.m_position = pImage->GetHalfSize();
		pImage->m_transform.m_position.x *= -1.0f;
		pImage->m_transform.m_position.y *= -1.0f;
	}


	{
		GameObjectHandle hSceneChanger = CreateGameObject(L"Scene Changer");
		hSceneChanger.ToPtr()->AddComponent<SceneChange>();
	}


	{
		GameObjectHandle hSun = CreateGameObject(L"Sun");
		GameObject* pSun = hSun.ToPtr();
		ComponentHandle<DirectionalLight> hLight = pSun->AddComponent<DirectionalLight>();
		DirectionalLight* pLight = hLight.ToPtr();
		pLight->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pSun->AddComponent<SunScript>();
	}
	

	{
		GameObjectHandle hCamera1 = CreateGameObject(L"Camera");
		GameObject* pCamera1 = hCamera1.ToPtr();
		pCamera1->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, -5.0f));
		ComponentHandle<Camera> hCamera1CamComponent = pCamera1->AddComponent<Camera>();
		Camera* pCamera1CamComponent = hCamera1CamComponent.ToPtr();
		pCamera1CamComponent->SetBackgroundColor(Colors::Gray);
		pCamera1CamComponent->SetDepth(0);
		pCamera1CamComponent->SetFieldOfView(92);
		pCamera1CamComponent->SetClippingPlanes(0.3f, 500.0f);
		ComponentHandle<FirstPersonCamera> hFPSCam = pCamera1->AddComponent<FirstPersonCamera>();		// 1인칭 카메라 조작
		hFPSCam.ToPtr()->m_hPanel = hPanel;
	}
	

	{
		GameObjectHandle hCamera2 = CreateGameObject(L"Camera2");
		GameObject* pCamera2 = hCamera2.ToPtr();
		pCamera2->m_transform.SetPosition(XMFLOAT3A(-5.5f, 2.5f, -2.5f));
		pCamera2->m_transform.SetRotation(XMFLOAT3A(0.0f, XMConvertToRadians(90), 0.0f));
		ComponentHandle< Camera> hCamera2CamComponent = pCamera2->AddComponent<Camera>();
		Camera* pCamera2CamComponent = hCamera2CamComponent.ToPtr();
		pCamera2CamComponent->SetBackgroundColor(Colors::Green);
		pCamera2CamComponent->SetDepth(1);
		pCamera2CamComponent->SetFieldOfView(70);
		pCamera2CamComponent->SetMaximumTessellationExponent(4.0f);
		pCamera2CamComponent->SetMinimumTessellationExponent(0.0f);
		pCamera2CamComponent->SetMaximumDistanceEndTessellation(300.0f);
		pCamera2CamComponent->SetMinimumDistanceStartTessellation(25.0f);
		pCamera2CamComponent->SetViewportRect(0.05f, 0.7f, 0.25f, 0.25f);
	}


	GameObjectHandle hKart;
	GameObject* pKart;
	{
		hKart = CreateGameObject(L"Kart");
		pKart = hKart.ToPtr();
		pKart->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, 0.0f));
		pKart->AddComponent<TestScript>();
		ComponentHandle< MeshRenderer> hKartMeshRenderer = pKart->AddComponent<MeshRenderer>();
		MeshRenderer* pKartMeshRenderer = hKartMeshRenderer.ToPtr();

		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Model\\newyorkmusclecar\\newyorkmusclecar_body.obj");
		pKartMeshRenderer->m_mesh = meshes[0];

		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
		material->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
		material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 55.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Model\\newyorkmusclecar\\newyorkmusclecar_tex.png");
		pKartMeshRenderer->m_mesh->m_subsets[0].m_material = material;
	}

	
	{
		GameObjectHandle hPlanet = CreateGameObject(L"AlienPlanet");
		GameObject* pPlanet = hPlanet.ToPtr();
		pPlanet->m_transform.SetPosition(XMFLOAT3A(-5.0f, 0.0f, 0.0f));
		auto hPlanetScript = pPlanet->AddComponent<Planet>();
		auto pPlanetScript = hPlanetScript.ToPtr();
		pPlanetScript->m_hCenter = hKart;
		ComponentHandle<MeshRenderer> hPlanetMeshRenderer = pPlanet->AddComponent<MeshRenderer>();
		MeshRenderer* pPlanetMeshRenderer = hPlanetMeshRenderer.ToPtr();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Model\\planet\\RinglessPlanet.obj");
		pPlanetMeshRenderer->m_mesh = meshes[0];
		// 재질 설정
		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		material->m_ambient = XMFLOAT4A(material->m_diffuse.x * 0.2f, material->m_diffuse.y * 0.2f, material->m_diffuse.z * 0.2f, 1.0f);
		material->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Model\\planet\\RinglessPlanetB_Diffuse.png");
		pPlanetMeshRenderer->m_mesh->m_subsets[0].m_material = material;
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
		pPillarMeshRenderer->m_mesh = meshes[0];
		// 재질 설정
		pPillarMeshRenderer->m_mesh->m_subsets[0].m_material = parkinglotMat;
	}


	{
		GameObjectHandle hBollard = CreateGameObject(L"Bollard");
		GameObject* pBollard = hBollard.ToPtr();
		pBollard->m_transform.SetPosition(XMFLOAT3A(-2.0f, 0.0f, 0.0f));
		ComponentHandle<MeshRenderer> hBollardMeshRenderer = pBollard->AddComponent<MeshRenderer>();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\maps\\mart\\bollard.obj");
		MeshRenderer* pBollardMeshRenderer = hBollardMeshRenderer.ToPtr();
		pBollardMeshRenderer->m_mesh = meshes[0];
		// 재질 설정
		pBollardMeshRenderer->m_mesh->m_subsets[0].m_material = parkinglotMat;
	}



	Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\cloudy_puresky.dds", false);
	// Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\warm_restaurant_night.dds", false);
	// Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\snowcube.dds", false);
	// Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\sunsetcube.dds", false);
	Environment::GetInstance()->SetSkyboxCubeMap(skyboxCubeMap);
}
