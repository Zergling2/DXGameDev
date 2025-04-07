#pragma once

#include <ZergEngine\Common\EngineHelper.h>

namespace ze
{
	class SyncFileLogger;

	class ISubsystem abstract
	{
		friend class RuntimeImpl;
	public:
		ISubsystem();
		virtual ~ISubsystem() = default;
	private:
		virtual void Init(void* pDesc) = 0;
		virtual void Release() = 0;
	};
}
