#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class CollisionTrigger;

	class CollisionTriggerManager : public IComponentManager
	{
		friend class Runtime;
		friend class CollisionTrigger;
	public:
		static CollisionTriggerManager* GetInstance() { return s_pInstance; }
	private:
		CollisionTriggerManager() = default;
		virtual ~CollisionTriggerManager() = default;

		static void CreateInstance();
		static void DestroyInstance();

		virtual void RemoveDestroyedComponents() override;

		void UpdateCollisionTriggerTransform() {}
	private:
		static CollisionTriggerManager* s_pInstance;
	};
}
