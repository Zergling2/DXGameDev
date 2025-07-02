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
		auto hSun = CreateGameObject(L"Sun");
		auto pSun = hSun.ToPtr();
		ComponentHandle<DirectionalLight> hLight = pSun->AddComponent<DirectionalLight>();
		auto pLight = hLight.ToPtr();
		pLight->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pSun->AddComponent<SunScript>();
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
		pCamera->AddComponent<FirstPersonCamera>();		// 1��Ī ī�޶� ����
	}
	

	{
		auto hAlienPlanet = CreateGameObject(L"AlienPlanet");
		auto pAlienPlanet = hAlienPlanet.ToPtr();
		pAlienPlanet->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, 10.0f));
		auto hMeshRenderer = pAlienPlanet->AddComponent<MeshRenderer>();
		// �޽� ����
		auto pMeshRenderer = hMeshRenderer.ToPtr();
		auto meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Model\\planet\\RinglessPlanet.obj");
		pMeshRenderer->m_mesh = meshes[0];
		// ���� ����
		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		material->m_ambient = XMFLOAT4A(0.2f, 0.2f, 0.2f, 1.0f);
		material->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture(L"Resource\\Model\\planet\\RinglessPlanetA_Diffuse.png");
		pMeshRenderer->m_mesh->m_subsets[0].m_material = material;
	}

	{
		auto meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Model\\weapons\\m762.obj");

		{
			auto hWeaponMagazine = CreateGameObject(L"Magazine");
			auto* pWeaponMagazine = hWeaponMagazine.ToPtr();
			pWeaponMagazine->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, 2.0f));
			pWeaponMagazine->m_transform.SetRotation(XMFLOAT3A(0.0f, XMConvertToRadians(45.0f), 0.0f));
			auto hMeshRenderer = pWeaponMagazine->AddComponent<MeshRenderer>();
			auto pMeshRenderer = hMeshRenderer.ToPtr();
			pMeshRenderer->m_mesh = meshes[0];
			auto material = ResourceLoader::GetInstance()->CreateMaterial();
			material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
			material->m_diffuse = XMFLOAT4A(0.85f, 0.85f, 0.85f, 1.0f);
			material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 55.0f);
			pMeshRenderer->m_mesh->m_subsets[0].m_material = material;
		}

		{
			auto hWeapon = CreateGameObject(L"Weapon");
			auto* pWeapon = hWeapon.ToPtr();
			pWeapon->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, 2.0f));
			pWeapon->m_transform.SetRotation(XMFLOAT3A(0.0f, XMConvertToRadians(45.0f), 0.0f));
			auto hMeshRenderer = pWeapon->AddComponent<MeshRenderer>();
			auto pMeshRenderer = hMeshRenderer.ToPtr();
			pMeshRenderer->m_mesh = meshes[1];
			auto material = ResourceLoader::GetInstance()->CreateMaterial();
			material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
			material->m_diffuse = XMFLOAT4A(0.85f, 0.85f, 0.85f, 1.0f);
			material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 55.0f);
			pMeshRenderer->m_mesh->m_subsets[0].m_material = material;
		}
	}
	


	// Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadCubeMapTexture(L"Resource\\Skybox\\cloudy_puresky.dds");
	// Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadCubeMapTexture(L"Resource\\Skybox\\warm_restaurant_night.dds");
	Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadCubeMapTexture(L"Resource\\Skybox\\snowcube.dds");
	// Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadCubeMapTexture(L"Resource\\Skybox\\sunsetcube.dds");
	Environment::GetInstance()->SetSkyboxCubeMap(skyboxCubeMap);
}
