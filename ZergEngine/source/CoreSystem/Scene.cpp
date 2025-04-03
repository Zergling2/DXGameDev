#include <ZergEngine\CoreSystem\Scene.h>
#include <ZergEngine\CoreSystem\GameObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

IScene::IScene()
	: m_pDeferredGameObjects()
{
}

GameObject* IScene::CreateGameObject(PCWSTR name)
{
	GameObject* pGameObject = new(std::nothrow) GameObject(true, name);

	if (pGameObject)
		m_pDeferredGameObjects.push_back(pGameObject);

	return pGameObject;
}
