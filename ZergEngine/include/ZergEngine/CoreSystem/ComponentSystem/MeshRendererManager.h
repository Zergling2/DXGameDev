#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class MeshRendererManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class MeshRenderer;
		ZE_DECLARE_SINGLETON(MeshRendererManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	};

	extern MeshRendererManagerImpl MeshRendererManager;
}
