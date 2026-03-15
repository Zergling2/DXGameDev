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
	// Update 루틴에서 카메라 최종 위치 결정된 상태로 가정
	// -> 카메라 행렬을 이용해서 캐릭터 위 닉네임에 대한 스크린 좌표 위치 획득 및 Text UI 이동
	// 코드 구현...


}
