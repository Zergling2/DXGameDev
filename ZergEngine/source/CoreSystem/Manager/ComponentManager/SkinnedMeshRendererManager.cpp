#include <ZergEngine\CoreSystem\Manager\ComponentManager\SkinnedMeshRendererManager.h>

using namespace ze;

SkinnedMeshRendererManager* SkinnedMeshRendererManager::s_pInstance = nullptr;

void SkinnedMeshRendererManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new SkinnedMeshRendererManager();
}

void SkinnedMeshRendererManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}
