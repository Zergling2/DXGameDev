#include "ThirdPersonCharacter.h"
#include "..\Resource\GlobalScriptGameObject.h"
#include "..\Resource\Character.h"
#include "..\Resource\WeaponDefinition.h"
#include "..\Resource\HitboxName.h"
#include "GameResources.h"

using namespace ze;

const XMFLOAT3 ThirdPersonCharacter::s_weaponTVOffset[] =
{
	XMFLOAT3(-0.004f, +0.06f, +0.03f),
	XMFLOAT3(-0.014f, +0.07f, +0.03f)
};
bool ThirdPersonCharacter::s_weaponLocalRotCalc = false;
XMFLOAT4 ThirdPersonCharacter::s_weaponLocalRotQuaternion[];

ThirdPersonCharacter::ThirdPersonCharacter(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_hitboxActivated(true)
	, m_accountId(0)
	, m_currWeaponSlot(WeaponSlot::Unknown)
	, m_spWeaponDefs{}
	, m_hScriptGameResources()
	, m_hCharacterColliderRigidbody()
	, m_hSkinnedMeshRendererCharacter()
	, m_hMeshRendererTVWeapon()
	, m_hGameObjectTVWeaponBase()
	, m_hGameObjectTVWeapon()
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
	, m_action(WeaponAction::None)
	, m_pCurrWeaponEventTable(nullptr)
	, m_eventIndexCursor(0)
	, m_actionDuration(0.0f)
	, m_actionElapsed(0.0f)
	, m_isDead(true)
	, m_team(GameTeam::Unknown)
	, m_prevMoveType(MovementType::Unknown)
{
}

void ThirdPersonCharacter::Awake()
{
	if (!s_weaponLocalRotCalc)
	{
		// TV 무기 베이스 오프셋 계산 및 저장
		XMVECTOR primaryWeaponLocalRot = XMQuaternionRotationNormal(Vector3::Up(), XMConvertToRadians(+90));
		primaryWeaponLocalRot = XMQuaternionMultiply(primaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Forward(), XMConvertToRadians(+81)));
		primaryWeaponLocalRot = XMQuaternionMultiply(primaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Right(), XMConvertToRadians(+8)));
		XMStoreFloat4(&s_weaponLocalRotQuaternion[static_cast<size_t>(WeaponSlot::Primary)], primaryWeaponLocalRot);

		XMVECTOR secondaryWeaponLocalRot = XMQuaternionRotationNormal(Vector3::Up(), XMConvertToRadians(+90));
		secondaryWeaponLocalRot = XMQuaternionMultiply(secondaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Forward(), XMConvertToRadians(+81)));
		secondaryWeaponLocalRot = XMQuaternionMultiply(secondaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Right(), XMConvertToRadians(-10)));
		XMStoreFloat4(&s_weaponLocalRotQuaternion[static_cast<size_t>(WeaponSlot::Secondary)], secondaryWeaponLocalRot);

		s_weaponLocalRotCalc = true;
	}

	// ###################################################################
	// GameResources 스크립트 검색 및 저장
	GameObjectHandle hGameObjectGameResources = GameObject::Find(GAME_RESOURCES_GAME_OBJECT_NAME);
	assert(hGameObjectGameResources.IsValid());

	ComponentHandle<GameResources> hScriptGameResources = hGameObjectGameResources.ToPtr()->GetComponent<GameResources>();
	assert(hScriptGameResources.IsValid());
	m_hScriptGameResources = hScriptGameResources;
	const GameResources* pScriptGameResources = hScriptGameResources.ToPtr();


	// ###################################################################
	// 캐릭터 콜라이더 생성
	std::shared_ptr<CapsuleCollider> spCharacterCollider = pScriptGameResources->GetCharacterCollider();
	ComponentHandle<Rigidbody> hCharacterColliderRigidbody = m_pGameObject->AddComponent<Rigidbody>(
		spCharacterCollider,
		XMFLOAT3(0.0f, pScriptGameResources->GetCharacterColliderTotalHeight() / 2.0f, 0.0f)
	);
	m_hCharacterColliderRigidbody = hCharacterColliderRigidbody;
	Rigidbody* pCharacterColliderRigidbody = m_hCharacterColliderRigidbody.ToPtr();
	pCharacterColliderRigidbody->SetBodyType(RigidbodyType::Kinematic);

	// TV 무기 베이스 오브젝트 생성
	GameObjectHandle hGameObjectTVWeaponBase = Runtime::GetInstance()->CreateGameObject(L"TVWeaponBase");
	m_hGameObjectTVWeaponBase = hGameObjectTVWeaponBase;		// 오브젝트 핸들 저장
	GameObject* pGameObjectTVWeaponBase = hGameObjectTVWeaponBase.ToPtr();
	pGameObjectTVWeaponBase->m_transform.SetParent(&m_pGameObject->m_transform);

	// TV 테스트 무기
	{
		GameObjectHandle hGameObjTVWeapon = Runtime::GetInstance()->CreateGameObject(L"TVWeapon");
		m_hGameObjectTVWeapon = hGameObjTVWeapon;
		GameObject* pGameObjTVWeapon = hGameObjTVWeapon.ToPtr();
		pGameObjTVWeapon->m_transform.SetParent(&pGameObjectTVWeaponBase->m_transform);	// TVWeaponBase 게임오브젝트의 자식으로 추가해야 함.
		ComponentHandle<MeshRenderer> hWeaponMeshRendererTVWeapon = pGameObjTVWeapon->AddComponent<MeshRenderer>();
		m_hMeshRendererTVWeapon = hWeaponMeshRendererTVWeapon;
	}


	// 캐릭터 SkinnedMesh 오브젝트 생성 및 초기화
	ComponentHandle<SkinnedMeshRenderer> hSkinnedMeshRendererCharacter = m_pGameObject->AddComponent<SkinnedMeshRenderer>();
	m_hSkinnedMeshRendererCharacter = hSkinnedMeshRendererCharacter;	// 컴포넌트 핸들을 멤버로 저장
	SkinnedMeshRenderer* pSkinnedMeshRendererCharacter = hSkinnedMeshRendererCharacter.ToPtr();
	const CharacterViewInfo* pCharacterViewInfo = pScriptGameResources->GetCharacterViewInfo(L"steven.rt");
	this->SetCharacterView(pCharacterViewInfo);

	// ##############################################################################
	// 캐릭터 히트박스 생성
	m_hGameObjectHitboxBody = Runtime::GetInstance()->CreateGameObject(HTB_BODY_NAME);
	GameObject* pGameObjectHitboxBody = m_hGameObjectHitboxBody.ToPtr();
	pGameObjectHitboxBody->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxBody = pGameObjectHitboxBody->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterBodyCollider(),
		XMFLOAT3(
			0.0f,
			pScriptGameResources->GetCharacterBodyColliderHalfExtents().y - 0.06f,
			0.03f
		)
	);
	Rigidbody* pRigidbodyHitboxBody = m_hRigidbodyHitboxBody.ToPtr();
	pRigidbodyHitboxBody->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxBody->SetTrigger(true);

	m_hGameObjectHitboxNeck = Runtime::GetInstance()->CreateGameObject(HTB_NECK_NAME);
	GameObject* pGameObjectHitboxNeck = m_hGameObjectHitboxNeck.ToPtr();
	pGameObjectHitboxNeck->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxNeck = pGameObjectHitboxNeck->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterNeckCollider(),
		XMFLOAT3(
			0.0f,
			pScriptGameResources->GetCharacterNeckColliderHeight(),
			0.0f
		)
	);
	Rigidbody* pRigidbodyHitboxNeck = m_hRigidbodyHitboxNeck.ToPtr();
	pRigidbodyHitboxNeck->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxNeck->SetTrigger(true);

	m_hGameObjectHitboxHead = Runtime::GetInstance()->CreateGameObject(HTB_HEAD_NAME);
	GameObject* pGameObjectHitboxHead = m_hGameObjectHitboxHead.ToPtr();
	pGameObjectHitboxHead->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxHead = pGameObjectHitboxHead->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterHeadCollider(),
		XMFLOAT3(
			0.0f,
			pScriptGameResources->GetCharacterHeadColliderRadius(),
			0.02f
		)
	);
	Rigidbody* pRigidbodyHitboxHead = m_hRigidbodyHitboxHead.ToPtr();
	pRigidbodyHitboxHead->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxHead->SetTrigger(true);

	m_hGameObjectHitboxLeftUpperArm = Runtime::GetInstance()->CreateGameObject(HTB_UPPER_ARM_NAME);
	GameObject* pGameObjectHitboxLeftUpperArm = m_hGameObjectHitboxLeftUpperArm.ToPtr();
	pGameObjectHitboxLeftUpperArm->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxLeftUpperArm = pGameObjectHitboxLeftUpperArm->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterUpperArmCollider(),
		XMFLOAT3(
			0.0f,
			(pScriptGameResources->GetCharacterUpperArmColliderRadius() + pScriptGameResources->GetCharacterUpperArmColliderHeight()) / 2.0f,
			0.0f
		)
	);
	Rigidbody* pRigidbodyHitboxLeftUpperArm = m_hRigidbodyHitboxLeftUpperArm.ToPtr();
	pRigidbodyHitboxLeftUpperArm->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxLeftUpperArm->SetTrigger(true);

	m_hGameObjectHitboxRightUpperArm = Runtime::GetInstance()->CreateGameObject(HTB_UPPER_ARM_NAME);
	GameObject* pGameObjectHitboxRightUpperArm = m_hGameObjectHitboxRightUpperArm.ToPtr();
	pGameObjectHitboxRightUpperArm->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxRightUpperArm = pGameObjectHitboxRightUpperArm->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterUpperArmCollider(),
		XMFLOAT3(
			0.0f,
			(pScriptGameResources->GetCharacterUpperArmColliderRadius() + pScriptGameResources->GetCharacterUpperArmColliderHeight()) / 2.0f,
			0.0f
		)
	);
	Rigidbody* pRigidbodyHitboxRightUpperArm = m_hRigidbodyHitboxRightUpperArm.ToPtr();
	pRigidbodyHitboxRightUpperArm->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxRightUpperArm->SetTrigger(true);

	m_hGameObjectHitboxLeftForeArm = Runtime::GetInstance()->CreateGameObject(HTB_FORE_ARM_NAME);
	GameObject* pGameObjectHitboxLeftForeArm = m_hGameObjectHitboxLeftForeArm.ToPtr();
	pGameObjectHitboxLeftForeArm->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxLeftForeArm = pGameObjectHitboxLeftForeArm->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterForeArmCollider(),
		XMFLOAT3(
			0.0f,
			(pScriptGameResources->GetCharacterForeArmColliderRadius() + pScriptGameResources->GetCharacterForeArmColliderHeight()) / 2.0f,
			0.0f
		)
	);
	Rigidbody* pRigidbodyHitboxLeftForeArm = m_hRigidbodyHitboxLeftForeArm.ToPtr();
	pRigidbodyHitboxLeftForeArm->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxLeftForeArm->SetTrigger(true);

	m_hGameObjectHitboxRightForeArm = Runtime::GetInstance()->CreateGameObject(HTB_FORE_ARM_NAME);
	GameObject* pGameObjectHitboxRightForeArm = m_hGameObjectHitboxRightForeArm.ToPtr();
	pGameObjectHitboxRightForeArm->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxRightForeArm = pGameObjectHitboxRightForeArm->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterForeArmCollider(),
		XMFLOAT3(
			0.0f,
			(pScriptGameResources->GetCharacterForeArmColliderRadius() + pScriptGameResources->GetCharacterForeArmColliderHeight()) / 2.0f,
			0.0f
		)
	);
	Rigidbody* pRigidbodyHitboxRightForeArm = m_hRigidbodyHitboxRightForeArm.ToPtr();
	pRigidbodyHitboxRightForeArm->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxRightForeArm->SetTrigger(true);

	m_hGameObjectHitboxLeftThigh = Runtime::GetInstance()->CreateGameObject(HTB_THIGH_NAME);
	GameObject* pGameObjectHitboxLeftThigh = m_hGameObjectHitboxLeftThigh.ToPtr();
	pGameObjectHitboxLeftThigh->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxLeftThigh = pGameObjectHitboxLeftThigh->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterThighCollider(),
		XMFLOAT3(
			0.0f,
			(pScriptGameResources->GetCharacterThighColliderRadius() + pScriptGameResources->GetCharacterThighColliderHeight()) / 2.0f - 0.075f,
			0.0f
		)
	);
	Rigidbody* pRigidbodyHitboxLeftThigh = m_hRigidbodyHitboxLeftThigh.ToPtr();
	pRigidbodyHitboxLeftThigh->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxLeftThigh->SetTrigger(true);

	m_hGameObjectHitboxRightThigh = Runtime::GetInstance()->CreateGameObject(HTB_THIGH_NAME);
	GameObject* pGameObjectHitboxRightThigh = m_hGameObjectHitboxRightThigh.ToPtr();
	pGameObjectHitboxRightThigh->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxRightThigh = pGameObjectHitboxRightThigh->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterThighCollider(),
		XMFLOAT3(
			0.0f,
			(pScriptGameResources->GetCharacterThighColliderRadius() + pScriptGameResources->GetCharacterThighColliderHeight()) / 2.0f - 0.075f,
			0.0f
		)
	);
	Rigidbody* pRigidbodyHitboxRightThigh = m_hRigidbodyHitboxRightThigh.ToPtr();
	pRigidbodyHitboxRightThigh->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxRightThigh->SetTrigger(true);

	m_hGameObjectHitboxLeftCalf = Runtime::GetInstance()->CreateGameObject(HTB_CALF_NAME);
	GameObject* pGameObjectHitboxLeftCalf = m_hGameObjectHitboxLeftCalf.ToPtr();
	pGameObjectHitboxLeftCalf->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxLeftCalf = pGameObjectHitboxLeftCalf->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterCalfCollider(),
		XMFLOAT3(
			0.0f,
			(pScriptGameResources->GetCharacterCalfColliderRadius() + pScriptGameResources->GetCharacterCalfColliderHeight()) / 2.0f,
			0.0f
		)
	);
	Rigidbody* pRigidbodyHitboxLeftCalf = m_hRigidbodyHitboxLeftCalf.ToPtr();
	pRigidbodyHitboxLeftCalf->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxLeftCalf->SetTrigger(true);

	m_hGameObjectHitboxRightCalf = Runtime::GetInstance()->CreateGameObject(HTB_CALF_NAME);
	GameObject* pGameObjectHitboxRightCalf = m_hGameObjectHitboxRightCalf.ToPtr();
	pGameObjectHitboxRightCalf->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxRightCalf = pGameObjectHitboxRightCalf->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterCalfCollider(),
		XMFLOAT3(
			0.0f,
			(pScriptGameResources->GetCharacterCalfColliderRadius() + pScriptGameResources->GetCharacterCalfColliderHeight()) / 2.0f,
			0.0f
		)
	);
	Rigidbody* pRigidbodyHitboxRightCalf = m_hRigidbodyHitboxRightCalf.ToPtr();
	pRigidbodyHitboxRightCalf->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxRightCalf->SetTrigger(true);

	m_hGameObjectHitboxLeftFoot = Runtime::GetInstance()->CreateGameObject(HTB_FOOT_NAME);
	GameObject* pGameObjectHitboxLeftFoot = m_hGameObjectHitboxLeftFoot.ToPtr();
	pGameObjectHitboxLeftFoot->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxLeftFoot = pGameObjectHitboxLeftFoot->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterFootCollider(),
		XMFLOAT3(
			0.0f,
			-0.075f,
			-0.01f
		)
	);
	Rigidbody* pRigidbodyHitboxLeftFoot = m_hRigidbodyHitboxLeftFoot.ToPtr();
	pRigidbodyHitboxLeftFoot->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxLeftFoot->SetTrigger(true);

	m_hGameObjectHitboxRightFoot = Runtime::GetInstance()->CreateGameObject(HTB_FOOT_NAME);
	GameObject* pGameObjectHitboxRightFoot = m_hGameObjectHitboxRightFoot.ToPtr();
	pGameObjectHitboxRightFoot->m_transform.SetParent(&m_pGameObject->m_transform);
	m_hRigidbodyHitboxRightFoot = pGameObjectHitboxRightFoot->AddComponent<Rigidbody>(
		pScriptGameResources->GetCharacterFootCollider(),
		XMFLOAT3(
			0.0f,
			-0.075f,
			-0.01f
		)
	);
	Rigidbody* pRigidbodyHitboxRightFoot = m_hRigidbodyHitboxRightFoot.ToPtr();
	pRigidbodyHitboxRightFoot->SetBodyType(RigidbodyType::Kinematic);
	pRigidbodyHitboxRightFoot->SetTrigger(true);
	// ##############################################################################



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

	const Armature* const pCharacterArmature = pSkinnedMeshRendererCharacter->GetArmaturePtr();
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
	const float dt = ze::Time::GetInstance()->GetDeltaTime();

	if (m_hitboxActivated)
		this->UpdateTVWeaponBaseAndHitboxTransforms();
	

	if (m_action != WeaponAction::Idle && m_action != WeaponAction::Death1Idle && m_action != WeaponAction::None)
	{
		m_actionElapsed += dt;		// 업데이트

		/*
		// 이벤트 테이블 참조 및 이벤트 처리
		if (m_pCurrWeaponEventTable)
		{
			while (m_eventIndexCursor < m_pCurrWeaponEventTable->m_events.size())
			{
				const auto& item = m_pCurrWeaponEventTable->m_events[m_eventIndexCursor];
				if (item.first <= m_actionElapsed)
				{
					// 무기 이벤트 처리
					// 
					// 1. 사운드 처리
					const auto iter = m_spWeaponDef->m_weaponSounds.find(item.second);
					if (iter != m_spWeaponDef->m_weaponSounds.cend())
					{
						if (iter->second)
							iter->second->Play();
					}

					++m_eventIndexCursor;
				}
				else
				{
					break;
				}
			}
		}
		*/

		const float exceed = m_actionElapsed - m_actionDuration;
		switch (m_action)
		{
		case WeaponAction::Draw:
			if (exceed >= 0)
				this->OnIdle(exceed);
			break;
		case WeaponAction::Reload:
			if (exceed >= 0)
			{
				// this->OnReloadFinish();
				this->OnIdle(exceed);
			}
			break;
		case WeaponAction::Fire:
			// m_fireCoolTime = (std::max)(m_fireCoolTime - dt, 0.0f);
			if (exceed >= 0)
				this->OnIdle(exceed);
			break;
		case WeaponAction::Death1:
			if (exceed >= 0)
				this->OnDeadIdle(exceed, WeaponAction::Death1);
			break;
		default:
			break;
		}
	}
}

void ThirdPersonCharacter::SetCharacterView(const CharacterViewInfo* pCVI)
{
	SkinnedMeshRenderer* pSkinnedMeshRendererCharacter = m_hSkinnedMeshRendererCharacter.ToPtr();

	if (pCVI)
	{
		pSkinnedMeshRendererCharacter->SetMesh(pCVI->GetMesh());
		pSkinnedMeshRendererCharacter->SetArmature(pCVI->GetArmature());

		const auto& mtls = pCVI->GetMaterials();
		for (size_t i = 0; i < mtls.size(); ++i)
			pSkinnedMeshRendererCharacter->SetMaterial(i, mtls[i]);
	}
	else
	{
		pSkinnedMeshRendererCharacter->SetMesh(nullptr);
		pSkinnedMeshRendererCharacter->SetArmature(nullptr);
	}
}

void ThirdPersonCharacter::SetWeaponInUse(WeaponSlot slot, WeaponCode weaponCode)
{
	const GameResources* pScriptGameResources = m_hScriptGameResources.ToPtr();

	m_spWeaponDefs[static_cast<size_t>(slot)] = pScriptGameResources->GetWeaponDefinition(weaponCode);
}

void ThirdPersonCharacter::ShowView()
{
	m_hSkinnedMeshRendererCharacter.ToPtr()->Enable();
	m_hMeshRendererTVWeapon.ToPtr()->Enable();
}

void ThirdPersonCharacter::HideView()
{
	m_hSkinnedMeshRendererCharacter.ToPtr()->Disable();
	m_hMeshRendererTVWeapon.ToPtr()->Disable();
}

void ThirdPersonCharacter::OnInit(uint32_t accountId, GameTeam team, WeaponCode primary, WeaponCode secondary, WeaponSlot currWeapon, InGamePlayerState state,
	const XMFLOAT3& pos, const XMFLOAT4& rot, float camRotX)
{
	const GameResources* pScriptGameResources = m_hScriptGameResources.ToPtr();

	m_accountId = accountId;
	m_team = team;
	m_currWeaponSlot = currWeapon;
	m_prevMoveType = MovementType::Unknown;

	// 1. TPC 캐릭터 뷰 설정
	const CharacterViewInfo* pCVI = nullptr;
	switch (m_team)
	{
	case GameTeam::RedTeam:
		pCVI = pScriptGameResources->GetCharacterViewInfo(L"steven.rt");
		break;
	case GameTeam::BlueTeam:
		pCVI = pScriptGameResources->GetCharacterViewInfo(L"steven.bt");
		break;
	default:
		*reinterpret_cast<int*>(0) = 0;
		break;
	}
	this->SetCharacterView(pCVI);

	// 2. 무기 뷰 설정
	this->SetWeaponInUse(WeaponSlot::Primary, primary);
	this->SetWeaponInUse(WeaponSlot::Secondary, secondary);

	// 3. 상태에 따른 뷰 표시 제어
	switch (state)
	{
	case InGamePlayerState::Alive:
		m_isDead = false;
		m_pGameObject->m_transform.SetPosition(pos);
		m_pGameObject->m_transform.SetRotationQuaternion(rot);
		// this->SetBoneAdditiveBlending(pPacket->m_camRotX);
		this->ShowView();
		this->OnDraw(m_currWeaponSlot);
		// this->OnIdle(0.0f);	// 무기를 들고 무기에 대한 상호작용은 하지 않고 있는 상태.
		m_hSkinnedMeshRendererCharacter.ToPtr()->PlayGroupAnimation("stand_idle", "lower_body", true);
		break;
	case InGamePlayerState::Dead:
		m_isDead = true;
		this->HideView();
		break;
	case InGamePlayerState::Spectating:
		m_isDead = true;
		this->HideView();
		break;
	default:
		break;
	}
}

void ThirdPersonCharacter::OnDraw(WeaponSlot slot)
{
	if (slot < WeaponSlot::Count)
	{
		m_currWeaponSlot = slot;

		const WeaponDefinition* pWeaponDef = m_spWeaponDefs[static_cast<size_t>(m_currWeaponSlot)].get();

		// 1. 로컬 트랜스폼 설정
		GameObject* pGameObjTVWeapon = m_hGameObjectTVWeapon.ToPtr();
		pGameObjTVWeapon->m_transform.SetRotationQuaternion(s_weaponLocalRotQuaternion[static_cast<size_t>(m_currWeaponSlot)]);
		pGameObjTVWeapon->m_transform.SetPosition(s_weaponTVOffset[static_cast<size_t>(m_currWeaponSlot)]);


		// 2. 드로잉하는 무기로 메시를 업데이트
		MeshRenderer* pMeshRendererTVWeapon = m_hMeshRendererTVWeapon.ToPtr();
		pMeshRendererTVWeapon->SetMesh(pWeaponDef->m_spTVMesh);
		for (size_t i = 0; i < pWeaponDef->m_materials.size(); ++i)
			pMeshRendererTVWeapon->SetMaterial(i, pWeaponDef->m_materials[i]);


		m_action = WeaponAction::Draw;

		auto eventTableIter = pWeaponDef->m_eventTables.find(m_action);
		m_pCurrWeaponEventTable = eventTableIter == pWeaponDef->m_eventTables.end() ? nullptr : eventTableIter->second.get();
		m_eventIndexCursor = 0;
		m_actionDuration = pWeaponDef->GetDrawTime();
		m_actionElapsed = 0.0f;

		// 3. 애니메이션 재생
		this->PlayWeaponAnimation(WeaponAction::Draw, false);
	}
}

void ThirdPersonCharacter::OnFire()
{
	if (m_currWeaponSlot < WeaponSlot::Count)
	{
		const WeaponDefinition* pWeaponDef = m_spWeaponDefs[static_cast<size_t>(m_currWeaponSlot)].get();

		m_action = WeaponAction::Fire;
		auto eventTableIter = pWeaponDef->m_eventTables.find(m_action);
		m_pCurrWeaponEventTable = eventTableIter == pWeaponDef->m_eventTables.end() ? nullptr : eventTableIter->second.get();
		m_eventIndexCursor = 0;

		m_actionDuration = pWeaponDef->GetRecoilTime();
		m_actionElapsed = 0.0f;

		// 3. 애니메이션 재생
		this->PlayWeaponAnimation(WeaponAction::Fire, false);
	}
}

void ThirdPersonCharacter::OnReload()
{
	if (m_currWeaponSlot < WeaponSlot::Count)
	{
		const WeaponDefinition* pWeaponDef = m_spWeaponDefs[static_cast<size_t>(m_currWeaponSlot)].get();

		m_action = WeaponAction::Reload;
		auto eventTableIter = pWeaponDef->m_eventTables.find(m_action);
		m_pCurrWeaponEventTable = eventTableIter == pWeaponDef->m_eventTables.end() ? nullptr : eventTableIter->second.get();
		m_eventIndexCursor = 0;

		m_actionDuration = pWeaponDef->GetReloadTime();
		m_actionElapsed = 0.0f;

		this->PlayWeaponAnimation(WeaponAction::Reload, false);
	}
}

void ThirdPersonCharacter::OnIdle(float exceed)
{
	if (m_currWeaponSlot < WeaponSlot::Count)
	{
		m_action = WeaponAction::Idle;
		m_actionDuration = 0.0f;
		m_actionElapsed = 0.0f;

		this->PlayWeaponAnimation(WeaponAction::Idle, true);
	}
}

void ThirdPersonCharacter::OnDead(WeaponAction deadAction)
{
	m_isDead = true;

	m_prevMoveType = MovementType::Unknown;

	// 캐릭터 콜라이더 비활성화
	m_hCharacterColliderRigidbody.ToPtr()->Disable();

	if (m_currWeaponSlot < WeaponSlot::Count)
	{
		m_action = WeaponAction::Death1;
		m_actionDuration = CharacterViewInfo::GetDeathAnimTime();
		m_actionElapsed = 0.0f;

		this->PlayDeathAnimation(WeaponAction::Death1, false);
	}
}

void ThirdPersonCharacter::OnDeadIdle(float exceed, WeaponAction deathAction)
{
	if (m_currWeaponSlot < WeaponSlot::Count)
	{
		m_action = WeaponAction::Death1Idle;
		m_actionDuration = 0.0f;
		m_actionElapsed = 0.0f;

		this->PlayDeathAnimation(WeaponAction::Death1Idle, true);
	}
}

void ThirdPersonCharacter::OnRespawn(const XMFLOAT3& pos, const XMFLOAT4& rot, float camRotX)
{
	m_isDead = false;

	m_pGameObject->m_transform.SetPosition(pos);
	m_pGameObject->m_transform.SetRotationQuaternion(rot);
	// this->SetBoneAdditiveBlending(camRotX);
	this->ShowView();
	this->OnDraw(WeaponSlot::Secondary);

	m_prevMoveType = MovementType::Unknown;

	m_hSkinnedMeshRendererCharacter.ToPtr()->PlayGroupAnimation("stand_idle", "lower_body", true);

	// 캐릭터 콜라이더 활성화
	m_hCharacterColliderRigidbody.ToPtr()->Enable();
}

void ThirdPersonCharacter::OnTransform(const XMFLOAT3& pos, const XMFLOAT4& rot, float camRotX, MovementType moveType)
{
	m_pGameObject->m_transform.SetPosition(pos);
	m_pGameObject->m_transform.SetRotationQuaternion(rot);
	// this->SetBoneAdditiveBlending(camRotX);

	if (!m_isDead)
	{
		if (m_prevMoveType != moveType)	// 이동 애니메이션 교체 필요.
		{
			switch (moveType)
			{
			case MovementType::Stop:
				m_hSkinnedMeshRendererCharacter.ToPtr()->PlayGroupAnimation("stand_idle", "lower_body", true);
				break;
			case MovementType::LeftRight:
				m_hSkinnedMeshRendererCharacter.ToPtr()->PlayGroupAnimation("run_horizontal", "lower_body", true);
				break;
			case MovementType::ForwardBackward:
				m_hSkinnedMeshRendererCharacter.ToPtr()->PlayGroupAnimation("run", "lower_body", true);
				break;
			case MovementType::Diagonal:
				m_hSkinnedMeshRendererCharacter.ToPtr()->PlayGroupAnimation("run_horizontal", "lower_body", true);
				break;
			default:
				break;
			}
		}
	}

	m_prevMoveType = moveType;
}

void ThirdPersonCharacter::ActivateCharacterColliderAndHitbox()
{
	m_hCharacterColliderRigidbody.ToPtr()->Enable();
	m_hRigidbodyHitboxBody.ToPtr()->Enable();
	m_hRigidbodyHitboxNeck.ToPtr()->Enable();
	m_hRigidbodyHitboxHead.ToPtr()->Enable();
	m_hRigidbodyHitboxLeftUpperArm.ToPtr()->Enable();
	m_hRigidbodyHitboxRightUpperArm.ToPtr()->Enable();
	m_hRigidbodyHitboxLeftForeArm.ToPtr()->Enable();
	m_hRigidbodyHitboxRightForeArm.ToPtr()->Enable();
	m_hRigidbodyHitboxLeftThigh.ToPtr()->Enable();
	m_hRigidbodyHitboxRightThigh.ToPtr()->Enable();
	m_hRigidbodyHitboxLeftCalf.ToPtr()->Enable();
	m_hRigidbodyHitboxRightCalf.ToPtr()->Enable();
	m_hRigidbodyHitboxLeftFoot.ToPtr()->Enable();
	m_hRigidbodyHitboxRightFoot.ToPtr()->Enable();
	
	m_hitboxActivated = true;
}

void ThirdPersonCharacter::DeactivateCharacterColliderAndHitbox()
{
	m_hCharacterColliderRigidbody.ToPtr()->Disable();
	m_hRigidbodyHitboxBody.ToPtr()->Disable();
	m_hRigidbodyHitboxNeck.ToPtr()->Disable();
	m_hRigidbodyHitboxHead.ToPtr()->Disable();
	m_hRigidbodyHitboxLeftUpperArm.ToPtr()->Disable();
	m_hRigidbodyHitboxRightUpperArm.ToPtr()->Disable();
	m_hRigidbodyHitboxLeftForeArm.ToPtr()->Disable();
	m_hRigidbodyHitboxRightForeArm.ToPtr()->Disable();
	m_hRigidbodyHitboxLeftThigh.ToPtr()->Disable();
	m_hRigidbodyHitboxRightThigh.ToPtr()->Disable();
	m_hRigidbodyHitboxLeftCalf.ToPtr()->Disable();
	m_hRigidbodyHitboxRightCalf.ToPtr()->Disable();
	m_hRigidbodyHitboxLeftFoot.ToPtr()->Disable();
	m_hRigidbodyHitboxRightFoot.ToPtr()->Disable();

	m_hitboxActivated = false;
}

/*
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
*/

void ThirdPersonCharacter::UpdateTVWeaponBaseAndHitboxTransforms()
{
	const SkinnedMeshRenderer* pCharacterSkinnedMeshRenderer = m_hSkinnedMeshRendererCharacter.ToPtr();

	BoneTransform bt[MAX_BONE_COUNT];
	pCharacterSkinnedMeshRenderer->GetBoneTransforms(bt, _countof(bt));


	// 1. 무기 베이스 오브젝트 위치 업데이트 (오른손 뼈의 Transform 사용)
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

void ThirdPersonCharacter::PlayWeaponAnimation(WeaponAction action, bool loop)
{
	if (m_currWeaponSlot < WeaponSlot::Count)
	{
		const WeaponDefinition* const pWeaponDef = m_spWeaponDefs[static_cast<size_t>(m_currWeaponSlot)].get();

		const auto animIter = pWeaponDef->m_tvAnims.find(action);
		if (animIter != pWeaponDef->m_tvAnims.cend())
		{
			m_hSkinnedMeshRendererCharacter.ToPtr()->PlayGroupAnimation(animIter->second, "upper_body", loop);	// 무기는 상체 애니메이션만 관여
		}
		else
		{
			wprintf(L"[ThirdPersonCharacter::PlayWeaponAnimation()] Can not find animation item about '%s'.", pWeaponDef->GetName().c_str());
		}
	}
}

void ThirdPersonCharacter::PlayDeathAnimation(WeaponAction deathAction, bool loop)
{
	if (m_currWeaponSlot < WeaponSlot::Count)
	{
		const WeaponDefinition* const pWeaponDef = m_spWeaponDefs[static_cast<size_t>(m_currWeaponSlot)].get();

		const auto animIter = pWeaponDef->m_tvAnims.find(deathAction);
		if (animIter != pWeaponDef->m_tvAnims.cend())
		{
			m_hSkinnedMeshRendererCharacter.ToPtr()->PlayAnimation(animIter->second, loop);	// 상하체 모두 재생
		}
		else
		{
			wprintf(L"[ThirdPersonCharacter::PlayDeathAnimation()] Can not find animation item about '%s'.", pWeaponDef->GetName().c_str());
		}
	}
}
