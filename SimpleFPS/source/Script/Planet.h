#pragma once

#include <ZergEngine\ZergEngine.h>

class Planet : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	Planet()
		: m_orbitDistance(10.0f)
		, m_hCenter()
	{
	}
	virtual ~Planet() = default;

	virtual void Update() override;
public:
	float m_orbitDistance;
	ze::GameObjectHandle m_hCenter;
};
