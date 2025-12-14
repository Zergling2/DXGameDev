#include <ZergEngine\CoreSystem\Manager\ComponentManager\BillboardManager.h>

using namespace ze;

BillboardManager* BillboardManager::s_pInstance = nullptr;

void BillboardManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new BillboardManager();
}

void BillboardManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}
