#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class SystemInfoImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(SystemInfoImpl);
	public:
		inline const SYSTEM_INFO& GetSystemInfo() { return m_si; }
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	private:
		SYSTEM_INFO m_si;
	};

	extern SystemInfoImpl SystemInfo;
}
