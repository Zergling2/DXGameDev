#pragma once

#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

namespace ze
{
	class ScriptManager : public IComponentManager
	{
		friend class Runtime;
		friend class IScript;
		ZE_DECLARE_SINGLETON(ScriptManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void FixedUpdateScripts();
		void UpdateScripts();
		void LateUpdateScripts();

		// 스크립트의 경우 지연 삭제를 한다.
		virtual void Unregister(IComponent* pComponent) override {}
	};
}
