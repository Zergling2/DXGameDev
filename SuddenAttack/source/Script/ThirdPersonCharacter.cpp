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


	GameObjectHandle hGameObjectTVWeaponBase = Runtime::GetInstance()->CreateGameObject(L"tv_weapon_base");
	m_hGameObjectTVWeaponBase = hGameObjectTVWeaponBase;		// 오브젝트 핸들 저장
	GameObject* pGameObjectTVWeaponBase = hGameObjectTVWeaponBase.ToPtr();
	pGameObjectTVWeaponBase->m_transform.SetParent(&this->m_pGameObject->m_transform);

	// 캐릭터 SkinnedMesh 렌더러 컴포넌트 추가 및 설정
	ComponentHandle<SkinnedMeshRenderer> hSkinnedMeshRendererCharacter = this->m_pGameObject->AddComponent<SkinnedMeshRenderer>();
	m_hSkinnedMeshRendererCharacter = hSkinnedMeshRendererCharacter;	// 컴포넌트 핸들을 멤버로 저장
	SkinnedMeshRenderer* pSkinnedMeshRendererCharacter = hSkinnedMeshRendererCharacter.ToPtr();

	

	const CharacterViewInfo* pCharacterViewInfo = pScriptGameResources->GetCharacterViewInfo(L"steven");
	this->CreateCharacterView(pCharacterViewInfo);

	// pSkinnedMeshRendererCharacter->PlayAnimation("run", false);
	// pSkinnedMeshRendererCharacter->PlayAnimation("reload_rifle", true);
	// pSkinnedMeshRendererCharacter->PlayAnimation("aim_rifle", true);
	// pSkinnedMeshRendererCharacter->PlayAnimation("aim_pistol", true);
	// pSkinnedMeshRendererCharacter->PlayAnimation("reload_pistol", true);
	pSkinnedMeshRendererCharacter->PlayGroupAnimation("aim_rifle", "lower_body", true);
	// pSkinnedMeshRendererCharacter->PlayGroupAnimation("shoot_pistol", "upper_body", true);
	// pSkinnedMeshRendererCharacter->PlayGroupAnimation("reload_pistol", "upper_body", true
	pSkinnedMeshRendererCharacter->PlayGroupAnimation("aim_rifle", "upper_body", true);
}

void ThirdPersonCharacter::Update()
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

void ThirdPersonCharacter::CreateCharacterView(const CharacterViewInfo* pCharacterViewInfo)
{
	SkinnedMeshRenderer* pSkinnedMeshRendererCharacter = m_hSkinnedMeshRendererCharacter.ToPtr();
	pSkinnedMeshRendererCharacter->SetMesh(pCharacterViewInfo->GetMesh());
	pSkinnedMeshRendererCharacter->SetArmature(pCharacterViewInfo->GetArmature());
	
	for (size_t i = 0; i < pCharacterViewInfo->GetMaterials().size(); ++i)
		pSkinnedMeshRendererCharacter->SetMaterial(i, pCharacterViewInfo->GetMaterials()[i]);
}
