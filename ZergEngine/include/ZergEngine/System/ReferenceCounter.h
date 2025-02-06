#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
    // MT unsafe version
	class ReferenceCounter
	{
        template<typename T>
        friend class t_ptr;
	public:
		inline ReferenceCounter()
			: m_refCnt(1)
		{
		}
        virtual ~ReferenceCounter() = 0;
    private:
        inline void addRef() const { ++m_refCnt; }
        inline void release() const { if (--m_refCnt == 0) delete this; }
	private:
		mutable uint32_t m_refCnt;
	};

	template<typename T>
	class t_ptr
	{
	public:
        inline explicit t_ptr(T* ptr = nullptr) noexcept
            : m_ptr(ptr)
        {
        }

        inline t_ptr(const t_ptr& other) noexcept
            : m_ptr(other.m_ptr)
        {
            if (m_ptr)
                static_cast<ReferenceCounter*>(m_ptr)->addRef();
        }

        inline t_ptr(t_ptr&& other) noexcept
            : m_ptr(other.m_ptr)
        {
            other.m_ptr = nullptr;
        }

        inline ~t_ptr()
        {
            if (m_ptr)
                static_cast<ReferenceCounter*>(m_ptr)->release();
        }

        t_ptr& operator=(const t_ptr& other) noexcept
        {
            if (this != &other)
            {
                if (m_ptr)
                    static_cast<ReferenceCounter*>(m_ptr)->release();

                m_ptr = other.m_ptr;
                if (m_ptr)
                    static_cast<ReferenceCounter*>(m_ptr)->addRef();
            }
            return *this;
        }

        t_ptr& operator=(t_ptr&& other) noexcept
        {
            if (this != &other)
            {
                if (m_ptr)
                    static_cast<ReferenceCounter*>(m_ptr)->release();

                m_ptr = other.m_ptr;
                other.m_ptr = nullptr;
            }
            return *this;
        }

        inline T* operator->() const
        {
            return m_ptr;
        }

        inline T* get() const
        {
            return m_ptr;
        }

        inline explicit operator bool() const
        {
            return m_ptr != nullptr;
        }
	private:
		T* m_ptr;
	};
}
