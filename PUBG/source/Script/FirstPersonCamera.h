#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class FirstPersonCamera : public ze::IScript
	{
	public:
		FirstPersonCamera() = default;
		virtual ~FirstPersonCamera() = default;

		virtual void FixedUpdate() override;
		virtual void Update() override;
	};
}
