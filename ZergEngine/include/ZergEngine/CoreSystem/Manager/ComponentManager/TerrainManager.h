#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class TerrainManager : public IComponentManager
	{
		friend class Runtime;
		friend class Renderer;
		friend class Terrain;
	public:
		static TerrainManager* GetInstance() { return s_pInstance; }
	private:
		TerrainManager() = default;
		virtual ~TerrainManager() = default;

		static void CreateInstance();
		static void DestroyInstance();
	private:
		static TerrainManager* s_pInstance;
	};
}
