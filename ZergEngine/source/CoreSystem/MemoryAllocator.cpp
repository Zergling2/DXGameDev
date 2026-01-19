#include <ZergEngine\CoreSystem\MemoryAllocator.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

MemoryAllocator* MemoryAllocator::s_pInstance = nullptr;

void MemoryAllocator::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new MemoryAllocator();
}

void MemoryAllocator::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void MemoryAllocator::Init()
{
	::GetSystemInfo(&m_si);
}

void MemoryAllocator::Shutdown()
{
}

LPVOID MemoryAllocator::RequestSystemAllocGranularitySize()
{
	LPVOID pages = VirtualAlloc(
		nullptr,
		m_si.dwAllocationGranularity,
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
	);

	return pages;
}
