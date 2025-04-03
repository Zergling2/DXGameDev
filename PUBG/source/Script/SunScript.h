#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class SunScript : public ze::IScript
	{
	public:
		SunScript() = default;
		virtual ~SunScript() = default;
		virtual void FixedUpdate() override;
	};
}
