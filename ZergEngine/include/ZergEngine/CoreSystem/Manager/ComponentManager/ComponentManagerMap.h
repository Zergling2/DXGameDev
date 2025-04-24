#pragma once

#include <ZergEngine/CoreSystem/GamePlayBase/Component/ComponentType.h>

namespace ze
{
	class IComponentManager;

	class ComponentManagerMap
	{
	public:
		static IComponentManager* GetComponentManager(COMPONENT_TYPE type);
	};
}
