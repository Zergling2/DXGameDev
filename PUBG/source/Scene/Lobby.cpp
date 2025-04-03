#include "Lobby.h"
#include "..\Script\TestScript.h"
#include "..\Script\CameraTestMovement.h"
#include "..\Script\SunScript.h"

using namespace ze;
using namespace pubg;

ZE_IMPLEMENT_SCENE(Lobby);

void Lobby::OnLoadScene()
{
	GameObject* pGameObject;

	pGameObject = this->CreateGameObject(L"Sun");
	DirectionalLight* pLight = pGameObject->AddDeferredComponent<DirectionalLight>();
	pLight->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	pLight->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	pLight->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	pGameObject->AddDeferredComponent<SunScript>();

	Camera* pCamera;
	pGameObject = this->CreateGameObject(L"Camera");
	pGameObject->GetTransform().m_position = XMFLOAT3A(-5.0f, 100.0f, 0.0f);
	pGameObject->GetTransform().m_rotation = XMFLOAT4A(3.14f / 2.0f, -3.14f / 4.0f, 0.0f, 0.0f);
	pCamera = pGameObject->AddDeferredComponent<Camera>();
	pCamera->SetBackgroundColor(Colors::DarkOliveGreen);
	pCamera->SetDepth(0);
	pCamera->SetFieldOfView(92);
	// pGameObject->AddDeferredComponent<CameraTestMovement>();

	pGameObject = this->CreateGameObject(L"Camera2");
	pGameObject->GetTransform().m_position = XMFLOAT3A(2.5f, 105.0f, 0.0f);
	pGameObject->GetTransform().m_rotation = XMFLOAT4A(3.14f / 8.0f, -3.14f / 2.0f, 0.0f, 0.0f);
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
	

	GameObject* pKart = this->CreateGameObject(L"KartBody");
	pKart->AddDeferredComponent<TestScript>();
	pKart->GetTransform().m_position = XMFLOAT3A(0.0f, 102.5f, -5.0f);
	pKart->GetTransform().m_rotation = XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
	MeshRenderer* pMeshRenderer = pKart->AddDeferredComponent<MeshRenderer>();
	std::vector<std::shared_ptr<Mesh>> meshes = ResourceManager::GetInstance().CreateMesh(L"Resource\\Model\\newyorkmusclecar_body.obj");
	pMeshRenderer->m_mesh = meshes[0];
	

	std::shared_ptr<Material> newMat = ResourceManager::GetInstance().CreateMaterial();
	newMat->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
	newMat->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
	newMat->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 55.0f);
	newMat->m_baseMap = ResourceManager::GetInstance().CreateTexture(L"Resource\\Model\\newyorkmusclecar_tex.png");
	pMeshRenderer->m_mesh->m_subsets[0].m_material = newMat;

	auto cube_pn = this->CreateGameObject(L"CubePN");
	pMeshRenderer = cube_pn->AddDeferredComponent<MeshRenderer>();
	meshes = ResourceManager::GetInstance().CreateMesh(L"Resource\\Model\\test\\cube_pN.obj");
	pMeshRenderer->m_mesh = meshes[0];
	pMeshRenderer->m_mesh->m_subsets[0].m_material = ResourceManager::GetInstance().CreateMaterial();
	auto cubeMat = pMeshRenderer->m_mesh->m_subsets[0].m_material;
	cubeMat->m_diffuse.x = 0.5f;
	cubeMat->m_diffuse.y = 0.2f;
	cubeMat->m_diffuse.z = 0.2f;
	cubeMat->m_diffuse.w = 1.0f;
	cubeMat->m_ambient.x = cubeMat->m_diffuse.x * 0.25f;
	cubeMat->m_ambient.y = cubeMat->m_diffuse.y * 0.25f;
	cubeMat->m_ambient.z = cubeMat->m_diffuse.z * 0.25f;
	cubeMat->m_ambient.w = 1.0f;
	cubeMat->m_specular.x = 0.2f;
	cubeMat->m_specular.y = 0.2f;
	cubeMat->m_specular.z = 0.2f;
	cubeMat->m_specular.w = 16.0f;
	cube_pn->GetTransform().m_position = XMFLOAT3A(-5.0f, 97.5f, 0.0f);
	cube_pn->GetTransform().m_rotation = XMFLOAT4A(0.0f, 3.14f / 4.0f, 3.14f / 4.0f, 0.0f);
	
	std::shared_ptr<Skybox> sky = ResourceManager::GetInstance().CreateSkybox(L"Resource\\Skybox\\cloudy_puresky.dds");
	// std::shared_ptr<Skybox> sky = ResourceManager::GetInstance().CreateSkybox(L"Resource\\Skybox\\warm_restaurant_night.dds");
	// std::shared_ptr<Skybox> sky = ResourceManager::GetInstance().CreateSkybox(L"Resource\\Skybox\\snowcube.dds");
	// std::shared_ptr<Skybox> sky = ResourceManager::GetInstance().CreateSkybox(L"Resource\\Skybox\\sunsetcube.dds");
	Environment::GetInstance().SetSkybox(sky);

	// TerrainData tmd;
	// tmd.heightMapSize.row = 1025;
	// tmd.heightMapSize.column = 1025;
	// tmd.cellSpacing = 1.0f;
	// tmd.heightBase = 0.0f;
	// tmd.heightScale = 100.0f;
	// tmd.textureScale = 32.0f;
	// tmd.elementFormat = HEIGHT_MAP_FORMAT::RAW_16BIT;
	// std::shared_ptr<Terrain> terrain = ResourceManager::GetInstance().LoadTerrain(L"Resource\\Terrain\\RidgeThrough.r16", tmd);
	// std::shared_ptr<Terrain> terrain = ResourceManager::GetInstance().LoadTerrain(L"Resource\\Terrain\\RollingHills.r16", tmd);

	// TerrainData tmd;
	// tmd.heightMapSize.row = 4097;
	// tmd.heightMapSize.column = 4097;
	// tmd.cellSpacing = 1.0f;
	// tmd.heightBase = -100.0f;
	// tmd.heightScale = 200.0f;
	// tmd.textureScale = 32.0f;
	// tmd.elementFormat = HEIGHT_MAP_FORMAT::RAW_16BIT;
	// std::shared_ptr<Terrain> terrain = ResourceManager::GetInstance().LoadTerrain(L"Resource\\Terrain\\RockyPeaks.r16", tmd);

	// SetTerrain(terrain);
}
