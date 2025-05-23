#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class MovePointLight : public ze::MonoBehaviour
	{
	public:
		MovePointLight() = default;
		virtual ~MovePointLight() = default;

		virtual void Update() override;
	};
}
