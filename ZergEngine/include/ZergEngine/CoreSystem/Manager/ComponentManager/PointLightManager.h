#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class PointLightManager : public IComponentManager
	{
		friend class Runtime;
		friend class Renderer;
		friend class PointLight;
	public:
		static PointLightManager* GetInstance() { return s_pInstance; }
	private:
		PointLightManager() = default;
		virtual ~PointLightManager() = default;

		static void CreateInstance();
		static void DestroyInstance();
	private:
		static PointLightManager* s_pInstance;
	};
}
