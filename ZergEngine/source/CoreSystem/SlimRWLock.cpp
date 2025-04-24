#include <ZergEngine\CoreSystem\SlimRWLock.h>

using namespace ze;

void SlimRWLock::Init()
{
	assert(m_init == false);

	InitializeSRWLock(&m_lock);

#if defined(DEBUG) || defined(_DEBUG)
	m_init = true;
#endif
}
