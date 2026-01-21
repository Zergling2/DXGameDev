#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class RigidbodyManager : public IComponentManager
	{
		friend class Runtime;
		friend class IRigidbody;
	public:
		static RigidbodyManager* GetInstance() { return s_pInstance; }
	private:
		RigidbodyManager() = default;
		virtual ~RigidbodyManager() = default;

		static void CreateInstance();
		static void DestroyInstance();

		virtual void RemoveDestroyedComponents() override;
	private:
		static RigidbodyManager* s_pInstance;
	};
}
