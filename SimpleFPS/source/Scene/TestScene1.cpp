#include "TestScene1.h"
#include "..\Script\FirstPersonMovement.h"
#include "..\Script\Planet.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(TestScene1)

void TestScene1::OnLoadScene()
{
	std::vector<std::shared_ptr<Mesh>> meshes;

	UIObjectHandle hText1 = CreateText();
	Text* pText = static_cast<Text*>(hText1.ToPtr());
	pText->SetText(L"+");
	pText->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
	pText->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::VCENTER);
	pText->SetColor(Colors::Red);
	pText->GetTextFormat().SetSize(20);
	pText->Refresh();

	UIObjectHandle hText2 = CreateText();
	Text* pText2 = static_cast<Text*>(hText2.ToPtr());
	pText2->SetText(L"30초 후 경기 구역이 제한됩니다!");
	pText2->m_transform.SetHorizontalAnchor(HORIZONTAL_ANCHOR::CENTER);
	pText2->m_transform.SetVerticalAnchor(VERTICAL_ANCHOR::BOTTOM);
	pText2->m_transform.m_position.y = +200.0f;
	pText2->SetSize(XMFLOAT2(400, 50));
	pText2->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pText2->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pText2->SetColor(Colors::GreenYellow);
	pText2->GetTextFormat().SetSize(20);
	pText2->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_DEMI_BOLD);
	pText2->Refresh();

	{
		GameObjectHandle hSun = CreateGameObject(L"Sun");
		GameObject* pSun = hSun.ToPtr();
		pSun->m_transform.SetRotation(XMFLOAT3(XMConvertToRadians(45), XMConvertToRadians(30), 0));
		ComponentHandle<DirectionalLight> hLight = pSun->AddComponent<DirectionalLight>();
		DirectionalLight* pLight = hLight.ToPtr();
		pLight->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		pLight->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	}

	GameObjectHandle hMainCamera = CreateGameObject(L"Camera");
	GameObject* pMainCamera = hMainCamera.ToPtr();
	{
		pMainCamera->m_transform.SetPosition(XMFLOAT3A(0.0f, 1.5f, -2.0f));
		ComponentHandle<Camera> hMainCameraComponent = pMainCamera->AddComponent<Camera>();
		Camera* pMainCameraComponent = hMainCameraComponent.ToPtr();
		pMainCameraComponent->SetBackgroundColor(Colors::Gray);
		pMainCameraComponent->SetDepth(0);
		pMainCameraComponent->SetFieldOfView(92);
		pMainCameraComponent->SetClippingPlanes(0.1f, 500.0f);
		pMainCamera->AddComponent<FirstPersonMovement>();		// 1인칭 카메라 조작
	}

	{
		GameObjectHandle hRifle = CreateGameObject(L"Rifle");
		GameObject* pRifle = hRifle.ToPtr();
		pRifle->m_transform.SetPosition(XMFLOAT3A(0.0f, -0.2f, 0.15f));
		ComponentHandle<MeshRenderer> hRifleMeshRenderer = pRifle->AddComponent<MeshRenderer>();
		MeshRenderer* pRifleMeshRenderer = hRifleMeshRenderer.ToPtr();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Model\\weapons\\M16A1\\m16a1.obj");
		pRifleMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		auto material0 = ResourceLoader::GetInstance()->CreateMaterial();
		material0->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		material0->m_ambient = XMFLOAT4A(material0->m_diffuse.x * 0.2f, material0->m_diffuse.y * 0.2f, material0->m_diffuse.z * 0.2f, 1.0f);
		material0->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
		material0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Model\\weapons\\M16A1\\textures\\m16a1_0_Diffuse.jpg");

		auto material1 = ResourceLoader::GetInstance()->CreateMaterial();
		material1->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		material1->m_ambient = XMFLOAT4A(material1->m_diffuse.x * 0.2f, material1->m_diffuse.y * 0.2f, material1->m_diffuse.z * 0.2f, 1.0f);
		material1->m_specular = XMFLOAT4A(0.2f, 0.2f, 0.2f, 16.0f);
		material1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Model\\weapons\\M16A1\\textures\\m16a1_1_Diffuse.jpg");
		pRifleMeshRenderer->SetMaterial(0, material0);
		pRifleMeshRenderer->SetMaterial(1, material1);

		pRifle->m_transform.SetParent(&pMainCamera->m_transform);
	}


	{
		GameObjectHandle hCamera2 = CreateGameObject(L"Camera2");
		GameObject* pCamera2 = hCamera2.ToPtr();
		pCamera2->m_transform.SetPosition(XMFLOAT3A(-5.5f, 2.5f, -2.5f));
		pCamera2->m_transform.SetRotation(XMFLOAT3A(0.0f, XMConvertToRadians(90), 0.0f));
		ComponentHandle<Camera> hCamera2CamComponent = pCamera2->AddComponent<Camera>();
		Camera* pCamera2CamComponent = hCamera2CamComponent.ToPtr();
		pCamera2CamComponent->SetBackgroundColor(Colors::Green);
		pCamera2CamComponent->SetDepth(1);
		pCamera2CamComponent->SetFieldOfView(70);
		pCamera2CamComponent->SetMaximumTessellationExponent(4.0f);
		pCamera2CamComponent->SetMinimumTessellationExponent(0.0f);
		pCamera2CamComponent->SetMaximumDistanceEndTessellation(300.0f);
		pCamera2CamComponent->SetMinimumDistanceStartTessellation(25.0f);
		pCamera2CamComponent->SetViewportRect(0.7f, 0.7f, 0.25f, 0.25f);
	}


	
	{
		GameObjectHandle hWall;
		GameObject* pWall;
		hWall = CreateGameObject(L"Wall");
		pWall = hWall.ToPtr();
		pWall->m_transform.SetPosition(XMFLOAT3A(0.0f, 0.0f, 0.0f));
		ComponentHandle< MeshRenderer> hKartMeshRenderer = pWall->AddComponent<MeshRenderer>();
		MeshRenderer* pKartMeshRenderer = hKartMeshRenderer.ToPtr();

		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Maps\\mart\\wall1.obj");
		pKartMeshRenderer->SetMesh(meshes[0]);

		auto material = ResourceLoader::GetInstance()->CreateMaterial();
		material->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
		material->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
		material->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 4.0f);
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Maps\\mart\\Wall1_Diffuse.png");
		pKartMeshRenderer->SetMaterial(0, material);
	}

	{
		GameObjectHandle hRange = CreateGameObject(L"Range");
		GameObject* pRange = hRange.ToPtr();
		ComponentHandle<MeshRenderer> hRangeMeshRenderer = pRange->AddComponent<MeshRenderer>();
		// 메시 설정
		meshes = ResourceLoader::GetInstance()->LoadWavefrontOBJ(L"Resource\\Maps\\stage1\\range.obj");
		MeshRenderer* pBollardMeshRenderer = hRangeMeshRenderer.ToPtr();
		pBollardMeshRenderer->SetMesh(meshes[0]);
		// 재질 설정
		auto material = ResourceLoader::GetInstance()->CreateMaterial();

		XMStoreFloat4A(&material->m_diffuse, XMVectorReplicate(0.8f));	// 모래 느낌
		material->m_diffuse.w = 1.0f;
		XMStoreFloat4A(&material->m_specular, XMVectorZero());	// 모래 느낌
		material->m_specular.w = 1.0f;
		material->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Maps\\stage1\\Limestone.jpg");
		pBollardMeshRenderer->SetMaterial(0, material);
	}

	Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\snowcube.dds", false);
	Environment::GetInstance()->SetSkyboxCubeMap(skyboxCubeMap);
}
