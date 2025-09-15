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

		// �񵿱� �� �ε� �����忡 ���ؼ� ȣ��� �� ����.
		GameObjectHandle RegisterToHandleTable(GameObject* pGameObject);
		void AddToDestroyQueue(GameObject* pGameObject);

		// ��-���� ������Ʈ���� �ű�� �Լ�
		void MoveToActiveGroup(GameObject* pGameObject);
		// ��-���� ������Ʈ���� �ű�� �Լ�
		void MoveToInactiveGroup(GameObject* pGameObject);

		void AddToActiveGroup(GameObject* pGameObject);
		void AddToInactiveGroup(GameObject* pGameObject);

		void RemoveFromGroup(GameObject* pGameObject);	// Active/Inactive group���� ������ ����
		void RemoveDestroyedGameObjects();

		void SetActive(GameObject* pGameObject, bool active);

		// pTransform�� �׻� nullptr�� �ƴ� �Է�
		// pNewTransform�� nullptr�� �Է����� ���� �� ����.
		bool SetParent(Transform* pTransform, Transform* pNewParentTransform);

		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	private:
		static GameObjectManager* s_pInstance;

		uint64_t m_uniqueId;
		SlimRWLock m_lock;
		std::vector<GameObject*> m_activeGroup;		// �˻� �� ��ü ���̺��� ��ȸ�� �ʿ� ����
		std::vector<GameObject*> m_inactiveGroup;	// ��Ȱ��ȭ�� ���� ������Ʈ�� (�˻� ��󿡼� ����)
		std::vector<uint32_t> m_emptyHandleTableIndex;
		std::vector<GameObject*> m_handleTable;
		std::vector<GameObject*> m_destroyed;
	};
}
