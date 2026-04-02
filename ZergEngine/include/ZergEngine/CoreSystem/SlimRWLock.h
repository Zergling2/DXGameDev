#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class SlimRWLock
{
public:
	SlimRWLock()
	{
		InitializeSRWLock(&m_lock);
	}
	~SlimRWLock() = default;

	void AcquireLockExclusive() { AcquireSRWLockExclusive(&m_lock); }

	void ReleaseLockExclusive() { ReleaseSRWLockExclusive(&m_lock); }

	void AcquireLockShared() { AcquireSRWLockShared(&m_lock); }

	void ReleaseLockShared() { ReleaseSRWLockShared(&m_lock); }

	// If the lock is successfully acquired, the return value is true.
	bool TryAcquireLockExclusive() { return TryAcquireSRWLockExclusive(&m_lock) != 0; }

	// If the lock is successfully acquired, the return value is true.
	bool TryAcquireLockShared() { return TryAcquireSRWLockShared(&m_lock) != 0; }
private:
	SRWLOCK m_lock;
};

// RAII
class AutoAcquireSlimRWLockExclusive
{
public:
	AutoAcquireSlimRWLockExclusive(SlimRWLock& lock)
		: m_lock(lock)
	{
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
		m_lock.AcquireLockShared();
	}
	~AutoAcquireSlimRWLockShared()
	{
		m_lock.ReleaseLockShared();
	}
private:
	SlimRWLock& m_lock;
};
