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

		void MoveToInactiveVectorFromActiveVector(GameObject* pGameObject);
		void MoveToActiveVectorFromInactiveVector(GameObject* pGameObject);

		void AddPtrToActiveVector(GameObject* pGameObject) { AddPtrToVector(m_activeGameObjects, pGameObject); }
		void AddPtrToInactiveVector(GameObject* pGameObject) { AddPtrToVector(m_inactiveGameObjects, pGameObject); }
		static void AddPtrToVector(std::vector<GameObject*>& vector, GameObject* pGameObject);
		void RemovePtrFromActiveVector(GameObject* pGameObject) { RemovePtrFromVector(m_activeGameObjects, pGameObject); }
		void RemovePtrFromInactiveVector(GameObject* pGameObject) { RemovePtrFromVector(m_inactiveGameObjects, pGameObject); }
		static void RemovePtrFromVector(std::vector<GameObject*>& vector, GameObject* pGameObject);

		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	private:
		uint64_t m_uniqueId;
		std::vector<GameObject*> m_destroyed;
		std::vector<GameObject*> m_activeGameObjects;	// 검색 시 전체 테이블을 순회할 필요 제거
		std::vector<GameObject*> m_inactiveGameObjects;	// 비활성화된 게임 오브젝트들 (검색 대상에서 제외)
		std::vector<GameObject*> m_table;
	};

	extern GameObjectManagerImpl GameObjectManager;
}
