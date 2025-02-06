#include "CameraTestMovement.h"

using namespace pubg;

void CameraTestMovement::FixedUpdate()
{
	XMFLOAT3A position = this->GetGameObject()->GetTransform().m_position;
	position.y -= 0.1f;
	position.z += 0.0f;

	this->GetGameObject()->GetTransform().m_position = position;
}
