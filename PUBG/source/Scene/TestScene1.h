#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class TestScene1 : public ze::IScene
	{
		ZE_DECLARE_SCENE(TestScene1);
	public:
		virtual void OnLoadScene() override;
	};
}
