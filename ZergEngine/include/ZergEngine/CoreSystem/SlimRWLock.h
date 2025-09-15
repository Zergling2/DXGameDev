#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class SlimRWLock
	{
	public:
		SlimRWLock()
#if defined(DEBUG) || defined(_DEBUG)
			: m_init(false)
			, m_lock{}
#else
			: m_lock{}
#endif
		{
		}
		~SlimRWLock() = default;

		void Init();

		void AcquireLockExclusive() { assert(m_init == true); AcquireSRWLockExclusive(&m_lock); }

		void ReleaseLockExclusive() { assert(m_init == true); ReleaseSRWLockExclusive(&m_lock); }

		void AcquireLockShared() { assert(m_init == true); AcquireSRWLockShared(&m_lock); }

		void ReleaseLockShared() { assert(m_init == true); ReleaseSRWLockShared(&m_lock); }

		// If the lock is successfully acquired, the return value is true.
		bool TryAcquireLockExclusive() { assert(m_init == true); return TryAcquireSRWLockExclusive(&m_lock) != 0; }

		// If the lock is successfully acquired, the return value is true.
		bool TryAcquireLockShared() { assert(m_init == true); return TryAcquireSRWLockShared(&m_lock) != 0; }

#if defined(DEBUG) || defined(_DEBUG)
		bool IsInitialized() const { return m_init; }
#endif
	private:
#if defined(DEBUG) || defined(_DEBUG)
		bool m_init;
#endif
		SRWLOCK m_lock;
	};

	// RAII
	class AutoAcquireSlimRWLockExclusive
	{
	public:
		AutoAcquireSlimRWLockExclusive(SlimRWLock& lock)
			: m_lock(lock)
		{
			assert(m_lock.IsInitialized());
			m_lock.AcquireLockExclusive();
		}
		~AutoAcquireSlimRWLockExclusive()
		{
			m_lock.ReleaseLockExclusive();
		}
	private:
		SlimRWLock& m_lock;
	};

	// RAII
	class AutoAcquireSlimRWLockShared
	{
	public:
		AutoAcquireSlimRWLockShared(SlimRWLock& lock)
			: m_lock(lock)
		{
			assert(m_lock.IsInitialized());
			m_lock.AcquireLockShared();
		}
		~AutoAcquireSlimRWLockShared()
		{
			m_lock.ReleaseLockShared();
		}
	private:
		SlimRWLock& m_lock;
	};
}
