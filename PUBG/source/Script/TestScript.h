#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class TestScript : public ze::MonoBehaviour
	{
		DECLARE_MONOBEHAVIOUR_TYPE
	public:
		TestScript() = default;
		virtual ~TestScript() = default;

		virtual void FixedUpdate() override;
	};
}
