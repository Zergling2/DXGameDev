#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class SceneChange : public ze::MonoBehaviour
	{
		DECLARE_MONOBEHAVIOUR_TYPE
	public:
		SceneChange() = default;
		virtual ~SceneChange() = default;

		virtual void Update() override;
	};
}
