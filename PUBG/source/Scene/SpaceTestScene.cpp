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
	{
		auto hSceneChanger = CreateGameObject(L"Scene Changer");
		auto pSceneChanger = hSceneChanger.ToPtr();
		pSceneChanger->AddComponent<SceneChange>();
	}
	

	{
		auto hCamera = CreateGameObject(L"Camera");
		auto pCamera = hCamera.ToPtr();
		pCamera->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, 0.0f));
		auto hCameraComponent = pCamera->AddComponent<Camera>();
		auto pCameraComponent = hCameraComponent.ToPtr();
		pCameraComponent->SetBackgroundColor(Colors::White);
		pCameraComponent->SetDepth(0);
		pCameraComponent->SetFieldOfView(92);
		pCameraComponent->SetClippingPlanes(0.01f, 500.0f);
		pCamera->AddComponent<FirstPersonCamera>();		// 1인칭 카메라 조작
	}
	

	{
		auto hAlienPlanet = CreateGameObject(L"AlienPlanet");
		auto pAlienPlanet = hAlienPlanet.ToPtr();
		pAlienPlanet->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, 10.0f));
		auto hMeshRenderer = pAlienPlanet->AddComponent<MeshRenderer>();
		// 메시 설정
		auto pMeshRenderer = hMeshRenderer.ToPtr();
		auto meshes = Resource.LoadWavefrontOBJ(L"Resource\\Model\\planet\\RinglessPlanet.obj");
		pMeshRenderer->m_mesh = meshes[0];
		// 재질 설정
		auto material = Resource.CreateMaterial();
		material->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		material->m_ambient = XMFLOAT4A(0.2f, 0.2f, 0.2f, 1.0f);
		material->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
		material->m_diffuseMap = Resource.LoadTexture(L"Resource\\Model\\planet\\RinglessPlanetA_Diffuse.png");
		pMeshRenderer->m_mesh->m_subsets[0].m_material = material;
	}
	


	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\cloudy_puresky.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\warm_restaurant_night.dds");
	Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\snowcube.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\sunsetcube.dds");
	Environment.SetSkyboxCubeMap(skyboxCubeMap);
}
