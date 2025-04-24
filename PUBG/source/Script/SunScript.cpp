#include "SunScript.h"

using namespace pubg;
using namespace ze;

void SunScript::FixedUpdate()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	ComponentHandle<Transform> hTransform;
	Transform* pTransform;

	hTransform = pGameObject->GetComponent<Transform>();
	pTransform = hTransform.ToPtr();

	pTransform->Rotate(XMQuaternionRotationAxis(WORLD_RIGHT, 0.005f));
}
