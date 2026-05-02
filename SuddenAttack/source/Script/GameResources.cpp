#include "GameResources.h"

using namespace ze;

constexpr float CHARACTER_COLLIDER_RADIUS = 0.35f;
constexpr float GROUND_CHECK_COLLIDER_RADIUS_SUBTRACT_FACTOR = 0.0125f;

GameResources::GameResources(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_errTex()
	, m_emptyMdlPack()
	, m_mdlPacks()
	, m_texturePacks()
	, m_materialPacks()
{
}

void GameResources::Awake()
{
	m_errTex = ResourceLoader::GetInstance()->GetErrorTexture2D();
	m_spCharacterCollider = std::make_shared<CapsuleCollider>(CHARACTER_COLLIDER_RADIUS, 1.7f - 2 * CHARACTER_COLLIDER_RADIUS);
	m_spGroundCheckSweepCollider = std::make_shared<SphereCollider>(CHARACTER_COLLIDER_RADIUS - GROUND_CHECK_COLLIDER_RADIUS_SUBTRACT_FACTOR);

	bool result;
	bool grouping;
	result = AddTexture(L"login_bgr", ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\login_bgr.png"));
	assert(result);
	result = AddTexture(L"gamelist_bgr", ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\gamelist_bgr.png"));
	assert(result);

	m_errMaterial = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&m_errMaterial->m_diffuse, ColorsLinear::White);
	m_errMaterial->m_diffuseMap = m_errTex;
	XMStoreFloat4A(&m_errMaterial->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));



	// STANAG 30Rounds Magazine Material
	auto spM16A1MagMtl = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM16A1MagMtl->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.05f), 1.0f));
	XMStoreFloat4A(&spM16A1MagMtl->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 2.0f));
	result = AddMaterial(L"m16a1_mag_mtl0", std::move(spM16A1MagMtl));
	assert(result);

	// ## SHARED_DATA
	// 1. mdM16.m_armatures[0];
	MdlPack mdlpack_m16a1;
	ModelData mdM16 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m16a1\\m16a1_pv.glb");
	grouping = mdM16.m_armatures[0]->CreateBoneGroupByRootBoneName("default", "bone_m16a1_body");
	assert(grouping);

	mdlpack_m16a1.m_skinnedMesh = mdM16.m_skinnedMeshes[0];
	mdlpack_m16a1.m_armature = mdM16.m_armatures[0];
	mdlpack_m16a1.m_staticMesh = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m16a1\\m16a1_tv.obj").m_staticMeshes[0];
	result = AddModel(L"m16a1", std::move(mdlpack_m16a1));
	assert(result);
	result = AddTexture(L"m16a1_preview", ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\m16a1.png"));
	assert(result);
	auto spM16A1Mtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM16A1Mtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
	XMStoreFloat4A(&spM16A1Mtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 2.0f));
	spM16A1Mtl0->m_diffuseMap =	ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m16a1\\textures\\receiver_diffuse.jpg");
	spM16A1Mtl0->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m16a1\\textures\\receiver_normal.jpg");
	result = AddMaterial(L"m16a1_mtl0", std::move(spM16A1Mtl0));
	assert(result);
	auto spM16A1Mtl1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM16A1Mtl1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
	XMStoreFloat4A(&spM16A1Mtl1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 4.0f));
	spM16A1Mtl1->m_diffuseMap =	ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m16a1\\textures\\furniture_diffuse.jpg");
	spM16A1Mtl1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m16a1\\textures\\furniture_normal.jpg");
	result = AddMaterial(L"m16a1_mtl1", std::move(spM16A1Mtl1));
	assert(result);

	WeaponInfo wi_m16a1{ L"M16", 30, 90, FireType::Auto, 720 };
	AddWeaponInfo(L"m16a1", std::move(wi_m16a1));



	MdlPack mdlpack_m4a1;
	ModelData mdM4A1 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m4a1\\m4a1_pv.glb");
	mdlpack_m4a1.m_skinnedMesh = mdM4A1.m_skinnedMeshes[0];
	mdlpack_m4a1.m_armature = mdM16.m_armatures[0];		// !! share m16a1 armature
	mdlpack_m4a1.m_staticMesh = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m4a1\\m4a1_tv.obj").m_staticMeshes[0];
	result = AddModel(L"m4a1", std::move(mdlpack_m4a1));
	assert(result);
	result = AddTexture(L"m4a1_preview", ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\m4a1.png"));
	assert(result);
	auto spM4A1Mtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM4A1Mtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
	XMStoreFloat4A(&spM4A1Mtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
	spM4A1Mtl0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_diffuse.png");
	spM4A1Mtl0->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_normal.png");
	result = AddMaterial(L"m4a1_mtl0", std::move(spM4A1Mtl0));
	assert(result);
	auto spM4A1Mtl1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM4A1Mtl1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
	XMStoreFloat4A(&spM4A1Mtl1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 8.0f));
	spM4A1Mtl1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_diffuse.png");
	spM4A1Mtl1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_normal.png");
	result = AddMaterial(L"m4a1_mtl1", std::move(spM4A1Mtl1));
	assert(result);
	auto spM4A1Mtl2 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM4A1Mtl2->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
	XMStoreFloat4A(&spM4A1Mtl2->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
	spM4A1Mtl2->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_diffuse.png");
	spM4A1Mtl2->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_normal.png");
	result = AddMaterial(L"m4a1_mtl2", std::move(spM4A1Mtl2));
	assert(result);

	WeaponInfo wi_m4a1{ L"M4A1", 30, 90, FireType::Auto, 800 };
	AddWeaponInfo(L"m4a1", std::move(wi_m4a1));


	// C.USP
	// ## SHARED_DATA
	// 1. mdUSP.m_armatures[0]
	MdlPack mdlpack_usp;
	ModelData mdUSP = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\usp\\usp_pv.glb");
	mdUSP.m_armatures[0]->CreateBoneGroupByRootBoneName("default", "bone_usp_body");

	mdlpack_usp.m_skinnedMesh = mdUSP.m_skinnedMeshes[0];
	mdlpack_usp.m_armature = mdUSP.m_armatures[0];
	mdlpack_usp.m_staticMesh = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\usp\\usp_tv.obj").m_staticMeshes[0];
	result = AddModel(L"usp", std::move(mdlpack_usp));
	assert(result);
	result = AddTexture(L"usp_preview", ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\usp.png"));
	assert(result);
	std::shared_ptr<Material> spUSPMtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spUSPMtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	XMStoreFloat4A(&spUSPMtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	spUSPMtl0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\usp\\textures\\diffuse.png");
	spUSPMtl0->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\usp\\textures\\normal.png");
	result = AddMaterial(L"usp_mtl0", std::move(spUSPMtl0));
	assert(result);

	WeaponInfo wi_usp{ L"C.USP", 12, 24, FireType::Semi, 300 };
	AddWeaponInfo(L"usp", std::move(wi_usp));


	// B.92fs Black
	MdlPack mdlpack_b92fsb;
	ModelData mdB92fsb = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\b92fsb\\b92fsb_pv.glb");
	mdlpack_b92fsb.m_skinnedMesh = mdB92fsb.m_skinnedMeshes[0];
	mdlpack_b92fsb.m_armature = mdUSP.m_armatures[0];		// !! share usp armature
	mdlpack_b92fsb.m_staticMesh = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\b92fsb\\b92fsb_tv.obj").m_staticMeshes[0];
	result = AddModel(L"b92fsb", std::move(mdlpack_b92fsb));
	assert(result);
	result = AddTexture(L"b92fsb_preview", ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\b92fsb.png"));
	assert(result);
	auto spB92fsbMtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spB92fsbMtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	XMStoreFloat4A(&spB92fsbMtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	spB92fsbMtl0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\diffuse.png");
	spB92fsbMtl0->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\normal.png");
	result = AddMaterial(L"b92fsb_mtl0", std::move(spB92fsbMtl0));
	assert(result);
	auto spB92fsbMtl1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spB92fsbMtl1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	XMStoreFloat4A(&spB92fsbMtl1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	spB92fsbMtl1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_body_diffuse.jpg");
	spB92fsbMtl1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_body_normal.png");
	result = AddMaterial(L"b92fsb_mtl1", std::move(spB92fsbMtl1));
	assert(result);
	auto spB92fsbMtl2 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spB92fsbMtl2->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	XMStoreFloat4A(&spB92fsbMtl2->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	spB92fsbMtl2->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_lamp_diffuse.jpg");
	result = AddMaterial(L"b92fsb_mtl2", std::move(spB92fsbMtl2));
	assert(result);

	WeaponInfo wi_b92fsb{ L"B.92Fs Black", 15, 30, FireType::Semi, 350 };
	AddWeaponInfo(L"b92fsb", std::move(wi_b92fsb));


	// ## SHARED_DATA
	// 1. mdSteven.m_armatures[0];
	MdlPack mdlpack_steven;
	ModelData mdSteven = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\characters\\steven\\steven.glb");
	grouping = mdSteven.m_armatures[0]->CreateBoneGroupByRootBoneName("upper_body", "Spine0");
	assert(grouping);
	grouping = mdSteven.m_armatures[0]->CreateBoneGroupByExcludeGroup("lower_body", "upper_body");
	assert(grouping);

	mdlpack_steven.m_skinnedMesh = mdSteven.m_skinnedMeshes[0];
	mdlpack_steven.m_armature = mdSteven.m_armatures[0];
	result = AddModel(L"steven", std::move(mdlpack_steven));
	assert(result);
	auto spStevenMtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spStevenMtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
	XMStoreFloat4A(&spStevenMtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
	spStevenMtl0->m_specular.w = 4.0f;
	spStevenMtl0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\body_diffuse.png");
	result = AddMaterial(L"steven_mtl0", std::move(spStevenMtl0));
	assert(result);
	auto spStevenMtl1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spStevenMtl1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
	XMStoreFloat4A(&spStevenMtl1->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	spStevenMtl1->m_specular.w = 4.0f;
	spStevenMtl1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\clothes_diffuse.png");
	spStevenMtl1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\clothes_normal.png");
	result = AddMaterial(L"steven_mtl1", std::move(spStevenMtl1));
	assert(result);
	auto spStevenMtl2 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spStevenMtl2->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::DarkOliveGreen, 0.5f), 1.0f));
	XMStoreFloat4A(&spStevenMtl2->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::DarkOliveGreen, 0.15f), 1.0f));
	spStevenMtl2->m_specular.w = 4.0f;
	result = AddMaterial(L"steven_mtl2", std::move(spStevenMtl2));
	assert(result);




	// ## SHARED_DATA
	// 1. mdStevenArms.m_armatures[0];
	MdlPack mdlpack_steven_arms;
	ModelData mdStevenArms = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\characters\\steven\\arms.glb");
	grouping = mdStevenArms.m_armatures[0]->CreateBoneGroupByRootBoneName("default", "Root");
	assert(grouping);

	mdlpack_steven_arms.m_skinnedMesh = mdStevenArms.m_skinnedMeshes[0];
	mdlpack_steven_arms.m_armature = mdStevenArms.m_armatures[0];
	result = AddModel(L"steven_arms", std::move(mdlpack_steven_arms));
	assert(result);
	auto spStevenArmsMtl = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spStevenArmsMtl->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 1.0f));
	XMStoreFloat4A(&spStevenArmsMtl->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	spStevenArmsMtl->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\arms_diffuse.png");
	result = AddMaterial(L"steven_arms_mtl0", std::move(spStevenArmsMtl));
	assert(result);
}

bool GameResources::AddModel(std::wstring key, MdlPack value)
{
	auto ret = m_mdlPacks.insert(std::make_pair(std::move(key), std::move(value)));
	return ret.second;
}

bool GameResources::AddTexture(std::wstring key, Texture2D value)
{
	auto ret = m_texturePacks.insert(std::make_pair(std::move(key), std::move(value)));
	return ret.second;
}

bool GameResources::AddMaterial(std::wstring key, std::shared_ptr<ze::Material> value)
{
	auto ret = m_materialPacks.insert(std::make_pair(std::move(key), std::move(value)));
	return ret.second;
}

bool GameResources::AddWeaponInfo(std::wstring key, WeaponInfo info)
{
	auto ret = m_weaponInfos.insert(std::make_pair(std::move(key), std::move(info)));
	return ret.second;
}

MdlPack GameResources::GetModel(const std::wstring& key) const
{
	auto iter = m_mdlPacks.find(key);

	if (iter == m_mdlPacks.cend())
		return m_emptyMdlPack;
	else
		return iter->second;
}

ze::Texture2D GameResources::GetTexture(const std::wstring& key) const
{
	auto iter = m_texturePacks.find(key);

	if (iter == m_texturePacks.cend())
		return m_errTex;
	else
		return iter->second;
}

std::shared_ptr<Material> GameResources::GetMaterial(const std::wstring& key) const
{
	auto iter = m_materialPacks.find(key);

	if (iter == m_materialPacks.cend())
		return m_errMaterial;
	else
		return iter->second;
}

const WeaponInfo* GameResources::GetWeaponInfo(const std::wstring& key) const
{
	auto iter = m_weaponInfos.find(key);

	if (iter == m_weaponInfos.cend())
		return nullptr;
	else
		return &iter->second;
}

float GameResources::GetCharacterColliderRadius() const
{
	return CHARACTER_COLLIDER_RADIUS;
}

float GameResources::GetGroundCheckColliderSubtractFactor() const
{
	return GROUND_CHECK_COLLIDER_RADIUS_SUBTRACT_FACTOR;
}
