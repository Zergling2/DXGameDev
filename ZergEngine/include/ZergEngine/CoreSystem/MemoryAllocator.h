#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class MemoryAllocator
	{
		friend class Runtime;
	public:
		static MemoryAllocator* GetInstance() { return s_pInstance; }
	private:
		MemoryAllocator() = default;
		~MemoryAllocator() = default;

		static void CreateInstance();
		static void DestroyInstance();

		void Init();
		void UnInit();
	public:
		LPVOID RequestSystemAllocPageSize(uint32_t page);
		LPVOID RequestSystemAllocGranularitySize();
		DWORD GetSystemAllocationGranularity() const { return m_si.dwAllocationGranularity; }
	private:
		static MemoryAllocator* s_pInstance;
		SYSTEM_INFO m_si;
	};
}
