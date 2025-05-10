#include <ZergEngine\CoreSystem\SceneInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>

using namespace ze;

IScene::IScene()
	: m_pDeferredGameObjects(new(std::nothrow) std::vector<GameObject*>())
{
	assert(m_pDeferredGameObjects != nullptr);
}

GameObjectHandle IScene::CreateGameObject(PCWSTR name)
{
	// Not deferred game object.
	GameObject* pGameObject = new GameObject(GOF_DEFERRED | GOF_ACTIVE, name);

	GameObjectHandle hGameObject = GameObjectManager.RegisterToHandleTable(pGameObject);
	m_pDeferredGameObjects->push_back(pGameObject);

	return hGameObject;
}
