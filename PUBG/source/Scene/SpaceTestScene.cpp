#include "SpaceTestScene.h"
#include "..\Script\TestScript.h"
#include "..\Script\SunScript.h"
#include "..\Script\FirstPersonCamera.h"
#include "..\Script\SceneChange.h"

using namespace ze;
using namespace pubg;

ZE_IMPLEMENT_SCENE(SpaceTestScene);

void SpaceTestScene::OnLoadScene()
{
	GameObject* pGameObject = nullptr;
	Transform* pTransform = nullptr;
	Camera* pCamera = nullptr;
	MeshRenderer* pMeshRenderer = nullptr;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::shared_ptr<Material> material;

	pGameObject = this->CreateGameObject(L"Scene Changer");
	pGameObject->AddDeferredComponent<SceneChange>();

	pGameObject = this->CreateGameObject(L"Camera");
	pTransform = pGameObject->GetComponentRawPtr<Transform>();
	pTransform->SetPosition(XMFLOAT3A(0.0f, 0.0f, 0.0f));
	pCamera = pGameObject->AddDeferredComponent<Camera>();
	pCamera->SetBackgroundColor(Colors::White);
	pCamera->SetDepth(0);
	pCamera->SetFieldOfView(92);
	pCamera->SetClippingPlanes(0.01f, 500.0f);
	pGameObject->AddDeferredComponent<FirstPersonCamera>();		// 1인칭 카메라 조작

	pGameObject = this->CreateGameObject(L"AlienPlanet");
	pMeshRenderer = pGameObject->AddDeferredComponent<MeshRenderer>();
	pTransform = pGameObject->GetComponentRawPtr<Transform>();
	pTransform->SetPosition(XMFLOAT3A(0.0f, 0.0f, 0.0f));
	// 메시 설정
	meshes = Resource.LoadWavefrontOBJ(L"Resource\\Model\\planet\\RinglessPlanet.obj");
	pMeshRenderer->m_mesh = meshes[0];
	// 재질 설정
	material = Resource.CreateMaterial();
	material->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	material->m_ambient = XMFLOAT4A(material->m_diffuse.x * 0.2f, material->m_diffuse.y * 0.2f, material->m_diffuse.z * 0.2f, 1.0f);
	material->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
	material->m_diffuseMap = Resource.LoadTexture(L"Resource\\Model\\planet\\RinglessPlanetA_Diffuse.png");
	pMeshRenderer->m_mesh->m_subsets[0].m_material = material;


	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\cloudy_puresky.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\warm_restaurant_night.dds");
	Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\snowcube.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\sunsetcube.dds");
	Environment.SetSkyboxCubeMap(skyboxCubeMap);
}
