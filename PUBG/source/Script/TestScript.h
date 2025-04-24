#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class TestScript : public ze::IScript
	{
	public:
		TestScript() = default;
		virtual ~TestScript() = default;

		virtual void FixedUpdate() override;
	};
}
