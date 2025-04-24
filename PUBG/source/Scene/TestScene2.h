#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class TestScene2 : public ze::IScene
	{
		ZE_DECLARE_SCENE(TestScene2);
	public:
		virtual void OnLoadScene() override;
	};
}
