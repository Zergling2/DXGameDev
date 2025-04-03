#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class PointLightManager : public IComponentManager
	{
		friend class Runtime;
		friend class PointLight;
		ZE_DECLARE_SINGLETON(PointLightManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	};
}
