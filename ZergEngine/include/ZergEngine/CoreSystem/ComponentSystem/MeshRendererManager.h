#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class MeshRendererManager : public IComponentManager
	{
		friend class Runtime;
		friend class MeshRenderer;
		ZE_DECLARE_SINGLETON(MeshRendererManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	};
}
