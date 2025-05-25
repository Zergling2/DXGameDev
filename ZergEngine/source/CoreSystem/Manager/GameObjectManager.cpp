#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

namespace ze
{
	GameObjectManagerImpl GameObjectManager;
}

using namespace ze;

GameObjectManagerImpl::GameObjectManagerImpl()
	: m_uniqueId(0)
	, m_lock()
	, m_destroyed()
	, m_activeGameObjects()
	, m_inactiveGameObjects()
	, m_table(8192, nullptr)
{
	m_lock.Init();
}

GameObjectManagerImpl::~GameObjectManagerImpl()
{
}

void GameObjectManagerImpl::Init(void* pDesc)
{
	m_uniqueId = 0;
}

void GameObjectManagerImpl::Release()
{
	assert(m_activeGameObjects.size() == 0);
	assert(m_inactiveGameObjects.size() == 0);

	m_table.clear();
	m_uniqueId = 0;
}

GameObjectHandle GameObjectManagerImpl::RegisterToHandleTable(GameObject* pGameObject)
{
	GameObjectHandle hGameObject;

	// Auto Exclusive Lock
	AutoAcquireSlimRWLockExclusive autolock(m_lock);
	
	// ���̺� ���
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

	m_table[emptyIndex] = pGameObject;
	pGameObject->m_tableIndex = emptyIndex;

	hGameObject = GameObjectHandle(emptyIndex, pGameObject->GetId());	// ��ȿ�� �ڵ� �غ�

	return hGameObject;
}

void GameObjectManagerImpl::AddToDestroyQueue(GameObject* pGameObject)
{
	assert(pGameObject->IsOnTheDestroyQueue() == false);

	pGameObject->OnFlag(GAMEOBJECT_FLAG::ON_DESTROY_QUEUE);
	m_destroyed.push_back(pGameObject);
}

GameObjectHandle GameObjectManagerImpl::FindGameObject(PCWSTR name)
{
	GameObjectHandle hGameObject;	// null handle

	for (GameObject* pGameObject : m_activeGameObjects)
	{
		if (wcscmp(pGameObject->GetName(), name) == 0)
		{
			hGameObject = pGameObject->ToHandle();
			break;
		}
	}

	return hGameObject;
}

void GameObjectManagerImpl::RemoveDestroyedGameObjects()
{
	// �ı��� ���ӿ�����Ʈ ���� ���� �񵿱� �� �ε� ���������� ���ӿ�����Ʈ �������� ���� m_table�� ���Ҵ��� �Ͼ �� �����Ƿ�
	// �� ȹ���� �ݵ�� �ʿ��ϴ�.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (GameObject* pGameObject : m_destroyed)
	{
		assert(pGameObject->IsDeferred() == false);
		assert(pGameObject->IsOnTheDestroyQueue() == true);

		// 1. Transform �ڽ� �θ� ���� ����
		Transform* pTransform = &pGameObject->m_transform;
		Transform* pParentTransform = pTransform->m_pParentTransform;
		if (pParentTransform != nullptr)
		{
#if defined(DEBUG) || defined(_DEBUG)
			bool find = false;
#endif
			std::vector<Transform*>::const_iterator end = pParentTransform->m_children.cend();
			std::vector<Transform*>::const_iterator iter = pParentTransform->m_children.cbegin();
			while (iter != end)
			{
				if (*iter == pTransform)
				{
#if defined(DEBUG) || defined(_DEBUG)
					find = true;
#endif
					pParentTransform->m_children.erase(iter);
					break;
				}
				++iter;
			}
			assert(find == true);
		}

		for (Transform* pChild : pTransform->m_children)
		{
			assert(pChild->m_pParentTransform == pTransform);
			pChild->m_pParentTransform = nullptr;	// �ؿ��� �� delete�� �ڽ��� �����ϴ� ���� ���� (��۸������� ����)
		}
		// pTransform->m_children.clear();	// ��ü delete�� �ڵ� �Ҹ�

		if (pGameObject->IsActive())
			RemovePtrFromActiveVector(pGameObject);
		else
			RemovePtrFromInactiveVector(pGameObject);

		// ���̺��� ����
		m_table[pGameObject->m_tableIndex] = nullptr;

		// for debugging...
		pGameObject->m_activeIndex = std::numeric_limits<uint32_t>::max();
		pGameObject->m_tableIndex = std::numeric_limits<uint32_t>::max();

		delete pGameObject;
	}

	m_destroyed.clear();
}

void GameObjectManagerImpl::MoveToInactiveVectorFromActiveVector(GameObject* pGameObject)
{
	// ��Ȱ��ȭ�� ���ӿ�����Ʈ���� Ȯ�� assert
	assert(pGameObject->IsActive() == false);
	RemovePtrFromActiveVector(pGameObject);
	AddPtrToInactiveVector(pGameObject);
}

void GameObjectManagerImpl::MoveToActiveVectorFromInactiveVector(GameObject* pGameObject)
{
	// Ȱ��ȭ�� ���ӿ�����Ʈ���� Ȯ�� assert
	assert(pGameObject->IsActive() == true);
	RemovePtrFromInactiveVector(pGameObject);
	AddPtrToActiveVector(pGameObject);
}

void GameObjectManagerImpl::AddPtrToVector(std::vector<GameObject*>& vector, GameObject* pGameObject)
{
	vector.push_back(pGameObject);
	pGameObject->m_activeIndex = static_cast<uint32_t>(vector.size() - 1);
}

void GameObjectManagerImpl::RemovePtrFromVector(std::vector<GameObject*>& vector, GameObject* pGameObject)
{
	uint32_t vectorSize = static_cast<uint32_t>(vector.size());
	assert(vectorSize > 0);

	const uint32_t activeIndex = pGameObject->m_activeIndex;
	const uint32_t lastIndex = vectorSize - 1;

	assert(activeIndex < vectorSize);
	// assert(activeIndex != std::numeric_limits<uint32_t>::max());
	assert(vector[activeIndex] == pGameObject);	// �߿�!

	// ������� ���ӿ�����Ʈ �����Ͱ� �� �ڿ� �ִ°��� �ƴѰ��
	// �������� ��ġ�� �����Ϳ� ��ġ�� �ٲ۴�.
	if (activeIndex != lastIndex)
	{
		std::swap(vector[activeIndex], vector[lastIndex]);
		vector[activeIndex]->m_activeIndex = activeIndex;	// �������� �ִ� ���ӿ�����Ʈ�� activeIndex�� �ùٸ��� ������Ʈ ���־�� �Ѵ�!
	}

	// �ҼӵǾ� �ִ� vector���� ����
	vector.pop_back();
}
