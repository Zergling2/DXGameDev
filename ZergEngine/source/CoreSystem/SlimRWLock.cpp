#include <ZergEngine\CoreSystem\SlimRWLock.h>

using namespace ze;

SlimRWLock::SlimRWLock()
	: m_init(false)
{
}

void SlimRWLock::Init()
{
	if (!m_init)
	{
		InitializeSRWLock(&m_lock);
		m_init = true;
	}
}

