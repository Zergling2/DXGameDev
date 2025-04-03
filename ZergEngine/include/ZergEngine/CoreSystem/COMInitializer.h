#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class COMInitializer : public ISubsystem
	{
		friend class Runtime;
		ZE_DECLARE_SINGLETON(COMInitializer);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	private:
		bool m_init;
	};
}
