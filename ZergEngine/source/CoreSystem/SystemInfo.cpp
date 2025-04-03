#include <ZergEngine\CoreSystem\SystemInfo.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(SystemInfo);

SystemInfo::SystemInfo()
    : m_si{}
{
}

SystemInfo::~SystemInfo()
{
}

void SystemInfo::Init(void* pDesc)
{
    ::GetSystemInfo(&m_si);
}

void SystemInfo::Release()
{
    // Nothing to do.
}
