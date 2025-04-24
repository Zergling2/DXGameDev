#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class SpaceTestScene : public ze::IScene
	{
		ZE_DECLARE_SCENE(SpaceTestScene);
	public:
		virtual void OnLoadScene() override;
	};
}
