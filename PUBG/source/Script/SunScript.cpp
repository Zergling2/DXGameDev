#include "SunScript.h"

using namespace pubg;
using namespace ze;

void SunScript::FixedUpdate()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	pGameObject->m_transform.Rotate(XMQuaternionRotationAxis(WORLD_RIGHT, 0.005f));
}
