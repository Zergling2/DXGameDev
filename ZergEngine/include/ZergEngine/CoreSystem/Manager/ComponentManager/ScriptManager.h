#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class IScript;

	class ScriptManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class SceneManagerImpl;
		friend class IScript;
		ZE_DECLARE_SINGLETON(ScriptManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void AddToStartingQueue(IScript* pScript);
		void CallStart();

		void FixedUpdateScripts();
		void UpdateScripts();
		void LateUpdateScripts();

		virtual void RemoveDestroyedComponents() override;
	private:
		std::vector<IScript*> m_startingScripts;
	};

	extern ScriptManagerImpl ScriptManager;
}
