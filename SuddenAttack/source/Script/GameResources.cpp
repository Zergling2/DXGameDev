#include "GameResources.h"
#include "..\Resource\WeaponInfo.h"
#include "..\Resource\Character.h"
#include "..\Resource\Arms.h"

using namespace ze;

constexpr float CHARACTER_COLLIDER_RADIUS = 0.35f;
constexpr float GROUND_CHECK_COLLIDER_RADIUS_SUBTRACT_FACTOR = 0.0125f;
constexpr float ANIMATION_DEAD_FRAME_TIME = 0.02f;

GameResources::GameResources(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_errTex()
	, m_errMaterial()
	, m_weaponViewInfos()
	, m_weaponInfos()
	, m_spCharacterCollider()
	, m_spGroundCheckSweepCollider()
{
}

void GameResources::Awake()
{
	m_errTex = ResourceLoader::GetInstance()->GetErrorTexture2D();
	m_spCharacterCollider = std::make_shared<CapsuleCollider>(CHARACTER_COLLIDER_RADIUS, 1.7f - 2 * CHARACTER_COLLIDER_RADIUS);
	m_spGroundCheckSweepCollider = std::make_shared<SphereCollider>(CHARACTER_COLLIDER_RADIUS - GROUND_CHECK_COLLIDER_RADIUS_SUBTRACT_FACTOR);

	bool result;
	bool grouping;
	result = AddTexture2D(L"login_bgr", ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\login_bgr.png"));
	assert(result);
	result = AddTexture2D(L"gamelist_bgr", ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\gamelist_bgr.png"));
	assert(result);

	m_errMaterial = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&m_errMaterial->m_diffuse, ColorsLinear::White);
	m_errMaterial->m_diffuseMap = m_errTex;
	XMStoreFloat4A(&m_errMaterial->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));


	// STANAG 30Rounds Magazine Material
	auto spSTANAG30rdsMagMtl = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spSTANAG30rdsMagMtl->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.05f), 1.0f));
	XMStoreFloat4A(&spSTANAG30rdsMagMtl->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 2.0f));


	// M16
	// ## SHARED_DATA
	// 1. md_m16a1_armature
	ModelData md_m16a1 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m16a1\\m16a1_pv.glb");

	std::shared_ptr<SkinnedMesh> mdl_m16a1_pv = md_m16a1.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_m16a1 = md_m16a1.m_armatures[0];
	grouping = arma_m16a1->CreateBoneGroupByRootBoneName("default", "bone_m16a1_body");
	assert(grouping);
	std::shared_ptr<StaticMesh> mdl_m16a1_tv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m16a1\\m16a1_tv.obj").m_staticMeshes[0];

	std::vector<std::shared_ptr<Material>> m16a1_mtls;
	auto spM16A1Mtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM16A1Mtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
	XMStoreFloat4A(&spM16A1Mtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 2.0f));
	spM16A1Mtl0->m_diffuseMap =	ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m16a1\\textures\\receiver_diffuse.jpg");
	spM16A1Mtl0->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m16a1\\textures\\receiver_normal.jpg");
	auto spM16A1Mtl1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM16A1Mtl1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
	XMStoreFloat4A(&spM16A1Mtl1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 4.0f));
	spM16A1Mtl1->m_diffuseMap =	ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m16a1\\textures\\furniture_diffuse.jpg");
	spM16A1Mtl1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m16a1\\textures\\furniture_normal.jpg");

	m16a1_mtls.push_back(spM16A1Mtl0);
	m16a1_mtls.push_back(spM16A1Mtl1);
	m16a1_mtls.push_back(spSTANAG30rdsMagMtl);
	auto tex2d_m16a1_previewImage = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\m16a1.png");
	
	std::unique_ptr<WeaponViewInfo> wvi_m16a1 = std::make_unique<WeaponViewInfo>(
		"m16a1_draw", "arms_draw_m16a1", arma_m16a1->GetAnimation("m16a1_draw")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"m16a1_reload", "arms_reload_m16a1", arma_m16a1->GetAnimation("m16a1_reload")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"m16a1_fire", "arms_fire_m16a1", arma_m16a1->GetAnimation("m16a1_fire")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"m16a1_run", "arms_run_m16a1", arma_m16a1->GetAnimation("m16a1_run")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"m16a1_idle", "arms_idle_m16a1", arma_m16a1->GetAnimation("m16a1_idle")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		mdl_m16a1_pv, arma_m16a1, mdl_m16a1_tv, std::move(m16a1_mtls), std::move(tex2d_m16a1_previewImage));
	this->AddWeaponViewInfo(L"m16a1", std::move(wvi_m16a1));
	std::unique_ptr<WeaponInfo> ei_m16a1 = std::make_unique<WeaponInfo>(L"M16",	30, 90, 650.0f);
	this->AddWeaponInfo(L"m16a1", std::move(ei_m16a1));

	
	// M4A1 Carbine
	ModelData md_m4a1 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m4a1\\m4a1_pv.glb");
	std::shared_ptr<SkinnedMesh> mdl_m4a1_pv = md_m4a1.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_m4a1 = arma_m16a1;		// »À´ë °øÀ¯
	std::shared_ptr<StaticMesh> mdl_m4a1_tv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m4a1\\m4a1_tv.obj").m_staticMeshes[0];

	std::vector<std::shared_ptr<Material>> m4a1_mtls;
	auto spM4A1Mtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM4A1Mtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
	XMStoreFloat4A(&spM4A1Mtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
	spM4A1Mtl0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_diffuse.png");
	spM4A1Mtl0->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_normal.png");
	auto spM4A1Mtl1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM4A1Mtl1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
	XMStoreFloat4A(&spM4A1Mtl1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 8.0f));
	spM4A1Mtl1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_diffuse.png");
	spM4A1Mtl1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_normal.png");
	auto spM4A1Mtl2 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spM4A1Mtl2->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
	XMStoreFloat4A(&spM4A1Mtl2->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
	spM4A1Mtl2->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_diffuse.png");
	spM4A1Mtl2->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_normal.png");

	m4a1_mtls.push_back(spM4A1Mtl0);
	m4a1_mtls.push_back(spM4A1Mtl1);
	m4a1_mtls.push_back(spM4A1Mtl2);
	m4a1_mtls.push_back(spSTANAG30rdsMagMtl);

	auto tex2d_m4a1_previewImage = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\m4a1.png");

	std::unique_ptr<WeaponViewInfo> wvi_m4a1 = std::make_unique<WeaponViewInfo>(
		"m16a1_draw", "arms_draw_m16a1", arma_m16a1->GetAnimation("m16a1_draw")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"m16a1_reload", "arms_reload_m16a1", arma_m16a1->GetAnimation("m16a1_reload")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"m16a1_fire", "arms_fire_m16a1", arma_m16a1->GetAnimation("m16a1_fire")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"m16a1_run", "arms_run_m16a1", arma_m16a1->GetAnimation("m16a1_run")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"m16a1_idle", "arms_idle_m16a1", arma_m16a1->GetAnimation("m16a1_idle")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		mdl_m4a1_pv, arma_m4a1, mdl_m4a1_tv, std::move(m4a1_mtls), std::move(tex2d_m4a1_previewImage));
	this->AddWeaponViewInfo(L"m4a1", std::move(wvi_m4a1));
	std::unique_ptr<WeaponInfo> ei_m4a1 = std::make_unique<WeaponInfo>(L"M4A1 Carbine", 30, 90, 720.0f);
	this->AddWeaponInfo(L"m4a1", std::move(ei_m4a1));


	// C.USP
	// ## SHARED_DATA
	// 1. md_usp_armature
	ModelData md_usp = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\usp\\usp_pv.glb");
	std::shared_ptr<SkinnedMesh> mdl_usp_pv = md_usp.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_usp = md_usp.m_armatures[0];
	grouping = arma_usp->CreateBoneGroupByRootBoneName("default", "bone_usp_body");
	assert(grouping);
	std::shared_ptr<StaticMesh> mdl_usp_tv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\usp\\usp_tv.obj").m_staticMeshes[0];

	std::vector<std::shared_ptr<Material>> usp_mtls;
	std::shared_ptr<Material> spUSPMtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spUSPMtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	XMStoreFloat4A(&spUSPMtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	spUSPMtl0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\usp\\textures\\diffuse.png");
	spUSPMtl0->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\usp\\textures\\normal.png");

	usp_mtls.push_back(spUSPMtl0);

	auto tex2d_usp_previewImage = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\usp.png");

	std::unique_ptr<WeaponViewInfo> wvi_usp = std::make_unique<WeaponViewInfo>(
		"usp_draw", "arms_draw_usp", arma_usp->GetAnimation("usp_draw")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"usp_reload", "arms_reload_usp", arma_usp->GetAnimation("usp_reload")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"usp_fire", "arms_fire_usp", arma_usp->GetAnimation("usp_fire")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"usp_run", "arms_run_usp", arma_usp->GetAnimation("usp_run")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"usp_idle", "arms_idle_usp", arma_usp->GetAnimation("usp_idle")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		mdl_usp_pv, arma_usp, mdl_usp_tv, std::move(usp_mtls), std::move(tex2d_usp_previewImage));
	this->AddWeaponViewInfo(L"usp", std::move(wvi_usp));
	std::unique_ptr<WeaponInfo> ei_usp = std::make_unique<WeaponInfo>(L"C.USP", 12, 24, 300.0f);
	this->AddWeaponInfo(L"usp", std::move(ei_usp));


	// B.92fs Black
	ModelData mdB92fsb = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\b92fsb\\b92fsb_pv.glb");
	std::shared_ptr<SkinnedMesh> mdl_b92fsb_pv = mdB92fsb.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_b92fsb = arma_usp;		// »À´ë °øÀ¯
	std::shared_ptr<StaticMesh> mdl_b92fsb_tv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\b92fsb\\b92fsb_tv.obj").m_staticMeshes[0];

	std::vector<std::shared_ptr<Material>> b92fsb_mtls;

	auto spB92fsbMtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spB92fsbMtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	XMStoreFloat4A(&spB92fsbMtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	spB92fsbMtl0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\diffuse.png");
	spB92fsbMtl0->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\normal.png");
	auto spB92fsbMtl1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spB92fsbMtl1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	XMStoreFloat4A(&spB92fsbMtl1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	spB92fsbMtl1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_body_diffuse.jpg");
	spB92fsbMtl1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_body_normal.png");
	auto spB92fsbMtl2 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spB92fsbMtl2->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	XMStoreFloat4A(&spB92fsbMtl2->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	spB92fsbMtl2->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_lamp_diffuse.jpg");
	
	b92fsb_mtls.push_back(spB92fsbMtl0);
	b92fsb_mtls.push_back(spB92fsbMtl1);
	b92fsb_mtls.push_back(spB92fsbMtl2);

	auto tex2d_b92fsb_previewImage = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\b92fsb.png");

	std::unique_ptr<WeaponViewInfo> wvi_b92fsb = std::make_unique<WeaponViewInfo>(
		"usp_draw", "arms_draw_usp", arma_usp->GetAnimation("usp_draw")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"usp_reload", "arms_reload_usp", arma_usp->GetAnimation("usp_reload")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"usp_fire", "arms_fire_usp", arma_usp->GetAnimation("usp_fire")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"usp_run", "arms_run_usp", arma_usp->GetAnimation("usp_run")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		"usp_idle", "arms_idle_usp", arma_usp->GetAnimation("usp_idle")->GetDuration() - ANIMATION_DEAD_FRAME_TIME,
		mdl_b92fsb_pv, arma_b92fsb, mdl_b92fsb_tv, std::move(b92fsb_mtls), std::move(tex2d_b92fsb_previewImage));
	this->AddWeaponViewInfo(L"b92fsb", std::move(wvi_b92fsb));
	std::unique_ptr<WeaponInfo> ei_b92fsb = std::make_unique<WeaponInfo>(L"B.92Fs Black", 15, 30, 340.0f);
	this->AddWeaponInfo(L"b92fsb", std::move(ei_b92fsb));

	// ## SHARED_DATA
	// 1. arma_steven
	ModelData md_steven = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\characters\\steven\\steven.glb");
	std::shared_ptr<SkinnedMesh> mdl_steven = md_steven.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_steven = md_steven.m_armatures[0];
	grouping = arma_steven->CreateBoneGroupByRootBoneName("upper_body", "Spine0");
	assert(grouping);
	grouping = arma_steven->CreateBoneGroupByExcludeGroup("lower_body", "upper_body");
	assert(grouping);

	std::vector<std::shared_ptr<Material>> steven_mtls;
	auto spStevenMtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spStevenMtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
	XMStoreFloat4A(&spStevenMtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
	spStevenMtl0->m_specular.w = 4.0f;
	spStevenMtl0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\body_diffuse.png");
	auto spStevenMtl1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spStevenMtl1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
	XMStoreFloat4A(&spStevenMtl1->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	spStevenMtl1->m_specular.w = 4.0f;
	spStevenMtl1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\clothes_diffuse.png");
	spStevenMtl1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\clothes_normal.png");
	auto spStevenMtl2 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spStevenMtl2->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::DarkOliveGreen, 0.5f), 1.0f));
	XMStoreFloat4A(&spStevenMtl2->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::DarkOliveGreen, 0.15f), 1.0f));
	spStevenMtl2->m_specular.w = 4.0f;

	steven_mtls.push_back(spStevenMtl0);
	steven_mtls.push_back(spStevenMtl1);
	steven_mtls.push_back(spStevenMtl2);

	std::unique_ptr<CharacterViewInfo> upStevenViewInfo = std::make_unique<CharacterViewInfo>(mdl_steven, arma_steven, std::move(steven_mtls));
	this->AddCharacterViewInfo(L"steven", std::move(upStevenViewInfo));




	// ## SHARED_DATA
	// 1. arma_stevenArms
	ModelData md_stevenArms = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\characters\\steven\\arms.glb");
	std::shared_ptr<SkinnedMesh> mdl_stevenArms = md_stevenArms.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_stevenArms = md_stevenArms.m_armatures[0];
	grouping = arma_stevenArms->CreateBoneGroupByRootBoneName("default", "Root");
	assert(grouping);

	std::vector<std::shared_ptr<Material>> stevenArms_mtls;
	auto spStevenArmsMtl = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spStevenArmsMtl->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 1.0f));
	XMStoreFloat4A(&spStevenArmsMtl->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	spStevenArmsMtl->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\arms_diffuse.png");
	assert(result);

	stevenArms_mtls.push_back(spStevenArmsMtl);

	std::unique_ptr<ArmsViewInfo> upStevenArmsViewInfo = std::make_unique<ArmsViewInfo>(mdl_stevenArms, arma_stevenArms, std::move(stevenArms_mtls));
	this->AddArmsViewInfo(L"steven", std::move(upStevenArmsViewInfo));
}

bool GameResources::AddTexture2D(std::wstring key, ze::Texture2D texture)
{
	auto ret = m_texture2ds.insert(std::make_pair(std::move(key), std::move(texture)));
	return ret.second;
}

bool GameResources::AddArmsViewInfo(std::wstring key, std::unique_ptr<ArmsViewInfo> upArmsViewInfo)
{
	auto ret = m_armsViewInfos.insert(std::make_pair(std::move(key), std::move(upArmsViewInfo)));
	return ret.second;
}

bool GameResources::AddCharacterViewInfo(std::wstring key, std::unique_ptr<CharacterViewInfo> upCharacterViewInfo)
{
	auto ret = m_characterViewInfos.insert(std::make_pair(std::move(key), std::move(upCharacterViewInfo)));
	return ret.second;
}

bool GameResources::AddWeaponViewInfo(std::wstring key, std::unique_ptr<WeaponViewInfo> upWeaponViewInfo)
{
	auto ret = m_weaponViewInfos.insert(std::make_pair(std::move(key), std::move(upWeaponViewInfo)));
	return ret.second;
}

bool GameResources::AddWeaponInfo(std::wstring key, std::unique_ptr<WeaponInfo> upWeaponInfo)
{
	auto ret = m_weaponInfos.insert(std::make_pair(std::move(key), std::move(upWeaponInfo)));
	return ret.second;
}

Texture2D GameResources::GetTexture2D(const std::wstring& key) const
{
	auto iter = m_texture2ds.find(key);

	if (iter == m_texture2ds.cend())
		return Texture2D();
	else
		return iter->second;
}

const ArmsViewInfo* GameResources::GetArmsViewinfo(const std::wstring& key) const
{
	auto iter = m_armsViewInfos.find(key);

	if (iter == m_armsViewInfos.cend())
		return nullptr;
	else
		return iter->second.get();
}

const CharacterViewInfo* GameResources::GetCharacterViewInfo(const std::wstring& key) const
{
	auto iter = m_characterViewInfos.find(key);

	if (iter == m_characterViewInfos.cend())
		return nullptr;
	else
		return iter->second.get();
}

const WeaponViewInfo* GameResources::GetWeaponViewInfo(const std::wstring& key) const
{
	auto iter = m_weaponViewInfos.find(key);

	if (iter == m_weaponViewInfos.cend())
		return nullptr;
	else
		return iter->second.get();
}

const WeaponInfo* GameResources::GetWeaponInfo(const std::wstring& key) const
{
	auto iter = m_weaponInfos.find(key);

	if (iter == m_weaponInfos.cend())
		return nullptr;
	else
		return iter->second.get();
}

float GameResources::GetCharacterColliderRadius() const
{
	return CHARACTER_COLLIDER_RADIUS;
}

float GameResources::GetGroundCheckColliderSubtractFactor() const
{
	return GROUND_CHECK_COLLIDER_RADIUS_SUBTRACT_FACTOR;
}
