#include "Lobby.h"
#include "..\Script\TestScript.h"
#include "..\Script\CameraTestMovement.h"
#include "..\Script\SunScript.h"

using namespace zergengine;
using namespace pubg;

void Lobby::LoadScene()
{
	auto sun = CreateEmpty(L"Sun");
	std::shared_ptr<DirectionalLight> lightComp = sun->AddComponent<DirectionalLight>();
	lightComp->m_ambient = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	lightComp->m_diffuse = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	lightComp->m_specular = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	sun->AddComponent<SunScript>();

	std::shared_ptr<Camera> camComp;

	auto camera = CreateEmpty(L"Camera");
	camComp = camera->AddComponent<Camera>();
	camComp->SetBackgroundColor(Colors::DarkOliveGreen);
	// camera->AddComponent<CameraTestMovement>();
	camera->GetTransform().m_position = XMFLOAT3A(-5.0f, 100.0f, 0.0f);
	camera->GetTransform().m_rotation = XMFLOAT4A(3.14f / 2.0f, -3.14f / 4.0f, 0.0f, 0.0f);

	auto camera2 = CreateEmpty(L"Camera2");
	camComp = camera2->AddComponent<Camera>();
	camComp->SetBackgroundColor(Colors::Chocolate);
	camComp->SetDepth(3);
	camComp->SetFieldOfView(70);
	camComp->SetClippingPlanes(0.01f, 500.0f);
	camComp->SetMaximumTessellationExponent(4.0f);
	camComp->SetMinimumTessellationExponent(0.0f);
	camComp->SetMaximumDistanceEndTessellation(300.0f);
	camComp->SetMinimumDistanceStartTessellation(25.0f);
	camComp->SetViewportRect(0.05f, 0.7f, 0.25f, 0.25f);
	camera2->GetTransform().m_position = XMFLOAT3A(2.5f, 105.0f, 0.0f);
	camera2->GetTransform().m_rotation = XMFLOAT4A(3.14f / 8.0f, -3.14f / 2.0f, 0.0f, 0.0f);

	auto kart = CreateEmpty(L"KartBody");
	kart->AddComponent<TestScript>();
	kart->GetTransform().m_position = XMFLOAT3A(0.0f, 102.5f, -5.0f);
	kart->GetTransform().m_rotation = XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
	std::shared_ptr<MeshRenderer> mr = kart->AddComponent<MeshRenderer>();
	std::vector<std::shared_ptr<Mesh>> meshes = Resource::LoadMesh(L"Model\\newyorkmusclecar_body.obj");
	mr->m_mesh = meshes[0];

	std::shared_ptr<Material> newMat = Resource::CreateMaterial();
	newMat->m_ambient = XMFLOAT4A(0.25f, 0.25f, 0.25f, 1.0f);
	newMat->m_diffuse = XMFLOAT4A(0.8f, 0.8f, 0.8f, 1.0f);
	newMat->m_specular = XMFLOAT4A(0.5f, 0.5f, 0.5f, 55.0f);
	newMat->m_baseMap = Resource::LoadTexture(L"Model\\newyorkmusclecar_tex.png");
	mr->m_mesh->m_subsets[0].m_material = newMat;

	auto cube_pn = CreateEmpty(L"CubePN");
	mr = cube_pn->AddComponent<MeshRenderer>();
	meshes = Resource::LoadMesh(L"Model\\test\\cube_pN.obj");
	mr->m_mesh = meshes[0];
	mr->m_mesh->m_subsets[0].m_material = Resource::CreateMaterial();
	auto cubeMat = mr->m_mesh->m_subsets[0].m_material;
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
	
	// std::shared_ptr<Skybox> sky = Resource::LoadSkybox(L"Skybox\\cloudy_puresky.dds");
	// std::shared_ptr<Skybox> sky = Resource::LoadSkybox(L"Skybox\\warm_restaurant_night.dds");
	// std::shared_ptr<Skybox> sky = Resource::LoadSkybox(L"Skybox\\snowcube.dds");
	std::shared_ptr<Skybox> sky = Resource::LoadSkybox(L"Skybox\\sunsetcube.dds");
	SetSkybox(sky);

	TerrainMapDescriptor tmd;
	tmd.heightMapSize.row = 1025;
	tmd.heightMapSize.column = 1025;
	tmd.cellSpacing = 1.0f;
	tmd.heightBase = 0.0f;
	tmd.heightScale = 100.0f;
	tmd.textureScale = 32.0f;
	tmd.elementFormat = HEIGHT_MAP_FORMAT::RAW_16BIT;
	std::shared_ptr<Terrain> terrain = Resource::LoadTerrain(L"Terrain\\RidgeThrough.r16", tmd);
	// std::shared_ptr<Terrain> terrain = Resource::LoadTerrain(L"Terrain\\RollingHills.r16", tmd);

	// TerrainMapDescriptor tmd;
	// tmd.heightMapSize.row = 4097;
	// tmd.heightMapSize.column = 4097;
	// tmd.cellSpacing = 1.0f;
	// tmd.heightBase = -100.0f;
	// tmd.heightScale = 200.0f;
	// tmd.textureScale = 32.0f;
	// tmd.elementFormat = HEIGHT_MAP_FORMAT::RAW_16BIT;
	// std::shared_ptr<Terrain> terrain = Resource::LoadTerrain(L"Terrain\\RockyPeaks.r16", tmd);

	SetTerrain(terrain);
}
