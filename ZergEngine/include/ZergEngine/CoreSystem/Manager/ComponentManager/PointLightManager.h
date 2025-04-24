#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class PointLightManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class PointLight;
		ZE_DECLARE_SINGLETON(PointLightManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	};

	extern PointLightManagerImpl PointLightManager;
}
