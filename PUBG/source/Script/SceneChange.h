#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class SceneChange : public ze::MonoBehaviour
	{
	public:
		SceneChange() = default;
		virtual ~SceneChange() = default;

		virtual void Update() override;
	};
}
