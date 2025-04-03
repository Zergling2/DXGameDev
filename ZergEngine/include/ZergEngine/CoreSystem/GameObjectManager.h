#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class GameObject;

	class GameObjectManager : public ISubsystem
	{
		friend class Runtime;
		friend class GameObject;
		friend class GameObjectHandle;
		friend class SceneManager;
		ZE_DECLARE_SINGLETON(GameObjectManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		// deferred �÷��׸� �����ϰ� ���ӿ�����Ʈ�� �� ������ �ְ� m_index�� ��ȿ�� ���� �������ְ� ��ȿ�� ���� �ڵ��� ��ȯ���־�� �Ѵ�.
		GameObjectHandle Register(GameObject* pGameObject);

		// �ش� ��ü�� �����ϴ� ������ nullptr�� �����.
		// ���� ��ü �ı��� ��û�Ǿ��µ� �ش� �ε����� �ִ°� �� ��ü�� �ƴ϶�� ���� �߸��� ��Ȳ�̴�. �ڵ� ��� ���ٸ� ����ϱ� ������
		// ��۸� ������ ������ ����� �Ѵ�. ����� �����غ��� �Ѵ�.
		// �޸� ������ ������ �ʴ´�. �޸� ������ Runtime::Destroy() �Լ����� �Ǵ� SceneManager���� ������ ���� �ı��� �� ���ش�.
		void Unregister(GameObject* pGameObject);

		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	private:
		uint64_t m_uniqueId;
		std::vector<GameObject*> m_activeGameObjects;	// �˻� �� ��ü ���̺��� ��ȸ�� �ʿ並 ���ش�.
		GameObject* m_ptrTable[64 * 128];
	};
}
