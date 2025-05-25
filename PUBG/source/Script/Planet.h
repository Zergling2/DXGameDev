#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class Planet : public ze::MonoBehaviour
	{
		DECLARE_MONOBEHAVIOUR_TYPE
	public:
		Planet()
			: m_hCenter()
		{
		}
		virtual ~Planet() = default;

		virtual void Update() override;
	public:
		ze::GameObjectHandle m_hCenter;
	};
}
