#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class MemoryAllocatorImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(MemoryAllocatorImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	public:
		LPVOID RequestSystemAllocPageSize(uint32_t page);
		LPVOID RequestSystemAllocGranularitySize();
	};

	extern MemoryAllocatorImpl MemoryAllocator;
}
