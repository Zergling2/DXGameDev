#pragma once

#include <ZergEngine\ZergEngine.h>

class FirstPersonMovement : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	FirstPersonMovement()
	{
	}
	virtual ~FirstPersonMovement() = default;

	virtual void Update() override;
};
