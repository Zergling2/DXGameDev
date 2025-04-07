#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class CameraManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class WindowImpl;
		friend class Camera;
		ZE_DECLARE_SINGLETON(CameraManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void Update();
		void OnResize();

		virtual ComponentHandle Register(IComponent* pComponent) override;
		virtual void Unregister(IComponent* pComponent) override;
	};

	extern CameraManagerImpl CameraManager;
}
