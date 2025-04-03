#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class MemoryAllocator : public ISubsystem
	{
		friend class Runtime;
		ZE_DECLARE_SINGLETON(MemoryAllocator);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	public:
		LPVOID RequestSystemAllocPageSize(uint32_t page);
		LPVOID RequestSystemAllocGranularitySize();
	};
}
