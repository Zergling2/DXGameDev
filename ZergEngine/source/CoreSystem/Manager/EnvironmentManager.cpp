#include <ZergEngine\CoreSystem\Manager\EnvironmentManager.h>

namespace ze
{
	EnvironmentManagerImpl Environment;
}

using namespace ze;

EnvironmentManagerImpl::EnvironmentManagerImpl()
	: m_skyboxCubeMap()
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
	m_skyboxCubeMap.Reset();
}
