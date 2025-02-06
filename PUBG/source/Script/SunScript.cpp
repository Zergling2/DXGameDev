#include "SunScript.h"

using namespace pubg;

void SunScript::FixedUpdate()
{
	XMFLOAT4A rotation = this->GetGameObject()->GetTransform().m_rotation;
	rotation.x += 0.005f;
	this->GetGameObject()->GetTransform().m_rotation = rotation;
}
