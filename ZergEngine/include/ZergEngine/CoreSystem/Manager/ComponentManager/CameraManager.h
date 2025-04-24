#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class CameraManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class WindowImpl;
		friend class SceneManagerImpl;
		friend class Camera;
		ZE_DECLARE_SINGLETON(CameraManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void Update();
		void OnResize();

		virtual ComponentHandleBase Register(IComponent* pComponent) override;
		virtual void RemoveDestroyedComponents() override;
	};

	extern CameraManagerImpl CameraManager;
}
