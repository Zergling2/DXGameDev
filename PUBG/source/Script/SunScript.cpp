#include "SunScript.h"

using namespace pubg;
using namespace ze;

void SunScript::FixedUpdate()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	XMFLOAT4A rotation = pGameObject->GetTransform().m_rotation;
	rotation.x += 0.005f;
	pGameObject->GetTransform().m_rotation = rotation;
}
