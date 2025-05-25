#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class MovePointLight : public ze::MonoBehaviour
	{
		DECLARE_MONOBEHAVIOUR_TYPE
	public:
		MovePointLight() = default;
		virtual ~MovePointLight() = default;

		virtual void Update() override;
	};
}
