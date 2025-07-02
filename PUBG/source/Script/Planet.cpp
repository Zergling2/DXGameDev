#include "Planet.h"

using namespace ze;
using namespace pubg;

void Planet::Update()
{
	auto pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	auto pTarget = m_hCenter.ToPtr();
	if (!pTarget)
		return;

	constexpr float orbitDistance = 5.0f;

	XMVECTOR direction = XMVector3Normalize(pGameObject->m_transform.GetWorldPosition() - pTarget->m_transform.GetWorldPosition());
	pGameObject->m_transform.SetWorldPosition(pTarget->m_transform.GetWorldPosition() + direction * orbitDistance);

	pGameObject->m_transform.RotateAround(pTarget->m_transform.GetWorldPosition(), XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		XMConvertToRadians(45.0f * Time::GetInstance()->GetDeltaTime()));
}
