#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class CharacterControllerManager : public IComponentManager
	{
		friend class Runtime;
		friend class CharacterController;
	public:
		static CharacterControllerManager* GetInstance() { return s_pInstance; }
	private:
		CharacterControllerManager() = default;
		virtual ~CharacterControllerManager() = default;

		static void CreateInstance();
		static void DestroyInstance();

		void SyncTransformFromPhysicsEngine();

		virtual void RemoveDestroyedComponents() override;
	private:
		static CharacterControllerManager* s_pInstance;
	};
}
