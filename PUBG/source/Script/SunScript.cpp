#include "SunScript.h"

using namespace pubg;
using namespace ze;

void SunScript::FixedUpdate()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	pGameObject->m_transform.Rotate(XMQuaternionRotationAxis(Math::Vector3::Right(), 0.005f));
}
