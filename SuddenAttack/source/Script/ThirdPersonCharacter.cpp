#include "ThirdPersonCharacter.h"

using namespace ze;

ThirdPersonCharacter::ThirdPersonCharacter(ze::GameObject& owner)
	: MonoBehaviour(owner)
{
}

void ThirdPersonCharacter::Update()
{
	const SkinnedMeshRenderer* pSkinnedMeshRendererThirdPersonCharacter = m_hSkinnedMeshRendererThirdPersonCharacter.ToPtr();
	if (!pSkinnedMeshRendererThirdPersonCharacter)
		return;

	XMFLOAT3A s;
	XMFLOAT4A r;
	XMFLOAT3A t;
	if (!pSkinnedMeshRendererThirdPersonCharacter->GetBoneTransform("Hand.R", s, r, t))
		return;

	GameObject* pGameObjectTVWeaponBase = m_hGameObjectTVWeaponBase.ToPtr();
	if (!pGameObjectTVWeaponBase)
		return;

	// pGameObjectTVWeaponBase->m_transform.SetScale(s);
	pGameObjectTVWeaponBase->m_transform.SetRotationQuaternion(r);
	pGameObjectTVWeaponBase->m_transform.SetPosition(t);
}

void ThirdPersonCharacter::LateUpdate()
{
	// Update 루틴에서 카메라 최종 위치 결정된 상태로 가정
	// -> 카메라 행렬을 이용해서 캐릭터 위 닉네임에 대한 스크린 좌표 위치 획득 및 Text UI 이동
	// 코드 구현...


}
