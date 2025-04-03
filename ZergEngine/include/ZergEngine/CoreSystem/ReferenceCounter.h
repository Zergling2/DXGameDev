#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
    // MT Unsafe version
	class ReferenceCounter
	{
        template<typename T>
        friend class t_ptr;
	public:
		inline ReferenceCounter()
			: m_rc(1)
		{
		}
        virtual ~ReferenceCounter() = 0;
    private:
        inline void AddRef() { ++m_rc; }
        inline void Release() { if (--m_rc == 0) delete this; }
	private:
		size_t m_rc;
	};

	template<typename _T>
	class t_ptr
	{
	public:
        inline explicit t_ptr(_T* ptr = nullptr) noexcept
            : m_ptr(ptr)
        {
        }

        inline t_ptr(const t_ptr& other) noexcept
            : m_ptr(other.m_ptr)
        {
            if (m_ptr)
                static_cast<ReferenceCounter*>(m_ptr)->AddRef();
        }

        inline t_ptr(t_ptr&& other) noexcept
            : m_ptr(other.m_ptr)
        {
            other.m_ptr = nullptr;
        }

        inline ~t_ptr()
        {
            if (m_ptr)
                static_cast<ReferenceCounter*>(m_ptr)->Release();
        }

        t_ptr& operator=(const t_ptr& other) noexcept
        {
            if (this != &other)
            {
                if (m_ptr)
                    static_cast<ReferenceCounter*>(m_ptr)->Release();

                m_ptr = other.m_ptr;
                if (m_ptr)
                    static_cast<ReferenceCounter*>(m_ptr)->AddRef();
            }
            return *this;
        }

        t_ptr& operator=(t_ptr&& other) noexcept
        {
            if (this != &other)
            {
                if (m_ptr)
                    static_cast<ReferenceCounter*>(m_ptr)->Release();

                m_ptr = other.m_ptr;
                other.m_ptr = nullptr;
            }
            return *this;
        }

        inline _T* operator->() const
        {
            return m_ptr;
        }

        inline _T* get() const
        {
            return m_ptr;
        }

        inline explicit operator bool() const
        {
            return m_ptr != nullptr;
        }
	private:
        _T* m_ptr;
	};

    template<class T>
    bool operator==(const t_ptr<T>& a, decltype(__nullptr)) throw()
    {
        return a.get() == nullptr;
    }

    template<class T>
    bool operator==(decltype(__nullptr), const t_ptr<T>& a) throw()
    {
        return a.get() == nullptr;
    }
}
