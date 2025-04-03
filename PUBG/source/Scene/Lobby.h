#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class Lobby : public ze::IScene
	{
		ZE_DECLARE_SCENE(Lobby);
	public:
		virtual void OnLoadScene() override;
	};
}
