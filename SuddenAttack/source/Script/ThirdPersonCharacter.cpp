#include "ThirdPersonCharacter.h"
#include "../Resource/GlobalGameObjects.h"
#include "../Resource/Character.h"
#include "GameResources.h"

using namespace ze;

const XMFLOAT3 ThirdPersonCharacter::PRIMARY_WEAPON_TV_OFFSET(-0.004f, +0.06f, +0.03f);
const XMFLOAT3 ThirdPersonCharacter::SECONDARY_WEAPON_TV_OFFSET(-0.014f, +0.07f, +0.03f);

ThirdPersonCharacter::ThirdPersonCharacter(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_primaryWeaponLocalRot(0.0f, 0.0f, 0.0f, 0.0f)
	, m_secondaryWeaponLocalRot(0.0f, 0.0f, 0.0f, 0.0f)
	, m_hSkinnedMeshRendererCharacter()
	, m_hGameObjectTVWeaponBase()
	, m_hGameObjectTVWeapons()
	, m_hGameObjectHitboxBody()			// Spine0
	, m_hGameObjectHitboxNeck()			// Neck
	, m_hGameObjectHitboxHead()			// Head
	, m_hGameObjectHitboxLeftUpperArm()	// UpperArm.L
	, m_hGameObjectHitboxRightUpperArm()// UpperArm.R
	, m_hGameObjectHitboxLeftForeArm()	// ForeArm.L
	, m_hGameObjectHitboxRightForeArm()	// ForeArm.R
	, m_hGameObjectHitboxLeftThigh()	// Thigh.L
	, m_hGameObjectHitboxRightThigh()	// Thigh.R
	, m_hGameObjectHitboxLeftCalf()		// Calf.L
	, m_hGameObjectHitboxRightCalf()	// Calf.R
	, m_hGameObjectHitboxLeftFoot()		// Foot.L
	, m_hGameObjectHitboxRightFoot()	// Foot.R
	, m_biSpine0(0)
	, m_biNeck(0)
	, m_biHead(0)
	, m_biLeftUpperArm(0)
	, m_biRightUpperArm(0)
	, m_biLeftForeArm(0)
	, m_biRightForeArm(0)
	, m_biLeftThigh(0)
	, m_biRightThigh(0)
	, m_biLeftCalf(0)
	, m_biRightCalf(0)
	// , m_biLeftFoot(0)
	// , m_biRightFoot(0)
	, m_biLeftToe(0)
	, m_biRightToe(0)
	, m_biLeftHand(0)
	, m_biRightHand(0)
{
}

void ThirdPersonCharacter::Awake()
{
	// GameResources 오브젝트 검색 및 스크립트 저장
	GameObjectHandle hGameObjectGameResources = GameObject::Find(GO_GAME_RESOURCES_NAME);
	assert(hGameObjectGameResources.IsValid());

	ComponentHandle<GameResources> hScriptGameResources = hGameObjectGameResources.ToPtr()->GetComponent<GameResources>();
	assert(hScriptGameResources.IsValid());
	m_hScriptGameResources = hScriptGameResources;
	GameResources* pScriptGameResources = hScriptGameResources.ToPtr();

	std::shared_ptr<CapsuleCollider> spCharacterCollider = pScriptGameResources->GetCharacterCollider();
	// ...

	// ###################################################################
	// TV 무기 베이스 오프셋 계산 및 저장
	XMVECTOR primaryWeaponLocalRot = XMQuaternionRotationNormal(Vector3::Up(), XMConvertToRadians(+90));
	primaryWeaponLocalRot = XMQuaternionMultiply(primaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Forward(), XMConvertToRadians(+81)));
	primaryWeaponLocalRot = XMQuaternionMultiply(primaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Right(), XMConvertToRadians(+8)));
	XMStoreFloat4(&m_primaryWeaponLocalRot, primaryWeaponLocalRot);

	XMVECTOR secondaryWeaponLocalRot = XMQuaternionRotationNormal(Vector3::Up(), XMConvertToRadians(+90));
	secondaryWeaponLocalRot = XMQuaternionMultiply(secondaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Forward(), XMConvertToRadians(+81)));
	secondaryWeaponLocalRot = XMQuaternionMultiply(secondaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Right(), XMConvertToRadians(-10)));
	XMStoreFloat4(&m_secondaryWeaponLocalRot, secondaryWeaponLocalRot);
	// ###################################################################

	// TV 무기 베이스 오브젝트 생성
	GameObjectHandle hGameObjectTVWeaponBase = Runtime::GetInstance()->CreateGameObject(L"tv_weapon_base");
	m_hGameObjectTVWeaponBase = hGameObjectTVWeaponBase;		// 오브젝트 핸들 저장
	GameObject* pGameObjectTVWeaponBase = hGameObjectTVWeaponBase.ToPtr();
	pGameObjectTVWeaponBase->m_transform.SetParent(&this->m_pGameObject->m_transform);

	// 캐릭터 SkinnedMesh 오브젝트 생성 및 초기화
	ComponentHandle<SkinnedMeshRenderer> hSkinnedMeshRendererCharacter = this->m_pGameObject->AddComponent<SkinnedMeshRenderer>();
	m_hSkinnedMeshRendererCharacter = hSkinnedMeshRendererCharacter;	// 컴포넌트 핸들을 멤버로 저장
	SkinnedMeshRenderer* pSkinnedMeshRendererCharacter = hSkinnedMeshRendererCharacter.ToPtr();
	const CharacterViewInfo* pCharacterViewInfo = pScriptGameResources->GetCharacterViewInfo(L"steven");
	this->CreateCharacterView(pCharacterViewInfo);


	// 캐릭터 히트박스 생성
	m_hGameObjectHitboxBody = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxBody = m_hGameObjectHitboxBody.ToPtr();
	pGameObjectHitboxBody->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxBody =
		pGameObjectHitboxBody->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterBodyCollider(),
			XMFLOAT3(
				0.0f,
				pScriptGameResources->GetCharacterBodyColliderHalfExtents().y - 1.0f,
				0.03f
			)
		);
	Rigidbody* pRigidbodyHitboxBody = hRigidbodyHitboxBody.ToPtr();
	pRigidbodyHitboxBody->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxBody->SetTrigger(true);
	
	m_hGameObjectHitboxNeck = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxNeck = m_hGameObjectHitboxNeck.ToPtr();
	pGameObjectHitboxNeck->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxNeck =
		pGameObjectHitboxNeck->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterNeckCollider(),
			XMFLOAT3(
				0.0f,
				pScriptGameResources->GetCharacterNeckColliderHeight(),
				0.0f
			)
		);
	Rigidbody* pRigidbodyHitboxNeck = hRigidbodyHitboxNeck.ToPtr();
	pRigidbodyHitboxNeck->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxNeck->SetTrigger(true);
	
	m_hGameObjectHitboxHead = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxHead = m_hGameObjectHitboxHead.ToPtr();
	pGameObjectHitboxHead->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxHead =
		pGameObjectHitboxHead->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterHeadCollider(),
			XMFLOAT3(
				0.0f,
				pScriptGameResources->GetCharacterHeadColliderRadius(),
				0.02f)
		);
	Rigidbody* pRigidbodyHitboxHead = hRigidbodyHitboxHead.ToPtr();
	pRigidbodyHitboxHead->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxHead->SetTrigger(true);
	
	m_hGameObjectHitboxLeftUpperArm = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxLeftUpperArm = m_hGameObjectHitboxLeftUpperArm.ToPtr();
	pGameObjectHitboxLeftUpperArm->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxLeftUpperArm =
		pGameObjectHitboxLeftUpperArm->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterUpperArmCollider(),
			XMFLOAT3(
				0.0f,
				(pScriptGameResources->GetCharacterUpperArmColliderRadius() + pScriptGameResources->GetCharacterUpperArmColliderHeight()) / 2.0f,
				0.0f
			)
		);
	Rigidbody* pRigidbodyHitboxLeftUpperArm = hRigidbodyHitboxLeftUpperArm.ToPtr();
	pRigidbodyHitboxLeftUpperArm->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxLeftUpperArm->SetTrigger(true);
	
	m_hGameObjectHitboxRightUpperArm = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxRightUpperArm = m_hGameObjectHitboxRightUpperArm.ToPtr();
	pGameObjectHitboxRightUpperArm->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxRightUpperArm =
		pGameObjectHitboxRightUpperArm->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterUpperArmCollider(),
			XMFLOAT3(
				0.0f,
				(pScriptGameResources->GetCharacterUpperArmColliderRadius() + pScriptGameResources->GetCharacterUpperArmColliderHeight()) / 2.0f,
				0.0f
			)
		);
	Rigidbody* pRigidbodyHitboxRightUpperArm = hRigidbodyHitboxRightUpperArm.ToPtr();
	pRigidbodyHitboxRightUpperArm->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxRightUpperArm->SetTrigger(true);
	
	m_hGameObjectHitboxLeftForeArm = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxLeftForeArm = m_hGameObjectHitboxLeftForeArm.ToPtr();
	pGameObjectHitboxLeftForeArm->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxLeftForeArm =
		pGameObjectHitboxLeftForeArm->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterForeArmCollider(),
			XMFLOAT3(
				0.0f,
				(pScriptGameResources->GetCharacterForeArmColliderRadius() + pScriptGameResources->GetCharacterForeArmColliderHeight()) / 2.0f,
				0.0f
			)
		);
	Rigidbody* pRigidbodyHitboxLeftForeArm = hRigidbodyHitboxLeftForeArm.ToPtr();
	pRigidbodyHitboxLeftForeArm->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxLeftForeArm->SetTrigger(true);
	
	m_hGameObjectHitboxRightForeArm = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxRightForeArm = m_hGameObjectHitboxRightForeArm.ToPtr();
	pGameObjectHitboxRightForeArm->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxRightForeArm =
		pGameObjectHitboxRightForeArm->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterForeArmCollider(),
			XMFLOAT3(
				0.0f,
				(pScriptGameResources->GetCharacterForeArmColliderRadius() + pScriptGameResources->GetCharacterForeArmColliderHeight()) / 2.0f,
				0.0f
			)
		);
	Rigidbody* pRigidbodyHitboxRightForeArm = hRigidbodyHitboxRightForeArm.ToPtr();
	pRigidbodyHitboxRightForeArm->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxRightForeArm->SetTrigger(true);

	m_hGameObjectHitboxLeftThigh = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxLeftThigh = m_hGameObjectHitboxLeftThigh.ToPtr();
	pGameObjectHitboxLeftThigh->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxLeftThigh =
		pGameObjectHitboxLeftThigh->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterThighCollider(),
			XMFLOAT3(
				0.0f,
				(pScriptGameResources->GetCharacterThighColliderRadius() + pScriptGameResources->GetCharacterThighColliderHeight()) / 2.0f - 0.075f,
				0.0f
			)
		);
	Rigidbody* pRigidbodyHitboxLeftThigh = hRigidbodyHitboxLeftThigh.ToPtr();
	pRigidbodyHitboxLeftThigh->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxLeftThigh->SetTrigger(true);

	m_hGameObjectHitboxRightThigh = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxRightThigh = m_hGameObjectHitboxRightThigh.ToPtr();
	pGameObjectHitboxRightThigh->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxRightThigh =
		pGameObjectHitboxRightThigh->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterThighCollider(),
			XMFLOAT3(
				0.0f,
				(pScriptGameResources->GetCharacterThighColliderRadius() + pScriptGameResources->GetCharacterThighColliderHeight()) / 2.0f - 0.075f,
				0.0f
			)
		);
	Rigidbody* pRigidbodyHitboxRightThigh = hRigidbodyHitboxRightThigh.ToPtr();
	pRigidbodyHitboxRightThigh->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxRightThigh->SetTrigger(true);

	m_hGameObjectHitboxLeftCalf = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxLeftCalf = m_hGameObjectHitboxLeftCalf.ToPtr();
	pGameObjectHitboxLeftCalf->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxLeftCalf =
		pGameObjectHitboxLeftCalf->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterCalfCollider(),
			XMFLOAT3(
				0.0f,
				(pScriptGameResources->GetCharacterCalfColliderRadius() + pScriptGameResources->GetCharacterCalfColliderHeight()) / 2.0f,
				0.0f
			)
		);
	Rigidbody* pRigidbodyHitboxLeftCalf = hRigidbodyHitboxLeftCalf.ToPtr();
	pRigidbodyHitboxLeftCalf->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxLeftCalf->SetTrigger(true);

	m_hGameObjectHitboxRightCalf = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxRightCalf = m_hGameObjectHitboxRightCalf.ToPtr();
	pGameObjectHitboxRightCalf->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxRightCalf =
		pGameObjectHitboxRightCalf->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterCalfCollider(),
			XMFLOAT3(
				0.0f,
				(pScriptGameResources->GetCharacterCalfColliderRadius() + pScriptGameResources->GetCharacterCalfColliderHeight()) / 2.0f,
				0.0f
			)
		);
	Rigidbody* pRigidbodyHitboxRightCalf = hRigidbodyHitboxRightCalf.ToPtr();
	pRigidbodyHitboxRightCalf->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxRightCalf->SetTrigger(true);

	m_hGameObjectHitboxLeftFoot = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxLeftFoot = m_hGameObjectHitboxLeftFoot.ToPtr();
	pGameObjectHitboxLeftFoot->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxLeftFoot =
		pGameObjectHitboxLeftFoot->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterFootCollider(),
			XMFLOAT3(
				0.0f,
				-0.075f,
				-0.01f
			)
		);
	Rigidbody* pRigidbodyHitboxLeftFoot = hRigidbodyHitboxLeftFoot.ToPtr();
	pRigidbodyHitboxLeftFoot->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxLeftFoot->SetTrigger(true);

	m_hGameObjectHitboxRightFoot = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectHitboxRightFoot = m_hGameObjectHitboxRightFoot.ToPtr();
	pGameObjectHitboxRightFoot->m_transform.SetParent(&m_pGameObject->m_transform);
	ComponentHandle<Rigidbody> hRigidbodyHitboxRightFoot =
		pGameObjectHitboxRightFoot->AddComponent<Rigidbody>(
			pScriptGameResources->GetCharacterFootCollider(),
			XMFLOAT3(
				0.0f,
				-0.075f,
				-0.01f
			)
		);
	Rigidbody* pRigidbodyHitboxRightFoot = hRigidbodyHitboxRightFoot.ToPtr();
	pRigidbodyHitboxRightFoot->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxRightFoot->SetTrigger(true);

	// 상체 애니메이션
	// fire_rifle
	// fire_pistol
	// idle_rifle
	// idle_pistol
	// reload_rifle
	// reload_pistol
	// 
	// 하체 애니메이션
	// run
	// stand_idle

	pSkinnedMeshRendererCharacter->PlayGroupAnimation("idle_pistol", "upper_body", true);
	pSkinnedMeshRendererCharacter->PlayGroupAnimation("run", "lower_body", true);

	const Armature* pCharacterArmature = pSkinnedMeshRendererCharacter->GetArmaturePtr();
	m_biSpine0 = pCharacterArmature->GetBoneIndex("Spine0");
	m_biNeck = pCharacterArmature->GetBoneIndex("Neck");
	m_biHead = pCharacterArmature->GetBoneIndex("Head");
	m_biLeftUpperArm = pCharacterArmature->GetBoneIndex("UpperArm.L");
	m_biRightUpperArm = pCharacterArmature->GetBoneIndex("UpperArm.R");
	m_biLeftForeArm = pCharacterArmature->GetBoneIndex("ForeArm.L");
	m_biRightForeArm = pCharacterArmature->GetBoneIndex("ForeArm.R");
	m_biLeftThigh = pCharacterArmature->GetBoneIndex("Thigh.L");
	m_biRightThigh = pCharacterArmature->GetBoneIndex("Thigh.R");
	m_biLeftCalf = pCharacterArmature->GetBoneIndex("Calf.L");
	m_biRightCalf = pCharacterArmature->GetBoneIndex("Calf.R");
	// m_biLeftFoot = pCharacterArmature->GetBoneIndex("Foot.L");
	// m_biRightFoot = pCharacterArmature->GetBoneIndex("Foot.R");
	m_biLeftToe = pCharacterArmature->GetBoneIndex("Toe.L");
	m_biRightToe = pCharacterArmature->GetBoneIndex("Toe.R");
	m_biLeftHand = pCharacterArmature->GetBoneIndex("Hand.L");
	m_biRightHand = pCharacterArmature->GetBoneIndex("Hand.R");
}

void ThirdPersonCharacter::Update()
{
	this->UpdateWeaponBaseAndHitboxTransforms();
}

void ThirdPersonCharacter::FixedUpdate()
{
}

void ThirdPersonCharacter::CreateCharacterView(const CharacterViewInfo* pCharacterViewInfo)
{
	SkinnedMeshRenderer* pSkinnedMeshRendererCharacter = m_hSkinnedMeshRendererCharacter.ToPtr();
	pSkinnedMeshRendererCharacter->SetMesh(pCharacterViewInfo->GetMesh());
	pSkinnedMeshRendererCharacter->SetArmature(pCharacterViewInfo->GetArmature());
	
	for (size_t i = 0; i < pCharacterViewInfo->GetMaterials().size(); ++i)
		pSkinnedMeshRendererCharacter->SetMaterial(i, pCharacterViewInfo->GetMaterials()[i]);
}

void ThirdPersonCharacter::UpdateWeaponBaseTransform()
{
	const SkinnedMeshRenderer* pSkinnedMeshRendererCharacter = m_hSkinnedMeshRendererCharacter.ToPtr();
	assert(pSkinnedMeshRendererCharacter);

	XMFLOAT3A s;
	XMFLOAT4A r;
	XMFLOAT3A t;
	if (!pSkinnedMeshRendererCharacter->GetBoneTransform("Hand.R", s, r, t))
		return;

	GameObject* pGameObjectTVWeaponBase = m_hGameObjectTVWeaponBase.ToPtr();
	assert(pGameObjectTVWeaponBase);

	// pGameObjectTVWeaponBase->m_transform.SetScale(s);
	pGameObjectTVWeaponBase->m_transform.SetRotationQuaternion(r);
	pGameObjectTVWeaponBase->m_transform.SetPosition(t);
}

void ThirdPersonCharacter::UpdateWeaponBaseAndHitboxTransforms()
{
	const SkinnedMeshRenderer* pCharacterSkinnedMeshRenderer = m_hSkinnedMeshRendererCharacter.ToPtr();

	BoneTransform bt[MAX_BONE_COUNT];
	pCharacterSkinnedMeshRenderer->GetBoneTransforms(bt, _countof(bt));


	// 1. 무기 베이스 오브젝트 위치 업데이트
	GameObject* pGameObjectTVWeaponBase = m_hGameObjectTVWeaponBase.ToPtr();
	pGameObjectTVWeaponBase->m_transform.SetRotationQuaternion(bt[m_biRightHand].m_rot);
	pGameObjectTVWeaponBase->m_transform.SetPosition(bt[m_biRightHand].m_translation);


	// 2. 히트박스 업데이트
	GameObject* pGameObjectHitboxBody = m_hGameObjectHitboxBody.ToPtr();
	pGameObjectHitboxBody->m_transform.SetRotationQuaternion(bt[m_biSpine0].m_rot);
	pGameObjectHitboxBody->m_transform.SetPosition(bt[m_biSpine0].m_translation);

	GameObject* pGameObjectHitboxNeck = m_hGameObjectHitboxNeck.ToPtr();
	pGameObjectHitboxNeck->m_transform.SetRotationQuaternion(bt[m_biNeck].m_rot);
	pGameObjectHitboxNeck->m_transform.SetPosition(bt[m_biNeck].m_translation);

	GameObject* pGameObjectHitboxHead = m_hGameObjectHitboxHead.ToPtr();
	pGameObjectHitboxHead->m_transform.SetRotationQuaternion(bt[m_biHead].m_rot);
	pGameObjectHitboxHead->m_transform.SetPosition(bt[m_biHead].m_translation);

	GameObject* pGameObjectHitboxLeftUpperArm = m_hGameObjectHitboxLeftUpperArm.ToPtr();
	pGameObjectHitboxLeftUpperArm->m_transform.SetRotationQuaternion(bt[m_biLeftUpperArm].m_rot);
	pGameObjectHitboxLeftUpperArm->m_transform.SetPosition(bt[m_biLeftUpperArm].m_translation);

	GameObject* pGameObjectHitboxRightUpperArm = m_hGameObjectHitboxRightUpperArm.ToPtr();
	pGameObjectHitboxRightUpperArm->m_transform.SetRotationQuaternion(bt[m_biRightUpperArm].m_rot);
	pGameObjectHitboxRightUpperArm->m_transform.SetPosition(bt[m_biRightUpperArm].m_translation);

	GameObject* pGameObjectHitboxLeftForeArm = m_hGameObjectHitboxLeftForeArm.ToPtr();
	pGameObjectHitboxLeftForeArm->m_transform.SetRotationQuaternion(bt[m_biLeftForeArm].m_rot);
	pGameObjectHitboxLeftForeArm->m_transform.SetPosition(bt[m_biLeftForeArm].m_translation);

	GameObject* pGameObjectHitboxRightForeArm = m_hGameObjectHitboxRightForeArm.ToPtr();
	pGameObjectHitboxRightForeArm->m_transform.SetRotationQuaternion(bt[m_biRightForeArm].m_rot);
	pGameObjectHitboxRightForeArm->m_transform.SetPosition(bt[m_biRightForeArm].m_translation);

	GameObject* pGameObjectHitboxLeftThigh = m_hGameObjectHitboxLeftThigh.ToPtr();
	pGameObjectHitboxLeftThigh->m_transform.SetRotationQuaternion(bt[m_biLeftThigh].m_rot);
	pGameObjectHitboxLeftThigh->m_transform.SetPosition(bt[m_biLeftThigh].m_translation);

	GameObject* pGameObjectHitboxRightThigh = m_hGameObjectHitboxRightThigh.ToPtr();
	pGameObjectHitboxRightThigh->m_transform.SetRotationQuaternion(bt[m_biRightThigh].m_rot);
	pGameObjectHitboxRightThigh->m_transform.SetPosition(bt[m_biRightThigh].m_translation);

	GameObject* pGameObjectHitboxLeftCalf = m_hGameObjectHitboxLeftCalf.ToPtr();
	pGameObjectHitboxLeftCalf->m_transform.SetRotationQuaternion(bt[m_biLeftCalf].m_rot);
	pGameObjectHitboxLeftCalf->m_transform.SetPosition(bt[m_biLeftCalf].m_translation);

	GameObject* pGameObjectHitboxRightCalf = m_hGameObjectHitboxRightCalf.ToPtr();
	pGameObjectHitboxRightCalf->m_transform.SetRotationQuaternion(bt[m_biRightCalf].m_rot);
	pGameObjectHitboxRightCalf->m_transform.SetPosition(bt[m_biRightCalf].m_translation);

	GameObject* pGameObjectHitboxLeftFoot = m_hGameObjectHitboxLeftFoot.ToPtr();
	pGameObjectHitboxLeftFoot->m_transform.SetRotationQuaternion(bt[m_biLeftToe].m_rot);
	pGameObjectHitboxLeftFoot->m_transform.SetPosition(bt[m_biLeftToe].m_translation);

	GameObject* pGameObjectHitboxRightFoot = m_hGameObjectHitboxRightFoot.ToPtr();
	pGameObjectHitboxRightFoot->m_transform.SetRotationQuaternion(bt[m_biRightToe].m_rot);
	pGameObjectHitboxRightFoot->m_transform.SetPosition(bt[m_biRightToe].m_translation);
}
