#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class SpotLightManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class SpotLight;
		ZE_DECLARE_SINGLETON(SpotLightManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	};

	extern SpotLightManagerImpl SpotLightManager;
}
