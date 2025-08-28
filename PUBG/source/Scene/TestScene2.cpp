#include "TestScene2.h"
#include "..\Script\TestScript.h"
#include "..\Script\SunScript.h"
#include "..\Script\FirstPersonCamera.h"
#include "..\Script\MovePointLight.h"
#include "..\Script\SceneChange.h"

using namespace ze;
using namespace pubg;

ZE_IMPLEMENT_SCENE(TestScene2);

void TestScene2::OnLoadScene()
{
	{
		GameObjectHandle hSceneChanger = CreateGameObject(L"Scene Changer");
		GameObject* pSceneChanger = hSceneChanger.ToPtr();
		pSceneChanger->AddComponent<SceneChange>();
	}


	{
		GameObjectHandle hSun = CreateGameObject(L"Sun");
		GameObject* pSun = hSun.ToPtr();
		ComponentHandle<DirectionalLight> hDirectionalLight = pSun->AddComponent<DirectionalLight>();
		DirectionalLight* pDirectionalLight = hDirectionalLight.ToPtr();
		pDirectionalLight->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pDirectionalLight->m_diffuse = XMFLOAT4A(0.1f, 0.1f, 0.1f, 1.0f);
		pDirectionalLight->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pSun->AddComponent<SunScript>();
	}


	{
		GameObjectHandle hCamera = CreateGameObject(L"Camera");
		GameObject* pCamera = hCamera.ToPtr();
		pCamera->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, -5.0f));
		ComponentHandle<Camera> hCameraComponent = pCamera->AddComponent<Camera>();
		Camera* pCameraComponent = hCameraComponent.ToPtr();
		pCameraComponent->SetBackgroundColor(Colors::DarkOliveGreen);
		pCameraComponent->SetDepth(0);
		pCameraComponent->SetFieldOfView(92);
		pCameraComponent->SetClippingPlanes(0.01f, 500.0f);
		pCamera->AddComponent<FirstPersonCamera>();		// 1인칭 카메라 조작
	}


	{
		GameObjectHandle hCamera2 = CreateGameObject(L"Camera2");
		GameObject* pCamera2 = hCamera2.ToPtr();
		pCamera2->m_transform.SetPosition(XMFLOAT3A(-5.5f, 5.0f, -2.5f));
		pCamera2->m_transform.SetRotation(XMFLOAT3A(0.0f, XMConvertToRadians(90.0f), 0.0f));
		ComponentHandle<Camera> hCameraComponent = pCamera2->AddComponent<Camera>();
		Camera* pCameraComponent = hCameraComponent.ToPtr();
		pCameraComponent->SetBackgroundColor(Colors::Chocolate);
		pCameraComponent->SetDepth(1);
		pCameraComponent->SetFieldOfView(70);
		pCameraComponent->SetMaximumTessellationExponent(4.0f);
		pCameraComponent->SetMinimumTessellationExponent(0.0f);
		pCameraComponent->SetMaximumDistanceEndTessellation(300.0f);
		pCameraComponent->SetMinimumDistanceStartTessellation(25.0f);
		pCameraComponent->SetViewportRect(0.7f, 0.05f, 0.25f, 0.25f);
	}
	

	{
		GameObjectHandle hPointLight = CreateGameObject(L"Point Light");
		GameObject* pPointLight = hPointLight.ToPtr();
		pPointLight->m_transform.SetPosition(XMFLOAT3A(0.0f, 2.5f, -1.0f));
		pPointLight->AddComponent<MovePointLight>();
		ComponentHandle<PointLight> hPointLightComponent = pPointLight->AddComponent<PointLight>();
		PointLight* pPointLightComponent = hPointLightComponent.ToPtr();
		pPointLightComponent->m_ambient = XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f);
		pPointLightComponent->m_diffuse = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);
		pPointLightComponent->m_specular = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);
		pPointLightComponent->m_range = 3.0f;
	}


	std::shared_ptr<Material> parkinglotMaterial;
	{
		GameObjectHandle hPillar = CreateGameObject(L"Pillar");
		GameObject* pPillar = hPillar.ToPtr();
		ComponentHandle<MeshRenderer> hMeshRenderer = pPillar->AddComponent<MeshRenderer>();
		// 메시 설정
		auto meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\maps\\mart\\pillar.obj");
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		parkinglotMaterial = ResourceLoader::GetInstance()->CreateMaterial();
		parkinglotMaterial->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		parkinglotMaterial->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
		parkinglotMaterial->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
		parkinglotMaterial->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\maps\\mart\\ParkingLotInterior_Diffuse.png");
		pMeshRenderer->SetMaterial(0, parkinglotMaterial);
	}
	

	{
		GameObjectHandle hBollard = this->CreateGameObject(L"Bollard");
		GameObject* pBollard = hBollard.ToPtr();
		pBollard->m_transform.SetPosition(XMFLOAT3A(-2.0f, 0.0f, 0.0f));
		ComponentHandle<MeshRenderer> hMeshRenderer = pBollard->AddComponent<MeshRenderer>();
		// 메시 설정
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		auto meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\maps\\mart\\bollard.obj");
		pMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		pMeshRenderer->SetMaterial(0, parkinglotMaterial);
	}


	{
		GameObjectHandle hKart = CreateGameObject(L"KartBody");
		GameObject* pKart = hKart.ToPtr();
		pKart->m_transform.SetPosition(XMFLOAT3A(5.0f, 0.0f, 0.0f));
		auto hMeshRenderer = pKart->AddComponent<MeshRenderer>();
		auto meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Model\\newyorktourbus\\newyorktourbus_body.obj");
		auto pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshes[0]);
		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
		material->m_diffuse = XMFLOAT4A(0.85f, 0.85f, 0.85f, 1.0f);
		material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 55.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Model\\newyorktourbus\\newyorktourbus_tex.png");
		pMeshRenderer->SetMaterial(0, material);
	}
	

	// Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\cloudy_puresky.dds", false);
	// Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\warm_restaurant_night.dds", false);
	// Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\snowcube.dds", false);
	Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\sunsetcube.dds", false);
	Environment::GetInstance()->SetSkyboxCubeMap(skyboxCubeMap);
}
