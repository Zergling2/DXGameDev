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
		UIObjectHandle hGreenPanel = CreatePanel();
		Panel* pGreenPanel = static_cast<Panel*>(hGreenPanel.ToPtr());
		pGreenPanel->SetSize(XMFLOAT2(600, 600));
		pGreenPanel->SetColor(XMVectorSet(0.25f, 0.75f, 0.25f, 0.5f));
		pGreenPanel->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
		pGreenPanel->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
		pGreenPanel->m_transform.m_position.x = 0.0f;
		pGreenPanel->m_transform.m_position.y = 0.0f;
	}

	{
		UIObjectHandle hImage = CreateImage();
		Image* pImage = static_cast<Image*>(hImage.ToPtr());
		pImage->SetSize(XMFLOAT2(1280, 720));
		Texture2D tex = Resource.LoadTexture(L"Resource\\test.png");
		pImage->SetTexture(tex);
		pImage->SetNativeSize(true);
		pImage->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::RIGHT);
		pImage->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::TOP);
		pImage->m_transform.m_position.x = -300;
		pImage->m_transform.m_position.y = -100;
	}

	{
		UIObjectHandle hButtonA = CreateButton();
		Button* pButtonA = static_cast<Button*>(hButtonA.ToPtr());
		pButtonA->SetSize(XMFLOAT2(200, 40));
		pButtonA->SetColor(XMVectorSet(0.75f, 0.25f, 0.25f, 0.5f));
		pButtonA->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::RIGHT);
		pButtonA->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::BOTTOM);
		pButtonA->m_transform.m_position.x = -110.0f;
		pButtonA->m_transform.m_position.y = +30.0f;
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
		pCamera1CamComponent->SetClippingPlanes(0.01f, 500.0f);
		pCamera1->AddComponent<FirstPersonCamera>();		// 1인칭 카메라 조작
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

		meshes = Resource.LoadWavefrontOBJ(L"Resource\\Model\\newyorkmusclecar\\newyorkmusclecar_body.obj");
		pKartMeshRenderer->m_mesh = meshes[0];

		auto material = Resource.CreateMaterial();
		material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
		material->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
		material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 55.0f);
		material->m_diffuseMap = Resource.LoadTexture(L"Resource\\Model\\newyorkmusclecar\\newyorkmusclecar_tex.png");
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
		meshes = Resource.LoadWavefrontOBJ(L"Resource\\Model\\planet\\RinglessPlanet.obj");
		pPlanetMeshRenderer->m_mesh = meshes[0];
		// 재질 설정
		auto material = Resource.CreateMaterial();
		material->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		material->m_ambient = XMFLOAT4A(material->m_diffuse.x * 0.2f, material->m_diffuse.y * 0.2f, material->m_diffuse.z * 0.2f, 1.0f);
		material->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
		material->m_diffuseMap = Resource.LoadTexture(L"Resource\\Model\\planet\\RinglessPlanetB_Diffuse.png");
		pPlanetMeshRenderer->m_mesh->m_subsets[0].m_material = material;
	}


	parkinglotMat = Resource.CreateMaterial();
	parkinglotMat->m_diffuse = XMFLOAT4A(0.9f, 0.9f, 0.9f, 1.0f);
	parkinglotMat->m_ambient = XMFLOAT4A(0.15f, 0.15f, 0.15f, 1.0f);
	parkinglotMat->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
	parkinglotMat->m_diffuseMap = Resource.LoadTexture(L"Resource\\maps\\mart\\ParkingLotInterior_Diffuse.png");
	{
		GameObjectHandle hPillar = CreateGameObject(L"Pillar");
		GameObject* pPillar = hPillar.ToPtr();
		ComponentHandle<MeshRenderer> hPillarMeshRenderer = pPillar->AddComponent<MeshRenderer>();
		MeshRenderer* pPillarMeshRenderer = hPillarMeshRenderer.ToPtr();
		// 메시 설정
		meshes = Resource.LoadWavefrontOBJ(L"Resource\\maps\\mart\\pillar.obj");
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
		meshes = Resource.LoadWavefrontOBJ(L"Resource\\maps\\mart\\bollard.obj");
		MeshRenderer* pBollardMeshRenderer = hBollardMeshRenderer.ToPtr();
		pBollardMeshRenderer->m_mesh = meshes[0];
		// 재질 설정
		pBollardMeshRenderer->m_mesh->m_subsets[0].m_material = parkinglotMat;
	}


	Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\cloudy_puresky.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\warm_restaurant_night.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\snowcube.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\sunsetcube.dds");
	Environment.SetSkyboxCubeMap(skyboxCubeMap);


	// TerrainData tmd;
	// tmd.heightMapSize.row = 1025;
	// tmd.heightMapSize.column = 1025;
	// tmd.cellSpacing = 1.0f;
	// tmd.heightBase = 0.0f;
	// tmd.heightScale = 100.0f;
	// tmd.textureScale = 32.0f;
	// tmd.elementFormat = HEIGHT_MAP_FORMAT::RAW_16BIT;
	// std::shared_ptr<Terrain> terrain = Resource.LoadTerrain(L"Resource\\Terrain\\RidgeThrough.r16", tmd);
	// std::shared_ptr<Terrain> terrain = Resource.LoadTerrain(L"Resource\\Terrain\\RollingHills.r16", tmd);

	// TerrainData tmd;
	// tmd.heightMapSize.row = 4097;
	// tmd.heightMapSize.column = 4097;
	// tmd.cellSpacing = 1.0f;
	// tmd.heightBase = -100.0f;
	// tmd.heightScale = 200.0f;
	// tmd.textureScale = 32.0f;
	// tmd.elementFormat = HEIGHT_MAP_FORMAT::RAW_16BIT;
	// std::shared_ptr<Terrain> terrain = Resource.LoadTerrain(L"Resource\\Terrain\\RockyPeaks.r16", tmd);

	// SetTerrain(terrain);
}
