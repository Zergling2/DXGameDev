#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>

using namespace ze;

TerrainManager* TerrainManager::s_pInstance = nullptr;

void TerrainManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new TerrainManager();
}

void TerrainManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}
