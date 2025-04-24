#include "TestScene2.h"
#include "..\Script\TestScript.h"
#include "..\Script\SunScript.h"
#include "..\Script\FirstPersonCamera.h"
#include "..\Script\MovePointLight.h"

using namespace ze;
using namespace pubg;

ZE_IMPLEMENT_SCENE(TestScene2);

void TestScene2::OnLoadScene()
{
	GameObject* pGameObject;
	MeshRenderer* pMeshRenderer;
	Camera* pCamera;
	DirectionalLight* pDirectionalLight;
	PointLight* pPointLight;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::shared_ptr<Material> material;
	Transform* pTransform;

	pGameObject = this->CreateGameObject(L"Sun");
	pDirectionalLight = pGameObject->AddDeferredComponent<DirectionalLight>();
	pDirectionalLight->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	pDirectionalLight->m_diffuse = XMFLOAT4A(0.1f, 0.1f, 0.1f, 1.0f);
	pDirectionalLight->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	pGameObject->AddDeferredComponent<SunScript>();

	pGameObject = this->CreateGameObject(L"Camera");
	pTransform = pGameObject->GetComponentRawPtr<Transform>();
	pTransform->SetPosition(XMFLOAT3A(0.0f, 0.0f, -5.0f));
	pCamera = pGameObject->AddDeferredComponent<Camera>();
	pCamera->SetBackgroundColor(Colors::DarkOliveGreen);
	pCamera->SetDepth(0);
	pCamera->SetFieldOfView(92);
	pCamera->SetClippingPlanes(0.01f, 500.0f);
	pGameObject->AddDeferredComponent<FirstPersonCamera>();		// 1인칭 카메라 조작

	pGameObject = this->CreateGameObject(L"Camera2");
	pTransform = pGameObject->GetComponentRawPtr<Transform>();
	pTransform->SetPosition(XMFLOAT3A(-5.5f, 5.0f, -2.5f));
	pTransform->SetRotation(XMFLOAT3A(0.0f, XMConvertToRadians(90.0f), 0.0f));
	pCamera = pGameObject->AddDeferredComponent<Camera>();
	pCamera->SetBackgroundColor(Colors::Chocolate);
	pCamera->SetDepth(1);
	pCamera->SetFieldOfView(70);
	pCamera->SetMaximumTessellationExponent(4.0f);
	pCamera->SetMinimumTessellationExponent(0.0f);
	pCamera->SetMaximumDistanceEndTessellation(300.0f);
	pCamera->SetMinimumDistanceStartTessellation(25.0f);
	pCamera->SetViewportRect(0.7f, 0.05f, 0.25f, 0.25f);

	pGameObject = this->CreateGameObject(L"Pillar");
	pMeshRenderer = pGameObject->AddDeferredComponent<MeshRenderer>();
	// 메시 설정
	meshes = Resource.LoadWavefrontOBJ(L"Resource\\maps\\mart\\pillar.obj");
	pMeshRenderer->m_mesh = meshes[0];
	// 재질 설정
	material = Resource.CreateMaterial();
	material->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	material->m_ambient = XMFLOAT4A(material->m_diffuse.x * 0.25f, material->m_diffuse.y * 0.25f, material->m_diffuse.z * 0.25f, 1.0f);
	material->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
	material->m_diffuseMap = Resource.LoadTexture(L"Resource\\maps\\mart\\ParkingLotInterior_Diffuse.png");
	pMeshRenderer->m_mesh->m_subsets[0].m_material = material;

	pGameObject = this->CreateGameObject(L"Point Light");
	pTransform = pGameObject->GetComponentRawPtr<Transform>();
	pTransform->SetPosition(XMFLOAT3A(0.0f, 2.5f, -1.0f));
	pGameObject->AddDeferredComponent<MovePointLight>();
	pPointLight = pGameObject->AddDeferredComponent<PointLight>();
	pPointLight->m_ambient = XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f);
	pPointLight->m_diffuse = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);
	pPointLight->m_specular = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);
	pPointLight->SetAtt(XMFLOAT3(0.0f, 5.0f, 0.0f));
	pPointLight->SetRange(10.0f);


	pGameObject = this->CreateGameObject(L"Bollard");
	pTransform = pGameObject->GetComponentRawPtr<Transform>();
	pTransform->SetPosition(XMFLOAT3A(-2.0f, 0.0f, 0.0f));
	pMeshRenderer = pGameObject->AddDeferredComponent<MeshRenderer>();
	// 메시 설정
	meshes = Resource.LoadWavefrontOBJ(L"Resource\\maps\\mart\\bollard.obj");
	pMeshRenderer->m_mesh = meshes[0];
	// 재질 설정
	pMeshRenderer->m_mesh->m_subsets[0].m_material = material;


	pGameObject = this->CreateGameObject(L"KartBody");
	pTransform = pGameObject->GetComponentRawPtr<Transform>();
	pTransform->SetPosition(XMFLOAT3A(5.0f, 0.0f, 0.0f));
	// pGameObject->AddDeferredComponent<TestScript>();
	pMeshRenderer = pGameObject->AddDeferredComponent<MeshRenderer>();
	meshes = Resource.LoadWavefrontOBJ(L"Resource\\Model\\newyorktourbus\\newyorktourbus_body.obj");
	pMeshRenderer->m_mesh = meshes[0];

	material = Resource.CreateMaterial();
	material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
	material->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
	material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 55.0f);
	material->m_diffuseMap = Resource.LoadTexture(L"Resource\\Model\\newyorktourbus\\newyorktourbus_tex.png");
	pMeshRenderer->m_mesh->m_subsets[0].m_material = material;

	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\cloudy_puresky.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\warm_restaurant_night.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\snowcube.dds");
	Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\sunsetcube.dds");
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
