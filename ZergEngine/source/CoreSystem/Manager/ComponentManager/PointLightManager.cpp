#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>

using namespace ze;

PointLightManager* PointLightManager::s_pInstance = nullptr;

void PointLightManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new PointLightManager();
}

void PointLightManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}