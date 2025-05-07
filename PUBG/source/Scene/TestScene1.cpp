#include "TestScene1.h"
#include "..\Script\TestScript.h"
#include "..\Script\SunScript.h"
#include "..\Script\FirstPersonCamera.h"
#include "..\Script\SceneChange.h"

using namespace ze;
using namespace pubg;

ZE_IMPLEMENT_SCENE(TestScene1);

void TestScene1::OnLoadScene()
{
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::shared_ptr<Material> parkinglotMat;
	
	{
		GameObject* pSceneChanger = CreateGameObject(L"Scene Changer");
		pSceneChanger->AddDeferredComponent<SceneChange>();
	}


	{
		GameObject* pSun = CreateGameObject(L"Sun");
		DirectionalLight* pLight = pSun->AddDeferredComponent<DirectionalLight>();
		pLight->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pSun->AddDeferredComponent<SunScript>();
	}
	

	{
		GameObject* pCamera1 = CreateGameObject(L"Camera");
		pCamera1->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, -5.0f));
		Camera* pCamera1CamComponent = pCamera1->AddDeferredComponent<Camera>();
		pCamera1CamComponent->SetBackgroundColor(Colors::Gray);
		pCamera1CamComponent->SetDepth(0);
		pCamera1CamComponent->SetFieldOfView(92);
		pCamera1CamComponent->SetClippingPlanes(0.01f, 500.0f);
		pCamera1->AddDeferredComponent<FirstPersonCamera>();		// 1인칭 카메라 조작
	}
	

	{
		GameObject* pCamera2 = CreateGameObject(L"Camera2");
		pCamera2->m_transform.SetPosition(XMFLOAT3A(-5.5f, 2.5f, -2.5f));
		pCamera2->m_transform.SetRotation(XMFLOAT3A(0.0f, XMConvertToRadians(90), 0.0f));
		Camera* pCamera2CamComponent = pCamera2->AddDeferredComponent<Camera>();
		pCamera2CamComponent->SetBackgroundColor(Colors::Green);
		pCamera2CamComponent->SetDepth(1);
		pCamera2CamComponent->SetFieldOfView(70);
		pCamera2CamComponent->SetMaximumTessellationExponent(4.0f);
		pCamera2CamComponent->SetMinimumTessellationExponent(0.0f);
		pCamera2CamComponent->SetMaximumDistanceEndTessellation(300.0f);
		pCamera2CamComponent->SetMinimumDistanceStartTessellation(25.0f);
		pCamera2CamComponent->SetViewportRect(0.05f, 0.7f, 0.25f, 0.25f);
	}


	GameObject* pKart;
	{
		pKart = CreateGameObject(L"Kart");
		pKart->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, 0.0f));
		pKart->AddDeferredComponent<TestScript>();
		MeshRenderer* pKartMeshRenderer = pKart->AddDeferredComponent<MeshRenderer>();

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
		GameObject* pPlanet = CreateGameObject(L"AlienPlanet");
		pPlanet->m_transform.SetPosition(XMFLOAT3A(0.0f, 5.0f, 0.0f));
		pPlanet->m_transform.SetParent(&pKart->m_transform);

		MeshRenderer* pPlanetMeshRenderer = pPlanet->AddDeferredComponent<MeshRenderer>();
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
		GameObject* pPillar = CreateGameObject(L"Pillar");
		MeshRenderer* pPillarMeshRenderer = pPillar->AddDeferredComponent<MeshRenderer>();
		// 메시 설정
		meshes = Resource.LoadWavefrontOBJ(L"Resource\\maps\\mart\\pillar.obj");
		pPillarMeshRenderer->m_mesh = meshes[0];
		// 재질 설정
		pPillarMeshRenderer->m_mesh->m_subsets[0].m_material = parkinglotMat;
	}


	{
		GameObject* pBollard = CreateGameObject(L"Bollard");
		pBollard->m_transform.SetPosition(XMFLOAT3A(-2.0f, 0.0f, 0.0f));
		MeshRenderer* pBollardMeshRenderer = pBollard->AddDeferredComponent<MeshRenderer>();
		// 메시 설정
		meshes = Resource.LoadWavefrontOBJ(L"Resource\\maps\\mart\\bollard.obj");
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
