#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class BillboardManager : public IComponentManager
	{
		friend class Runtime;
		friend class Renderer;
		friend class Billboard;
	public:
		static BillboardManager* GetInstance() { return s_pInstance; }
	private:
		BillboardManager() = default;
		virtual ~BillboardManager() = default;

		static void CreateInstance();
		static void DestroyInstance();
	private:
		static BillboardManager* s_pInstance;
	};
}
