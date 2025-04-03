#include <ZergEngine\CoreSystem\EnvironmentManager.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(Environment);

Environment::Environment()
	: m_spSkybox()
{
}

Environment::~Environment()
{
}

void Environment::Init(void* pDesc)
{
}

void Environment::Release()
{
	m_spSkybox.reset();
}
