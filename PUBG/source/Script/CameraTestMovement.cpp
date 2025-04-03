#include "CameraTestMovement.h"

using namespace pubg;
using namespace ze;

void CameraTestMovement::FixedUpdate()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	XMFLOAT3A position = pGameObject->GetTransform().m_position;
	position.y -= 0.1f;
	position.z += 0.0f;

	pGameObject->GetTransform().m_position = position;
}
