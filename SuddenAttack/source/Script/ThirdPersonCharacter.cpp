#include "ThirdPersonCharacter.h"
#include "../Resource/GlobalGameObjects.h"
#include "GameResources.h"

using namespace ze;

const XMFLOAT3 ThirdPersonCharacter::PRIMARY_WEAPON_TV_OFFSET(-0.004f, +0.06f, +0.03f);
const XMFLOAT3 ThirdPersonCharacter::SECONDARY_WEAPON_TV_OFFSET(-0.014f, +0.07f, +0.03f);

ThirdPersonCharacter::ThirdPersonCharacter(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_primaryWeaponLocalRot(0.0f, 0.0f, 0.0f, 0.0f)
	, m_secondaryWeaponLocalRot(0.0f, 0.0f, 0.0f, 0.0f)
{
}

void ThirdPersonCharacter::Awake()
{
	// GameResources 오브젝트 검색 및 스크립트 저장
	ze::GameObjectHandle hGameObjectGameResources = GameObject::Find(GO_GAME_RESOURCES_NAME);
	assert(hGameObjectGameResources.IsValid());

	m_hScriptGameResources = hGameObjectGameResources.ToPtr()->GetComponent<GameResources>();
	assert(m_hScriptGameResources.IsValid());
	std::shared_ptr<CapsuleCollider> spCharacterCollider = m_hScriptGameResources.ToPtr()->GetCharacterCollider();
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


	GameObjectHandle hGameObjectTVWeaponBase = Runtime::GetInstance()->CreateGameObject(L"tv_weapon_base");
	m_hGameObjectTVWeaponBase = hGameObjectTVWeaponBase;		// 오브젝트 핸들 저장
	GameObject* pGameObjectTVWeaponBase = hGameObjectTVWeaponBase.ToPtr();
	pGameObjectTVWeaponBase->m_transform.SetParent(&this->m_pGameObject->m_transform);



	// 캐릭터 SkinnedMesh 렌더러 컴포넌트 추가 및 설정 
	ComponentHandle<SkinnedMeshRenderer> hSkinnedMeshRendererThirdPersonCharacter = this->m_pGameObject->AddComponent<SkinnedMeshRenderer>();
	m_hSkinnedMeshRendererThirdPersonCharacter = hSkinnedMeshRendererThirdPersonCharacter;	// 컴포넌트 핸들을 멤버로 저장
	SkinnedMeshRenderer* pSkinnedMeshRendererThirdPersonCharacter = hSkinnedMeshRendererThirdPersonCharacter.ToPtr();



	GameResources* pScriptGameResources = m_hScriptGameResources.ToPtr();
	auto mdSteven = pScriptGameResources->GetModel(L"steven");
	auto spStevenMtl0 = pScriptGameResources->GetMaterial(L"steven_mtl0");
	auto spStevenMtl1 = pScriptGameResources->GetMaterial(L"steven_mtl1");
	auto spStevenMtl2 = pScriptGameResources->GetMaterial(L"steven_mtl2");

	pSkinnedMeshRendererThirdPersonCharacter->SetMesh(mdSteven.m_skinnedMesh);
	pSkinnedMeshRendererThirdPersonCharacter->SetArmature(mdSteven.m_armature);
	pSkinnedMeshRendererThirdPersonCharacter->SetMaterial(0, spStevenMtl0);
	pSkinnedMeshRendererThirdPersonCharacter->SetMaterial(1, spStevenMtl1);
	pSkinnedMeshRendererThirdPersonCharacter->SetMaterial(2, spStevenMtl2);

	// pSkinnedMeshRendererThirdPersonCharacter->PlayAnimation("run", false);
	// pSkinnedMeshRendererThirdPersonCharacter->PlayAnimation("reload_rifle", true);
	// pSkinnedMeshRendererThirdPersonCharacter->PlayAnimation("aim_rifle", true);
	// pSkinnedMeshRendererThirdPersonCharacter->PlayAnimation("aim_pistol", true);
	// pSkinnedMeshRendererThirdPersonCharacter->PlayAnimation("reload_pistol", true);
	pSkinnedMeshRendererThirdPersonCharacter->PlayGroupAnimation("aim_rifle", "lower_body", true);
	// pSkinnedMeshRendererThirdPersonCharacter->PlayGroupAnimation("shoot_pistol", "upper_body", true);
	// pSkinnedMeshRendererThirdPersonCharacter->PlayGroupAnimation("reload_pistol", "upper_body", true
	pSkinnedMeshRendererThirdPersonCharacter->PlayGroupAnimation("aim_rifle", "upper_body", true);
}

void ThirdPersonCharacter::Update()
{
	const SkinnedMeshRenderer* pSkinnedMeshRendererThirdPersonCharacter = m_hSkinnedMeshRendererThirdPersonCharacter.ToPtr();
	assert(pSkinnedMeshRendererThirdPersonCharacter);

	XMFLOAT3A s;
	XMFLOAT4A r;
	XMFLOAT3A t;
	if (!pSkinnedMeshRendererThirdPersonCharacter->GetBoneTransform("Hand.R", s, r, t))
		return;

	GameObject* pGameObjectTVWeaponBase = m_hGameObjectTVWeaponBase.ToPtr();
	assert(pGameObjectTVWeaponBase);

	// pGameObjectTVWeaponBase->m_transform.SetScale(s);
	pGameObjectTVWeaponBase->m_transform.SetRotationQuaternion(r);
	pGameObjectTVWeaponBase->m_transform.SetPosition(t);
}
