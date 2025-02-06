#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class RawVector
	{
	public:
		RawVector();
		~RawVector();
		inline size_t bytesize() const { return m_byteSize; }
		void push_back(void* ptr, size_t size);
		void resize(size_t size);
		inline void* data() const { return m_pMemBegin; }
	private:
		size_t m_byteSize;
		size_t m_capacity;
		byte* m_pWritePtr;
		byte* m_pMemBegin;
		byte* m_pMemEnd;
	};
}
