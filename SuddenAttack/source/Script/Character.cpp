#include "Character.h"

using namespace ze;

Character::Character(ze::GameObject& owner)
	: MonoBehaviour(owner)
{
}

void Character::Update()
{
	const SkinnedMeshRenderer* pSkinnedMeshRenderer = m_hSkinnedMeshRenderer.ToPtr();

	if (!pSkinnedMeshRenderer)
		return;

	XMFLOAT3A s;
	XMFLOAT4A r;
	XMFLOAT3A t;
	if (!pSkinnedMeshRenderer->GetBoneTransform("Hand.R", s, r, t))
		return;

	GameObject* pTvWeaponBase = m_hTvWeaponBase.ToPtr();
	if (!pTvWeaponBase)
		return;

	// pTvWeaponBase->m_transform.SetScale(s);
	pTvWeaponBase->m_transform.SetRotationQuaternion(r);
	pTvWeaponBase->m_transform.SetPosition(t);
}

void Character::LateUpdate()
{
	// 캐릭터 위 닉네임 표시 코드 테스트
	// ...
}
