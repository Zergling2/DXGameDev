#include <ZergEngine\System\GameObjectManager.h>
#include <ZergEngine\System\GameObject.h>

using namespace zergengine;

std::unordered_map<GameObject*, std::shared_ptr<GameObject>> GameObjectManager::s_gameObjects;

void GameObjectManager::AddGameObject(const std::shared_ptr<GameObject>& go)
{
	GameObjectManager::s_gameObjects.emplace(go.get(), go);
}

void GameObjectManager::RemoveGameObject(const GameObject* key)
{
	assert(key->IsDestroyed() == true);
	GameObjectManager::s_gameObjects.erase(const_cast<GameObject*>(key));
}
