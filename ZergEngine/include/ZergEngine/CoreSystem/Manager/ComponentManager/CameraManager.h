#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class CameraManager : public IComponentManager
	{
		friend class Runtime;
		friend class Renderer;
		friend class Camera;
	public:
		static CameraManager* GetInstance() { return s_pInstance; }
	private:
		CameraManager() = default;
		virtual ~CameraManager() = default;

		static void CreateInstance();
		static void DestroyInstance();

		virtual void AddToDirectAccessGroup(IComponent* pComponent) override;
		virtual void RemoveDestroyedComponents() override;

		void Update();
		bool ResizeBuffer(uint32_t width, uint32_t height);
	private:
		static CameraManager* s_pInstance;
	};
}
