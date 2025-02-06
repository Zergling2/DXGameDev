#include <ZergEngine\System\RawVector.h>

using namespace zergengine;

constexpr size_t INITIAL_SIZE = 4096;

RawVector::RawVector()
	: m_byteSize(0)
	, m_capacity(INITIAL_SIZE)
{
	m_pMemBegin = reinterpret_cast<byte*>(_aligned_malloc(m_capacity, 16));
	m_pWritePtr = m_pMemBegin;
	m_pMemEnd = m_pMemBegin + m_capacity;
}

RawVector::~RawVector()
{
	if (m_pMemBegin)
		_aligned_free(m_pMemBegin);
}

void RawVector::push_back(void* ptr, size_t size)
{
	if (m_pWritePtr + size > m_pMemEnd)
	{
		resize(m_capacity * 2);
		push_back(ptr, size);
	}
	else
	{
		memcpy(m_pWritePtr, ptr, size);
		m_pWritePtr += size;
		m_byteSize += size;
	}
}

void RawVector::resize(size_t size)
{
	if (size == 0 || size == m_capacity)
		return;

	byte* pNewMem  = reinterpret_cast<byte*>(_aligned_malloc(size, 16));

	if (size >= m_byteSize)
		memcpy_s(pNewMem, size, m_pMemBegin, m_byteSize);
	else
	{
		memcpy_s(pNewMem, size, m_pMemBegin, size);
		m_byteSize = size;
	}

	_aligned_free(m_pMemBegin);

	m_capacity = size;
	m_pMemBegin = pNewMem;
	m_pMemEnd = m_pMemBegin + m_capacity;

	m_pWritePtr = m_pMemBegin + m_byteSize;
}
