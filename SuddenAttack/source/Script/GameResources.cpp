#include "GameResources.h"
#include "..\Resource\Character.h"
#include "..\Resource\Arms.h"
#include "..\Resource\WeaponDefinition.h"

using namespace ze;

constexpr float CHARACTER_COLLIDER_RADIUS = 0.35f;
constexpr float GROUND_CHECK_COLLIDER_RADIUS_SUBTRACT_FACTOR = 0.0125f;
constexpr float ANIMATION_DEAD_FRAME_TIME = 0.01f;
constexpr XMFLOAT3 CHARACTER_BODY_COLLIDER_HALF_EXTENTS(0.17f, 0.28f, 0.095f);
constexpr float CHARACTER_NECK_COLLIDER_RADIUS = 0.06f;
constexpr float CHARACTER_NECK_COLLIDER_HEIGHT = 0.04f;
constexpr float CHARACTER_HEAD_COLLIDER_RADIUS = 0.105f;
constexpr float CHARACTER_UPPER_ARM_COLLIDER_HEIGHT = 0.22f;
constexpr float CHARACTER_UPPER_ARM_COLLIDER_RADIUS = 0.05f;
constexpr float CHARACTER_FORE_ARM_COLLIDER_HEIGHT = 0.3f;
constexpr float CHARACTER_FORE_ARM_COLLIDER_RADIUS = 0.03f;
constexpr float CHARACTER_THIGH_COLLIDER_HEIGHT = 0.4f;
constexpr float CHARACTER_THIGH_COLLIDER_RADIUS = 0.08f;
constexpr float CHARACTER_CALF_COLLIDER_HEIGHT = 0.3f;
constexpr float CHARACTER_CALF_COLLIDER_RADIUS = 0.07f;
constexpr XMFLOAT3 CHARACTER_FOOT_COLLIDER_HALF_EXTENTS(0.12f / 2.0f, 0.27f / 2.0f, 0.05f / 2.0f);

GameResources::GameResources(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_errTex()
	, m_errMaterial()
	, m_texture2ds()
	, m_armsViewInfos()
	, m_characterViewInfos()
	, m_weaponDefs()
	, m_spCharacterCollider()
	, m_spGroundCheckSweepCollider()
	, m_spCharacterBodyCollider()
	, m_spCharacterNeckCollider()
	, m_spCharacterHeadCollider()
	, m_spCharacterUpperArmCollider()
	, m_spCharacterForeArmCollider()
	, m_spCharacterThighCollider()
	, m_spCharacterCalfCollider()
	, m_spCharacterFootCollider()
{
}

void GameResources::Awake()
{
	m_errTex = ResourceLoader::GetInstance()->GetErrorTexture2D();
	m_spCharacterCollider = std::make_shared<CapsuleCollider>(CHARACTER_COLLIDER_RADIUS, 1.7f - 2 * CHARACTER_COLLIDER_RADIUS);
	m_spGroundCheckSweepCollider = std::make_shared<SphereCollider>(CHARACTER_COLLIDER_RADIUS - GROUND_CHECK_COLLIDER_RADIUS_SUBTRACT_FACTOR);


	m_spCharacterBodyCollider = std::make_shared<BoxCollider>(CHARACTER_BODY_COLLIDER_HALF_EXTENTS);
	m_spCharacterNeckCollider = std::make_shared<CylinderCollider>(CHARACTER_NECK_COLLIDER_RADIUS, CHARACTER_NECK_COLLIDER_HEIGHT);
	m_spCharacterHeadCollider = std::make_shared<SphereCollider>(CHARACTER_HEAD_COLLIDER_RADIUS);
	m_spCharacterUpperArmCollider = std::make_shared<CapsuleCollider>(CHARACTER_UPPER_ARM_COLLIDER_RADIUS, CHARACTER_UPPER_ARM_COLLIDER_HEIGHT);
	m_spCharacterForeArmCollider = std::make_shared<CapsuleCollider>(CHARACTER_FORE_ARM_COLLIDER_RADIUS, CHARACTER_FORE_ARM_COLLIDER_HEIGHT);
	m_spCharacterThighCollider = std::make_shared<CapsuleCollider>(CHARACTER_THIGH_COLLIDER_RADIUS, CHARACTER_THIGH_COLLIDER_HEIGHT);
	m_spCharacterCalfCollider = std::make_shared<CapsuleCollider>(CHARACTER_CALF_COLLIDER_RADIUS, CHARACTER_CALF_COLLIDER_HEIGHT);
	m_spCharacterFootCollider = std::make_shared<BoxCollider>(CHARACTER_FOOT_COLLIDER_HALF_EXTENTS);


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

	// ###############################
	// M16
	// ###############################
	// ## SHARED_DATA
	// 1. md_m16a1_armature
	// 2. sounds & anims (M16, M4A1, ...)
	// 3. event tables (M16, M4A1, ...)

	// 1.
	ModelData md_m16a1 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m16a1\\m16a1_pv.glb");

	std::shared_ptr<SkinnedMesh> mdl_m16a1_pv = md_m16a1.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_m16a1 = md_m16a1.m_armatures[0];
	grouping = arma_m16a1->CreateBoneGroupByRootBoneName("default", "bone_m16a1_body");
	assert(grouping);
	std::shared_ptr<StaticMesh> mdl_m16a1_tv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m16a1\\m16a1_tv.obj").m_staticMeshes[0];

	const float m16a1_draw_time = arma_m16a1->GetAnimation("m16a1_draw")->GetDuration() - ANIMATION_DEAD_FRAME_TIME;
	const float m16a1_reload_time = arma_m16a1->GetAnimation("m16a1_reload")->GetDuration() - ANIMATION_DEAD_FRAME_TIME;
	const float m16a1_recoil_time = arma_m16a1->GetAnimation("m16a1_fire")->GetDuration() - ANIMATION_DEAD_FRAME_TIME;

	// 2.
	std::unordered_map<WeaponAction, std::pair<std::string, std::string>> m16a1_animNames;	// <Weapon Anim, Arms Anim>
	m16a1_animNames[WeaponAction::Draw] = std::make_pair("m16a1_draw", "arms_draw_m16a1");
	m16a1_animNames[WeaponAction::Reload] = std::make_pair("m16a1_reload", "arms_reload_m16a1");
	m16a1_animNames[WeaponAction::Fire] = std::make_pair("m16a1_fire", "arms_fire_m16a1");
	m16a1_animNames[WeaponAction::Idle] = std::make_pair("m16a1_idle", "arms_idle_m16a1");

	std::unordered_map<WeaponEvent, std::shared_ptr<ze::AudioClip>> m16a1_sounds;
	m16a1_sounds[WeaponEvent::Bolt] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\m16a1_boltpull.wav");
	m16a1_sounds[WeaponEvent::MagOut] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\m4a1_clipout.wav");
	m16a1_sounds[WeaponEvent::MagIn] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\m4a1_clipin.wav");
	m16a1_sounds[WeaponEvent::Fire] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\m4a1_unsil-1.wav");

	// 3.
	std::unordered_map<WeaponAction, std::shared_ptr<WeaponEventTable>> m16a1_event_tables;
	{
		std::shared_ptr<WeaponEventTable> m16a1_draw_event_table = std::make_shared<WeaponEventTable>();
		m16a1_draw_event_table->AddEvent(0.45f, WeaponEvent::Bolt);
		m16a1_event_tables[WeaponAction::Draw] = std::move(m16a1_draw_event_table);

		std::shared_ptr<WeaponEventTable> m16a1_reload_event_table = std::make_shared<WeaponEventTable>();
		m16a1_reload_event_table->AddEvent(0.4f, WeaponEvent::MagOut);
		m16a1_reload_event_table->AddEvent(1.5f, WeaponEvent::MagIn);
		m16a1_reload_event_table->AddEvent(2.55f, WeaponEvent::Bolt);
		m16a1_event_tables[WeaponAction::Reload] = std::move(m16a1_reload_event_table);

		std::shared_ptr<WeaponEventTable> m16a1_fire_event_table = std::make_shared<WeaponEventTable>();
		m16a1_fire_event_table->AddEvent(0.0f, WeaponEvent::Fire);
		m16a1_event_tables[WeaponAction::Fire] = std::move(m16a1_fire_event_table);
	}
	


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

	ze::Texture2D m16a1_previewImage = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\m16a1.png");
	std::shared_ptr<WeaponDefinition> m16a1_wd = std::make_shared<WeaponDefinition>(
		WeaponCode::M16,
		L"M16",
		30,	// 탄창 용량
		90,	// 예비 탄약
		24,	// 데미지
		600.0f,	// RPM
		m16a1_draw_time,
		m16a1_reload_time,
		m16a1_recoil_time
	);

	// 무기 이벤트 테이블
	m16a1_wd->m_eventTables.insert(std::make_pair(WeaponAction::Draw, m16a1_event_tables[WeaponAction::Draw]));
	m16a1_wd->m_eventTables.insert(std::make_pair(WeaponAction::Reload, m16a1_event_tables[WeaponAction::Reload]));
	m16a1_wd->m_eventTables.insert(std::make_pair(WeaponAction::Fire, m16a1_event_tables[WeaponAction::Fire]));
	m16a1_wd->m_spPVMesh = mdl_m16a1_pv;
	m16a1_wd->m_spPVArmature = arma_m16a1;
	m16a1_wd->m_spTVMesh = mdl_m16a1_tv;
	m16a1_wd->m_materials = std::move(m16a1_mtls);
	m16a1_wd->m_previewImage = m16a1_previewImage;
	m16a1_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Draw, m16a1_animNames[WeaponAction::Draw]));
	m16a1_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Reload, m16a1_animNames[WeaponAction::Reload]));
	m16a1_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Fire, m16a1_animNames[WeaponAction::Fire]));
	m16a1_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Idle, m16a1_animNames[WeaponAction::Idle]));
	m16a1_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::Bolt, m16a1_sounds[WeaponEvent::Bolt]));
	m16a1_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::MagOut, m16a1_sounds[WeaponEvent::MagOut]));
	m16a1_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::MagIn, m16a1_sounds[WeaponEvent::MagIn]));
	m16a1_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::Fire, m16a1_sounds[WeaponEvent::Fire]));

	this->AddWeaponDefinition(m16a1_wd->GetCode(), m16a1_wd);

	// ###############################
	// M4A1 Carbine
	// ###############################
	ModelData md_m4a1 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m4a1\\m4a1_pv.glb");
	std::shared_ptr<SkinnedMesh> mdl_m4a1_pv = md_m4a1.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_m4a1 = arma_m16a1;		// 뼈대 공유
	std::shared_ptr<StaticMesh> mdl_m4a1_tv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m4a1\\m4a1_tv.obj").m_staticMeshes[0];

	std::unordered_map<WeaponEvent, std::shared_ptr<ze::AudioClip>> m4a1_sounds;
	m4a1_sounds[WeaponEvent::Bolt] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\m4a1_boltpull.wav");
	m4a1_sounds[WeaponEvent::MagOut] = m16a1_sounds[WeaponEvent::MagOut];
	m4a1_sounds[WeaponEvent::MagIn] = m16a1_sounds[WeaponEvent::MagIn];
	m4a1_sounds[WeaponEvent::Fire] = m16a1_sounds[WeaponEvent::Fire];


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

	ze::Texture2D m4a1_previewImage = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\m4a1.png");

	
	std::shared_ptr<WeaponDefinition> m4a1_wd = std::make_shared<WeaponDefinition>(
		WeaponCode::M4A1,
		L"M4A1 Carbine",
		30,	// 탄창 용량
		90,	// 예비 탄약
		24,	// 데미지
		660.0f,	// RPM
		m16a1_draw_time,
		m16a1_reload_time,
		m16a1_recoil_time
	);

	// 무기 이벤트 테이블
	m4a1_wd->m_eventTables.insert(std::make_pair(WeaponAction::Draw, m16a1_event_tables[WeaponAction::Draw]));
	m4a1_wd->m_eventTables.insert(std::make_pair(WeaponAction::Reload, m16a1_event_tables[WeaponAction::Reload]));
	m4a1_wd->m_eventTables.insert(std::make_pair(WeaponAction::Fire, m16a1_event_tables[WeaponAction::Fire]));
	m4a1_wd->m_spPVMesh = mdl_m4a1_pv;
	m4a1_wd->m_spPVArmature = arma_m4a1;
	m4a1_wd->m_spTVMesh = mdl_m4a1_tv;
	m4a1_wd->m_materials = std::move(m4a1_mtls);
	m4a1_wd->m_previewImage = m4a1_previewImage;
	m4a1_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Draw, m16a1_animNames[WeaponAction::Draw]));
	m4a1_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Reload, m16a1_animNames[WeaponAction::Reload]));
	m4a1_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Fire, m16a1_animNames[WeaponAction::Fire]));
	m4a1_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Idle, m16a1_animNames[WeaponAction::Idle]));
	m4a1_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::Bolt, m4a1_sounds[WeaponEvent::Bolt]));
	m4a1_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::MagOut, m4a1_sounds[WeaponEvent::MagOut]));
	m4a1_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::MagIn, m4a1_sounds[WeaponEvent::MagIn]));
	m4a1_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::Fire, m4a1_sounds[WeaponEvent::Fire]));

	this->AddWeaponDefinition(m4a1_wd->GetCode(), m4a1_wd);


	// ###############################
	// C.USP
	// ###############################
	// ## SHARED_DATA
	// 1. md_usp_armature
	// 2. sounds & anims (C.USP, B.92Fs Black, ...)
	// 3. event tables (C.USP, B.92Fs Black, ...)
	ModelData md_usp = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\usp\\usp_pv.glb");
	std::shared_ptr<SkinnedMesh> mdl_usp_pv = md_usp.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_usp = md_usp.m_armatures[0];
	grouping = arma_usp->CreateBoneGroupByRootBoneName("default", "bone_usp_body");
	assert(grouping);
	std::shared_ptr<StaticMesh> mdl_usp_tv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\usp\\usp_tv.obj").m_staticMeshes[0];
	

	const float usp_draw_time = arma_usp->GetAnimation("usp_draw")->GetDuration() - ANIMATION_DEAD_FRAME_TIME;
	const float usp_reload_time = arma_usp->GetAnimation("usp_reload")->GetDuration() - ANIMATION_DEAD_FRAME_TIME;
	const float usp_recoil_time = arma_usp->GetAnimation("usp_fire")->GetDuration() - ANIMATION_DEAD_FRAME_TIME;

	// 2.
	std::unordered_map<WeaponAction, std::pair<std::string, std::string>> usp_animNames;	// <Weapon Anim, Arms Anim>
	usp_animNames[WeaponAction::Draw] = std::make_pair("usp_draw", "arms_draw_usp");
	usp_animNames[WeaponAction::Reload] = std::make_pair("usp_reload", "arms_reload_usp");
	usp_animNames[WeaponAction::Fire] = std::make_pair("usp_fire", "arms_fire_usp");
	usp_animNames[WeaponAction::Idle] = std::make_pair("usp_idle", "arms_idle_usp");

	std::unordered_map<WeaponEvent, std::shared_ptr<ze::AudioClip>> usp_sounds;
	usp_sounds[WeaponEvent::SlideBack] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\usp_slideback.wav");
	usp_sounds[WeaponEvent::SlideRelease] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\usp_sliderelease.wav");
	usp_sounds[WeaponEvent::MagOut] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\usp_clipout.wav");
	usp_sounds[WeaponEvent::MagIn] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\usp_clipin.wav");
	usp_sounds[WeaponEvent::Fire] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\usp_unsil-1.wav");

	// 3.
	std::unordered_map<WeaponAction, std::shared_ptr<WeaponEventTable>> usp_event_tables;
	{
		std::shared_ptr<WeaponEventTable> usp_draw_event_table = std::make_shared<WeaponEventTable>();
		usp_draw_event_table->AddEvent(0.15f, WeaponEvent::SlideBack);
		usp_event_tables[WeaponAction::Draw] = std::move(usp_draw_event_table);

		std::shared_ptr<WeaponEventTable> usp_reload_event_table = std::make_shared<WeaponEventTable>();
		usp_reload_event_table->AddEvent(0.15f, WeaponEvent::MagOut);
		usp_reload_event_table->AddEvent(1.35f, WeaponEvent::MagIn);
		usp_reload_event_table->AddEvent(2.1f, WeaponEvent::SlideRelease);
		usp_event_tables[WeaponAction::Reload] = std::move(usp_reload_event_table);

		std::shared_ptr<WeaponEventTable> usp_fire_event_table = std::make_shared<WeaponEventTable>();
		usp_fire_event_table->AddEvent(0.0f, WeaponEvent::Fire);
		usp_event_tables[WeaponAction::Fire] = std::move(usp_fire_event_table);
	}


	std::vector<std::shared_ptr<Material>> usp_mtls;
	std::shared_ptr<Material> spUSPMtl0 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&spUSPMtl0->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	XMStoreFloat4A(&spUSPMtl0->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	spUSPMtl0->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\usp\\textures\\diffuse.png");
	spUSPMtl0->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\usp\\textures\\normal.png");

	usp_mtls.push_back(spUSPMtl0);

	ze::Texture2D usp_previewImage = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\usp.png");

	std::shared_ptr<WeaponDefinition> usp_wd = std::make_shared<WeaponDefinition>(
		WeaponCode::USP,
		L"C.USP",
		12,	// 탄창 용량
		24,	// 예비 탄약
		20,	// 데미지
		300.0f,	// RPM
		usp_draw_time,
		usp_reload_time,
		usp_recoil_time
	);

	// 무기 이벤트 테이블
	usp_wd->m_eventTables.insert(std::make_pair(WeaponAction::Draw, usp_event_tables[WeaponAction::Draw]));
	usp_wd->m_eventTables.insert(std::make_pair(WeaponAction::Reload, usp_event_tables[WeaponAction::Reload]));
	usp_wd->m_eventTables.insert(std::make_pair(WeaponAction::Fire, usp_event_tables[WeaponAction::Fire]));
	usp_wd->m_spPVMesh = mdl_usp_pv;
	usp_wd->m_spPVArmature = arma_usp;
	usp_wd->m_spTVMesh = mdl_usp_tv;
	usp_wd->m_materials = std::move(usp_mtls);
	usp_wd->m_previewImage = usp_previewImage;
	usp_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Draw, usp_animNames[WeaponAction::Draw]));
	usp_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Reload, usp_animNames[WeaponAction::Reload]));
	usp_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Fire, usp_animNames[WeaponAction::Fire]));
	usp_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Idle, usp_animNames[WeaponAction::Idle]));
	usp_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::SlideBack, usp_sounds[WeaponEvent::SlideBack]));
	usp_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::SlideRelease, usp_sounds[WeaponEvent::SlideRelease]));
	usp_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::MagOut, usp_sounds[WeaponEvent::MagOut]));
	usp_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::MagIn, usp_sounds[WeaponEvent::MagIn]));
	usp_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::Fire, usp_sounds[WeaponEvent::Fire]));

	this->AddWeaponDefinition(usp_wd->GetCode(), usp_wd);


	// B.92fs Black
	ModelData mdB92fsb = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\b92fsb\\b92fsb_pv.glb");
	std::shared_ptr<SkinnedMesh> mdl_b92fsb_pv = mdB92fsb.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_b92fsb = arma_usp;		// 뼈대 공유
	std::shared_ptr<StaticMesh> mdl_b92fsb_tv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\b92fsb\\b92fsb_tv.obj").m_staticMeshes[0];


	std::unordered_map<WeaponEvent, std::shared_ptr<ze::AudioClip>> b92fsb_sounds;
	b92fsb_sounds[WeaponEvent::SlideBack] = usp_sounds[WeaponEvent::SlideBack];
	b92fsb_sounds[WeaponEvent::SlideRelease] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\elite_sliderelease.wav");
	b92fsb_sounds[WeaponEvent::MagOut] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\elite_clipout.wav");
	b92fsb_sounds[WeaponEvent::MagIn] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\elite_rightclipin.wav");
	b92fsb_sounds[WeaponEvent::Fire] = ResourceLoader::GetInstance()->LoadWaveFile(L"resources\\sounds\\weapons\\elite_fire.wav");


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

	ze::Texture2D b92fsb_previewImage = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapons\\b92fsb.png");

	std::shared_ptr<WeaponDefinition> b92fsb_wd = std::make_shared<WeaponDefinition>(
		WeaponCode::B92FSBlack,
		L"B.92Fs Black",
		15,	// 탄창 용량
		30,	// 예비 탄약
		20,	// 데미지
		360.0f,	// RPM
		usp_draw_time,
		usp_reload_time,
		usp_recoil_time
	);



	// 무기 이벤트 테이블
	b92fsb_wd->m_eventTables.insert(std::make_pair(WeaponAction::Draw, usp_event_tables[WeaponAction::Draw]));
	b92fsb_wd->m_eventTables.insert(std::make_pair(WeaponAction::Reload, usp_event_tables[WeaponAction::Reload]));
	b92fsb_wd->m_eventTables.insert(std::make_pair(WeaponAction::Fire, usp_event_tables[WeaponAction::Fire]));
	b92fsb_wd->m_spPVMesh = mdl_b92fsb_pv;
	b92fsb_wd->m_spPVArmature = arma_b92fsb;
	b92fsb_wd->m_spTVMesh = mdl_b92fsb_tv;
	b92fsb_wd->m_materials = std::move(b92fsb_mtls);
	b92fsb_wd->m_previewImage = b92fsb_previewImage;
	b92fsb_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Draw, usp_animNames[WeaponAction::Draw]));
	b92fsb_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Reload, usp_animNames[WeaponAction::Reload]));
	b92fsb_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Fire, usp_animNames[WeaponAction::Fire]));
	b92fsb_wd->m_actionAnims.insert(std::make_pair(WeaponAction::Idle, usp_animNames[WeaponAction::Idle]));
	b92fsb_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::SlideBack, b92fsb_sounds[WeaponEvent::SlideBack]));
	b92fsb_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::SlideRelease, b92fsb_sounds[WeaponEvent::SlideRelease]));
	b92fsb_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::MagOut, b92fsb_sounds[WeaponEvent::MagOut]));
	b92fsb_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::MagIn, b92fsb_sounds[WeaponEvent::MagIn]));
	b92fsb_wd->m_weaponSounds.insert(std::make_pair(WeaponEvent::Fire, b92fsb_sounds[WeaponEvent::Fire]));

	this->AddWeaponDefinition(b92fsb_wd->GetCode(), b92fsb_wd);

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

bool GameResources::AddWeaponDefinition(WeaponCode key, std::shared_ptr<WeaponDefinition> spWeaponInfo)
{
	auto ret = m_weaponDefs.insert(std::make_pair(key, std::move(spWeaponInfo)));
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

std::shared_ptr<WeaponDefinition> GameResources::GetWeaponDefinition(WeaponCode key) const
{
	auto iter = m_weaponDefs.find(key);

	if (iter == m_weaponDefs.cend())
		return nullptr;
	else
		return iter->second;
}

const XMFLOAT3& GameResources::GetCharacterBodyColliderHalfExtents() const
{
	return CHARACTER_BODY_COLLIDER_HALF_EXTENTS;
}

float GameResources::GetCharacterNeckColliderHeight() const
{
	return CHARACTER_NECK_COLLIDER_HEIGHT;
}

float GameResources::GetCharacterHeadColliderRadius() const
{
	return CHARACTER_HEAD_COLLIDER_RADIUS;
}

float GameResources::GetCharacterUpperArmColliderRadius() const
{
	return CHARACTER_UPPER_ARM_COLLIDER_RADIUS;
}

float GameResources::GetCharacterUpperArmColliderHeight() const
{
	return CHARACTER_UPPER_ARM_COLLIDER_HEIGHT;
}

float GameResources::GetCharacterForeArmColliderRadius() const
{
	return CHARACTER_FORE_ARM_COLLIDER_RADIUS;
}

float GameResources::GetCharacterForeArmColliderHeight() const
{
	return CHARACTER_FORE_ARM_COLLIDER_HEIGHT;
}

float GameResources::GetCharacterThighColliderRadius() const
{
	return CHARACTER_THIGH_COLLIDER_RADIUS;
}

float GameResources::GetCharacterThighColliderHeight() const
{
	return CHARACTER_THIGH_COLLIDER_HEIGHT;
}

float GameResources::GetCharacterCalfColliderRadius() const
{
	return CHARACTER_CALF_COLLIDER_RADIUS;
}

float GameResources::GetCharacterCalfColliderHeight() const
{
	return CHARACTER_CALF_COLLIDER_HEIGHT;
}

const XMFLOAT3& GameResources::GetCharacterFootColliderHalfExtents() const
{
	return CHARACTER_FOOT_COLLIDER_HALF_EXTENTS;
}

float GameResources::GetCharacterColliderRadius() const
{
	return CHARACTER_COLLIDER_RADIUS;
}

float GameResources::GetGroundCheckColliderSubtractFactor() const
{
	return GROUND_CHECK_COLLIDER_RADIUS_SUBTRACT_FACTOR;
}
