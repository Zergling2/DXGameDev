#include "SlimRWLock.h"

void SlimRWLock::Init()
{
	assert(m_init == false);

	InitializeSRWLock(&m_lock);

#if defined(DEBUG) || defined(_DEBUG)
	m_init = true;
#endif
}
