#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>

using namespace ze;

SpotLightManager* SpotLightManager::s_pInstance = nullptr;

void SpotLightManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new SpotLightManager();
}

void SpotLightManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}
