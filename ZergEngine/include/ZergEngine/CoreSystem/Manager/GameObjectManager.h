#pragma once

#include <ZergEngine\CoreSystem\SlimRWLock.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <vector>

namespace ze
{
	class GameObject;
	class Transform;

	class GameObjectManager
	{
		friend class Runtime;
		friend class IScene;
		friend class GameObject;
		friend class GameObjectHandle;
		friend class Transform;
	public:
		static GameObjectManager* GetInstance() { return s_pInstance; }
	private:
		GameObjectManager();
		~GameObjectManager();

		static void CreateInstance();
		static void DestroyInstance();

		void Init();
		void UnInit();

		void Deploy(GameObject* pGameObject);

		GameObjectHandle FindGameObject(PCWSTR name);
		GameObjectHandle CreateObject(PCWSTR name);
		GameObjectHandle CreatePendingObject(GameObject** ppNewGameObject, PCWSTR name);
		void RequestDestroy(GameObject* pGameObject);

		GameObject* ToPtr(uint32_t tableIndex, uint64_t id) const;

		// 비동기 씬 로딩 스레드에 의해서 호출될 수 있음.
		GameObjectHandle RegisterToHandleTable(GameObject* pGameObject);
		void AddToDestroyQueue(GameObject* pGameObject);

		// 비-지연 오브젝트들을 옮기는 함수
		void MoveToActiveGroup(GameObject* pGameObject);
		// 비-지연 오브젝트들을 옮기는 함수
		void MoveToInactiveGroup(GameObject* pGameObject);

		void AddToActiveGroup(GameObject* pGameObject);
		void AddToInactiveGroup(GameObject* pGameObject);

		void RemoveFromGroup(GameObject* pGameObject);	// Active/Inactive group에서 포인터 제거
		void RemoveDestroyedGameObjects();

		void SetActive(GameObject* pGameObject, bool active);

		// pTransform은 항상 nullptr이 아닌 입력
		// pNewTransform은 nullptr이 입력으로 들어올 수 있음.
		bool SetParent(Transform* pTransform, Transform* pNewParentTransform);

		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	private:
		static GameObjectManager* s_pInstance;

		uint64_t m_uniqueId;
		SlimRWLock m_lock;
		std::vector<GameObject*> m_activeGroup;		// 검색 시 전체 테이블을 순회할 필요 제거
		std::vector<GameObject*> m_inactiveGroup;	// 비활성화된 게임 오브젝트들 (검색 대상에서 제외)
		std::vector<uint32_t> m_emptyHandleTableIndex;
		std::vector<GameObject*> m_handleTable;
		std::vector<GameObject*> m_destroyed;
	};
}
