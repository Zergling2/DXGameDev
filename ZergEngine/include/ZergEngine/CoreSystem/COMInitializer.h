#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class COMInitializerImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(COMInitializerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	private:
		bool m_init;
	};

	extern COMInitializerImpl COMInitializer;
}
