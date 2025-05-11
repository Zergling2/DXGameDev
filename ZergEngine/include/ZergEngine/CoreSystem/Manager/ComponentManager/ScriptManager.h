#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class MonoBehaviour;

	class ScriptManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class SceneManagerImpl;
		friend class MonoBehaviour;
		ZE_DECLARE_SINGLETON(ScriptManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void AddToStartingQueue(MonoBehaviour* pScript);
		void CallStart();

		void FixedUpdateScripts();
		void UpdateScripts();
		void LateUpdateScripts();

		virtual void RemoveDestroyedComponents() override;
	private:
		std::vector<MonoBehaviour*> m_startingScripts;
	};

	extern ScriptManagerImpl ScriptManager;
}
