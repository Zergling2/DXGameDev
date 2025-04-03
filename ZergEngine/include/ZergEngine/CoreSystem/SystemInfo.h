#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class SystemInfo : public ISubsystem
	{
		friend class Runtime;
		ZE_DECLARE_SINGLETON(SystemInfo);
	public:
		inline const SYSTEM_INFO& GetSystemInfo() { return m_si; }
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	private:
		SYSTEM_INFO m_si;
	};
}
