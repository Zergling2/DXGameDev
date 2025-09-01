#include "Planet.h"

using namespace ze;

void Planet::Update()
{
	auto pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	auto pTarget = m_hCenter.ToPtr();
	if (!pTarget)
		return;


}
