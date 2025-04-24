#include <ZergEngine\CoreSystem\Scene.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

IScene::IScene()
	: m_pDeferredGameObjects(new std::vector<GameObject*>())
{
}

GameObject* IScene::CreateGameObject(PCWSTR name)
{
	GameObject* pGameObject = new(std::nothrow) GameObject(
		static_cast<GAMEOBJECT_FLAG>(GOF_DEFERRED | GOF_ACTIVE),
		name
	);

	if (pGameObject)
		m_pDeferredGameObjects->push_back(pGameObject);

	return pGameObject;
}
