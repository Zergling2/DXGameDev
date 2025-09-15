#pragma once

#include <cstdlib>

namespace ze
{
	class RawVector
	{
	public:
		RawVector();
		~RawVector();
		inline size_t ByteSize() const { return m_byteSize; }
		void PushBack(void* ptr, size_t size);
		void Resize(size_t size);
		inline void* Data() const { return m_pMemBegin; }
	private:
		size_t m_byteSize;
		size_t m_capacity;
		char* m_pWritePtr;
		char* m_pMemBegin;
		char* m_pMemEnd;
	};
}
