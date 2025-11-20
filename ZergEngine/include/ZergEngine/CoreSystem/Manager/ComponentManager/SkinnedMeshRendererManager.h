#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class SkinnedMeshRendererManager : public IComponentManager
	{
		friend class Runtime;
		friend class Renderer;
		friend class SkinnedMeshRenderer;
	public:
		static SkinnedMeshRendererManager* GetInstance() { return s_pInstance; }
	private:
		SkinnedMeshRendererManager() = default;
		virtual ~SkinnedMeshRendererManager() = default;

		static void CreateInstance();
		static void DestroyInstance();
	private:
		static SkinnedMeshRendererManager* s_pInstance;
	};
}
