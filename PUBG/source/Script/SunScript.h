#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class SunScript : public zergengine::IScript
	{
	public:
		SunScript()
		{
		}
		virtual void FixedUpdate() override;
	};
}
