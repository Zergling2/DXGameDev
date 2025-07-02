#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class SpotLightManager : public IComponentManager
	{
		friend class Runtime;
		friend class Renderer;
		friend class SpotLight;
	public:
		static SpotLightManager* GetInstance() { return s_pInstance; }
	private:
		SpotLightManager() = default;
		virtual ~SpotLightManager() = default;

		static void CreateInstance();
		static void DestroyInstance();
	private:
		static SpotLightManager* s_pInstance;
	};
}
