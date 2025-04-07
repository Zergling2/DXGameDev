#include <ZergEngine\CoreSystem\EnvironmentManager.h>

namespace ze
{
	EnvironmentManagerImpl Environment;
}

using namespace ze;

EnvironmentManagerImpl::EnvironmentManagerImpl()
	: m_spSkybox()
{
}

EnvironmentManagerImpl::~EnvironmentManagerImpl()
{
}

void EnvironmentManagerImpl::Init(void* pDesc)
{
}

void EnvironmentManagerImpl::Release()
{
	m_spSkybox.reset();
}
