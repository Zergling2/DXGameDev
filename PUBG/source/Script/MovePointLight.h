#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class MovePointLight : public ze::IScript
	{
	public:
		MovePointLight() = default;
		virtual ~MovePointLight() = default;

		virtual void Update() override;
	};
}
