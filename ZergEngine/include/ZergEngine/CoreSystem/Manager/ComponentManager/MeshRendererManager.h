#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class MeshRendererManager : public IComponentManager
	{
		friend class Runtime;
		friend class Renderer;
		friend class MeshRenderer;
	public:
		static MeshRendererManager* GetInstance() { return s_pInstance; }
	private:
		MeshRendererManager() = default;
		virtual ~MeshRendererManager() = default;

		static void CreateInstance();
		static void DestroyInstance();
	private:
		static MeshRendererManager* s_pInstance;
	};
}
