#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class SpotLightManager : public IComponentManager
	{
		friend class Runtime;
		friend class SpotLight;
		ZE_DECLARE_SINGLETON(SpotLightManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	};
}
