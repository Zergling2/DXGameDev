#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

MeshRendererManager* MeshRendererManager::s_pInstance = nullptr;

void MeshRendererManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new MeshRendererManager();
}

void MeshRendererManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}
