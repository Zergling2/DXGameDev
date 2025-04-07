#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class TerrainManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class Terrain;
		ZE_DECLARE_SINGLETON(TerrainManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	};

	extern TerrainManagerImpl TerrainManager;
}
