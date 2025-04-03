#include <ZergEngine\CoreSystem\MemoryAllocator.h>
#include <ZergEngine\CoreSystem\SystemInfo.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(MemoryAllocator);

MemoryAllocator::MemoryAllocator()
{
}

MemoryAllocator::~MemoryAllocator()
{
}

void MemoryAllocator::Init(void* pDesc)
{
}

void MemoryAllocator::Release()
{
}

LPVOID MemoryAllocator::RequestSystemAllocGranularitySize()
{
	LPVOID pages = VirtualAlloc(
		nullptr,
		SystemInfo::GetInstance().GetSystemInfo().dwAllocationGranularity,
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
	);

	return pages;
}
