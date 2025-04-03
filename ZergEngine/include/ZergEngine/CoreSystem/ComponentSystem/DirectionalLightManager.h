#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class DirectionalLightManager : public IComponentManager
	{
		friend class Runtime;
		friend class DirectionalLight;
		ZE_DECLARE_SINGLETON(DirectionalLightManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	};
}
