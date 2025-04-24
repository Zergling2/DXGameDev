#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class ScriptManagerImpl : public IComponentManager
	{
		friend class RuntimeImpl;
		friend class IScript;
		ZE_DECLARE_SINGLETON(ScriptManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void FixedUpdateScripts();
		void UpdateScripts();
		void LateUpdateScripts();

		virtual void RemoveDestroyedComponents() override;
	};

	extern ScriptManagerImpl ScriptManager;
}
