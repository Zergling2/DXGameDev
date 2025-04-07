#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class DirectionalLightManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class DirectionalLight;
		ZE_DECLARE_SINGLETON(DirectionalLightManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	};

	extern DirectionalLightManagerImpl DirectionalLightManager;
}
