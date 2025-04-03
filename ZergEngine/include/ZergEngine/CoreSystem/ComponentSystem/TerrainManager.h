#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class TerrainManager : public IComponentManager
	{
		friend class Runtime;
		friend class Terrain;
		ZE_DECLARE_SINGLETON(TerrainManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	};
}
