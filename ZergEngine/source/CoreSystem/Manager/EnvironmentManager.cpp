#include <ZergEngine\CoreSystem\Manager\EnvironmentManager.h>

using namespace ze;

Environment* Environment::s_pInstance = nullptr;

Environment::Environment()
	: m_skyboxCubeMap()
{
}

Environment::~Environment()
{
}

void Environment::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new Environment();
}

void Environment::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void Environment::Init()
{
}

void Environment::UnInit()
{
	m_skyboxCubeMap.Reset();
}
