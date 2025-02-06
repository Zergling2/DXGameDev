#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class GameObject;

	// GameObject 검색 등을 지원
	class GameObjectManager
	{
		friend void Destroy(std::shared_ptr<GameObject>& gameObject);
		friend class SceneManager;
		friend class IScene;
	private:
		GameObjectManager();
		static void AddGameObject(const std::shared_ptr<GameObject>& go);
		static void RemoveGameObject(const GameObject* key);
	private:
		static std::unordered_map<GameObject*, std::shared_ptr<GameObject>> s_gameObjects;
	};
}
