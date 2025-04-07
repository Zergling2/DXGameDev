#include <ZergEngine\CoreSystem\SystemInfo.h>

namespace ze
{
    SystemInfoImpl SystemInfo;
}

using namespace ze;

SystemInfoImpl::SystemInfoImpl()
    : m_si{}
{
}

SystemInfoImpl::~SystemInfoImpl()
{
}

void SystemInfoImpl::Init(void* pDesc)
{
    ::GetSystemInfo(&m_si);
}

void SystemInfoImpl::Release()
{
    // Nothing to do.
}
