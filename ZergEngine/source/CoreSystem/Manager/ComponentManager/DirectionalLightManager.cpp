#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>

using namespace ze;

DirectionalLightManager* DirectionalLightManager::s_pInstance = nullptr;

void DirectionalLightManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new DirectionalLightManager();
}

void DirectionalLightManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}
