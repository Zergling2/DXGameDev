#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentType.h>

namespace ze
{
	class IComponentManager;

	class ComponentManagerMap
	{
	public:
		static IComponentManager* GetManager(COMPONENT_TYPE type);
	};
}
