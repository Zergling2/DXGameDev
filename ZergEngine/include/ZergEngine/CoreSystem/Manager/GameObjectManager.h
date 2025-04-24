#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class GameObject;

	class GameObjectManagerImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		friend class GameObject;
		friend class GameObjectHandle;
		friend class SceneManagerImpl;
		ZE_DECLARE_SINGLETON(GameObjectManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void AddToDestroyQueue(GameObject* pGameObject);

		GameObjectHandle FindGameObject(PCWSTR name);

		GameObjectHandle Register(GameObject* pGameObject);
		void RemoveDestroyedGameObjects();

		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	private:
		uint64_t m_uniqueId;
		std::vector<GameObject*> m_destroyed;
		std::vector<GameObject*> m_activeGameObjects;	// 검색 시 전체 테이블을 순회할 필요를 없앤다.
		std::vector<GameObject*> m_table;
	};

	extern GameObjectManagerImpl GameObjectManager;
}
