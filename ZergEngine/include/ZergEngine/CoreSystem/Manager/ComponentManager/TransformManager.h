#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class TransformManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class Transform;
		ZE_DECLARE_SINGLETON(TransformManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		virtual void RemoveDestroyedComponents() override;
	};

	extern TransformManagerImpl TransformManager;
}
