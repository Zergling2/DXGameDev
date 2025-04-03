#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class CameraTestMovement : public ze::IScript
	{
	public:
		CameraTestMovement()
		{
		}
		virtual void FixedUpdate() override;
	private:
	};
}
