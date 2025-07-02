#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class DirectionalLightManager : public IComponentManager
	{
		friend class Runtime;
		friend class Renderer;
		friend class DirectionalLight;
	public:
		static DirectionalLightManager* GetInstance() { return s_pInstance; }
	private:
		DirectionalLightManager() = default;
		virtual ~DirectionalLightManager() = default;

		static void CreateInstance();
		static void DestroyInstance();
	private:
		static DirectionalLightManager* s_pInstance;
	};
}
