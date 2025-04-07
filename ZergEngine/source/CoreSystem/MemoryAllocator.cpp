#include <ZergEngine\CoreSystem\MemoryAllocator.h>
#include <ZergEngine\CoreSystem\SystemInfo.h>
#include <ZergEngine\CoreSystem\Debug.h>

namespace ze
{
	MemoryAllocatorImpl MemoryAllocator;
}

using namespace ze;

MemoryAllocatorImpl::MemoryAllocatorImpl()
{
}

MemoryAllocatorImpl::~MemoryAllocatorImpl()
{
}

void MemoryAllocatorImpl::Init(void* pDesc)
{
}

void MemoryAllocatorImpl::Release()
{
}

LPVOID MemoryAllocatorImpl::RequestSystemAllocGranularitySize()
{
	LPVOID pages = VirtualAlloc(
		nullptr,
		SystemInfo.GetSystemInfo().dwAllocationGranularity,
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
	);

	return pages;
}
