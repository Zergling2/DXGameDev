#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class SlimRWLock
	{
	public:
		SlimRWLock();

		void Init();

		inline void AcquireLockExclusive() { assert(m_init == true); AcquireSRWLockExclusive(&m_lock); }

		inline void ReleaseLockExclusive() { assert(m_init == true); ReleaseSRWLockExclusive(&m_lock); }

		inline void AcquireLockShared() { assert(m_init == true); AcquireSRWLockShared(&m_lock); }

		inline void ReleaseLockShared() { assert(m_init == true); ReleaseSRWLockShared(&m_lock); }

		// If the lock is successfully acquired, the return value is true.
		inline bool TryAcquireLockExclusive() { assert(m_init == true); return TryAcquireSRWLockExclusive(&m_lock) != 0; }

		// If the lock is successfully acquired, the return value is true.
		inline bool TryAcquireLockShared() { assert(m_init == true); return TryAcquireSRWLockShared(&m_lock) != 0; }
	private:
		bool m_init;
		SRWLOCK m_lock;
	};
}
