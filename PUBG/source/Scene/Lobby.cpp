#include "Lobby.h"
#include "..\Script\TestScript.h"
#include "..\Script\CameraTestMovement.h"
#include "..\Script\SunScript.h"
#include "..\Script\FirstPersonCamera.h"

using namespace ze;
using namespace pubg;

ZE_IMPLEMENT_SCENE(Lobby);

void Lobby::OnLoadScene()
{
	GameObject* pGameObject;
	MeshRenderer* pMeshRenderer;
	Camera* pCamera;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::shared_ptr<Material> material;

	pGameObject = this->CreateGameObject(L"Sun");
	DirectionalLight* pLight = pGameObject->AddDeferredComponent<DirectionalLight>();
	pLight->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	pLight->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	pLight->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	pGameObject->AddDeferredComponent<SunScript>();

	pGameObject = this->CreateGameObject(L"Camera");
	pGameObject->GetTransform().m_position = XMFLOAT3A(0.0f, 0.0f, -5.0f);
	pCamera = pGameObject->AddDeferredComponent<Camera>();
	pCamera->SetBackgroundColor(Colors::DarkOliveGreen);
	pCamera->SetDepth(0);
	pCamera->SetFieldOfView(92);
	pGameObject->AddDeferredComponent<FirstPersonCamera>();		// 1인칭 카메라 조작

	pGameObject = this->CreateGameObject(L"Camera2");
	pGameObject->GetTransform().m_position = XMFLOAT3A(-5.5f, 5.0f, -2.5f);
	pGameObject->GetTransform().m_rotation = XMFLOAT4A(3.14f / 8.0f, 3.14f / 2.0f, 0.0f, 0.0f);
	pCamera = pGameObject->AddDeferredComponent<Camera>();
	pCamera->SetBackgroundColor(Colors::Chocolate);
	pCamera->SetDepth(1);
	pCamera->SetFieldOfView(70);
	pCamera->SetClippingPlanes(0.01f, 500.0f);
	pCamera->SetMaximumTessellationExponent(4.0f);
	pCamera->SetMinimumTessellationExponent(0.0f);
	pCamera->SetMaximumDistanceEndTessellation(300.0f);
	pCamera->SetMinimumDistanceStartTessellation(25.0f);
	pCamera->SetViewportRect(0.05f, 0.7f, 0.25f, 0.25f);
	
	pGameObject = this->CreateGameObject(L"Pillar");
	pMeshRenderer = pGameObject->AddDeferredComponent<MeshRenderer>();
	// 메시 설정
	meshes = Resource.CreateMesh(L"Resource\\maps\\mart\\pillar.obj");
	pMeshRenderer->m_mesh = meshes[0];
	// 재질 설정
	material = Resource.CreateMaterial();
	material->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	material->m_ambient = XMFLOAT4A(material->m_diffuse.x * 0.25f, material->m_diffuse.y * 0.25f, material->m_diffuse.z * 0.25f, 1.0f);
	material->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
	material->m_baseMap = Resource.CreateTexture(L"Resource\\maps\\mart\\ParkingLotInterior_Diffuse.png");
	pMeshRenderer->m_mesh->m_subsets[0].m_material = material;

	pGameObject = this->CreateGameObject(L"Bollard");
	pGameObject->GetTransform().m_position = XMFLOAT3A(-2.0f, 0.0f, 0.0f);
	pMeshRenderer = pGameObject->AddDeferredComponent<MeshRenderer>();
	// 메시 설정
	meshes = Resource.CreateMesh(L"Resource\\maps\\mart\\bollard.obj");
	pMeshRenderer->m_mesh = meshes[0];
	// 재질 설정
	pMeshRenderer->m_mesh->m_subsets[0].m_material = material;
	
	
	GameObject* pKart = this->CreateGameObject(L"KartBody");
	pKart->AddDeferredComponent<TestScript>();
	pKart->GetTransform().m_position = XMFLOAT3A(0.0f, 0.0f, 0.0f);
	pMeshRenderer = pKart->AddDeferredComponent<MeshRenderer>();
	
	meshes = Resource.CreateMesh(L"Resource\\Model\\newyorkmusclecar_body.obj");
	pMeshRenderer->m_mesh = meshes[0];
	
	material = Resource.CreateMaterial();
	material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
	material->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
	material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 55.0f);
	material->m_baseMap = Resource.CreateTexture(L"Resource\\Model\\newyorkmusclecar_tex.png");
	pMeshRenderer->m_mesh->m_subsets[0].m_material = material;

	std::shared_ptr<Skybox> sky = Resource.CreateSkybox(L"Resource\\Skybox\\cloudy_puresky.dds");
	// std::shared_ptr<Skybox> sky = Resource.CreateSkybox(L"Resource\\Skybox\\warm_restaurant_night.dds");
	// std::shared_ptr<Skybox> sky = Resource.CreateSkybox(L"Resource\\Skybox\\snowcube.dds");
	// std::shared_ptr<Skybox> sky = Resource.CreateSkybox(L"Resource\\Skybox\\sunsetcube.dds");
	Environment.SetSkybox(sky);

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
