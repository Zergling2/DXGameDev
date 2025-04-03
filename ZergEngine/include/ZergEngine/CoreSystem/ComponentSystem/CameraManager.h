#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class CameraManager : public IComponentManager
	{
		friend class Runtime;
		friend class Window;
		friend class Camera;
		ZE_DECLARE_SINGLETON(CameraManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void Update();
		void OnResize();

		virtual ComponentHandle Register(IComponent* pComponent) override;
		virtual void Unregister(IComponent* pComponent) override;
	};
}
