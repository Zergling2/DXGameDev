#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

namespace ze
{
	GameObjectManagerImpl GameObjectManager;
}

using namespace ze;

GameObjectManagerImpl::GameObjectManagerImpl()
	: m_uniqueId(0)
	, m_destroyed()
	, m_activeGameObjects()
	, m_table(8192, nullptr)
{
}

GameObjectManagerImpl::~GameObjectManagerImpl()
{
}

void GameObjectManagerImpl::Init(void* pDesc)
{
	// m_uniqueId = 0;
	// std::fill(m_table.begin(), m_table.end(), nullptr);
}

void GameObjectManagerImpl::Release()
{
	assert(m_activeGameObjects.size() == 0);

	m_activeGameObjects.clear();
	m_table.clear();
	m_uniqueId = 0;
}

void GameObjectManagerImpl::AddToDestroyQueue(GameObject* pGameObject)
{
	assert(pGameObject->IsOnTheDestroyQueue() == false);

	pGameObject->SetOnDestroyQueueFlag();
	m_destroyed.push_back(pGameObject);
}

GameObjectHandle GameObjectManagerImpl::FindGameObject(PCWSTR name)
{
	GameObjectHandle hGameObject;

	for (auto p : m_activeGameObjects)
	{
		if (wcscmp(p->GetName(), name) == 0)
		{
			hGameObject = p->ToHandle();
			break;
		}
	}

	return hGameObject;
}

GameObjectHandle GameObjectManagerImpl::Register(GameObject* pGameObject)
{
	assert(pGameObject != nullptr);
	assert(pGameObject->m_tableIndex == std::numeric_limits<uint32_t>::max());
	assert(pGameObject->m_activeIndex == std::numeric_limits<uint32_t>::max());

	GameObjectHandle hGameObject;

	// �� �ڸ� �˻�
	uint32_t emptyIndex = std::numeric_limits<uint32_t>::max();
	const uint32_t tableSize = static_cast<uint32_t>(m_table.size());
	for (uint32_t i = 0; i < tableSize; ++i)
	{
		if (m_table[i] == nullptr)
		{
			emptyIndex = i;
			break;
		}
	}
	
	// ���� �� �ڸ��� ã�� ������ ���
	if (emptyIndex == std::numeric_limits<uint32_t>::max())
	{
		emptyIndex = tableSize;
		m_table.push_back(nullptr);	// ���̺� ���� Ȯ��
	}

	m_activeGameObjects.push_back(pGameObject);
	pGameObject->m_activeIndex = static_cast<uint32_t>(m_activeGameObjects.size() - 1);
	m_table[emptyIndex] = pGameObject;
	pGameObject->m_tableIndex = emptyIndex;

	// deferred �÷��� ���� (AddComponent �ùٸ��� �۵� ����)
	pGameObject->m_flag = static_cast<GAMEOBJECT_FLAG>(pGameObject->m_flag & ~GOF_DEFERRED);

	hGameObject = GameObjectHandle(emptyIndex, pGameObject->GetId());	// ��ȿ�� �ڵ� �غ�

	return hGameObject;
}

void GameObjectManagerImpl::RemoveDestroyedGameObjects()
{
	for (GameObject* pGameObject : m_destroyed)
	{
		assert(pGameObject->IsOnTheDestroyQueue());

		// �ڽ� �θ� ���� ����
		if (pGameObject->m_pParent != nullptr)
		{
#if defined(DEBUG) || defined(_DEBUG)
			bool find = false;
#endif
			std::list<GameObject*>::const_iterator end = pGameObject->m_pParent->m_children.cend();
			std::list<GameObject*>::const_iterator iter = pGameObject->m_pParent->m_children.cbegin();
			while (iter != end)
			{
				if (*iter == pGameObject)
				{
#if defined(DEBUG) || defined(_DEBUG)
					find = true;
#endif
					pGameObject->m_pParent->m_children.erase(iter);
					break;
				}
			}
			assert(find == true);
		}

		for (GameObject* pChild : pGameObject->m_children)
		{
			assert(pChild->m_pParent == pGameObject);
			assert(pChild->IsOnTheDestroyQueue() == true);
			pChild->m_pParent = nullptr;	// �ؿ��� �� delete�� �ڽ��� �����ϴ� ���� ���� (��۸������� ����)
		}
		// pGameObject->m_children.clear();	// ��ü delete�� �ڵ� �Ҹ�

		uint32_t activeSize = static_cast<uint32_t>(m_activeGameObjects.size());
		assert(activeSize > 0);
		assert(m_table[pGameObject->m_tableIndex] == pGameObject);
		assert(pGameObject->m_tableIndex != std::numeric_limits<uint32_t>::max());
		assert(pGameObject->m_activeIndex != std::numeric_limits<uint32_t>::max());

		const uint32_t activeIndex = pGameObject->m_activeIndex;
		const uint32_t lastIndex = activeSize - 1;

		if (activeIndex != lastIndex)
		{
			// ������ ���� ������Ʈ�� ��ġ�� �¹ٲ� ��
			std::swap(m_activeGameObjects[activeIndex], m_activeGameObjects[lastIndex]);
			m_activeGameObjects[activeIndex]->m_activeIndex = activeIndex;
		}

		// Active ��Ͽ��� ����
		m_activeGameObjects.pop_back();

		// ���̺��� ����
		m_table[pGameObject->m_tableIndex] = nullptr;

		pGameObject->m_activeIndex = std::numeric_limits<uint32_t>::max();
		pGameObject->m_tableIndex = std::numeric_limits<uint32_t>::max();

		delete pGameObject;
	}

	m_destroyed.clear();
}
