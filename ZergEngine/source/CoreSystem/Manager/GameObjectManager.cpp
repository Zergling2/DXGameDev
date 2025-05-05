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
	, m_inactiveGameObjects()
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
	assert(m_inactiveGameObjects.size() == 0);

	m_table.clear();
	m_uniqueId = 0;
}

void GameObjectManagerImpl::AddToDestroyQueue(GameObject* pGameObject)
{
	assert(pGameObject->IsOnTheDestroyQueue() == false);

	pGameObject->OnFlag(GOF_ON_DESTROY_QUEUE);
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

GameObjectHandle GameObjectManagerImpl::Register(GameObject* pGameObject)
{
	assert(pGameObject != nullptr);
	assert(pGameObject->m_tableIndex == std::numeric_limits<uint32_t>::max());
	assert(pGameObject->m_activeIndex == std::numeric_limits<uint32_t>::max());

	GameObjectHandle hGameObject;

	// 1. ���̺� ���
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

	// Direct access vector�� ����
	// �����Ǿ��ٰ� Register �Ǵ°�� Active �÷��װ� 0�� �� �����Ƿ�
	// �и��ؾ� �Ѵ�.
	if (pGameObject->IsActive())
		AddPtrToActiveVector(pGameObject);
	else
		AddPtrToInactiveVector(pGameObject);

	// deferred �÷��� ���� (AddComponent �ùٸ��� �۵� ����)
	pGameObject->m_flag = static_cast<GAMEOBJECT_FLAG>(pGameObject->m_flag & ~GOF_DEFERRED);

	return hGameObject;
}

void GameObjectManagerImpl::RemoveDestroyedGameObjects()
{
	for (GameObject* pGameObject : m_destroyed)
	{
		assert(pGameObject->IsOnTheDestroyQueue());

		auto& vector = pGameObject->IsActive() ? m_activeGameObjects : m_inactiveGameObjects;

		uint32_t activeSize = static_cast<uint32_t>(vector.size());
		assert(activeSize > 0);
		assert(m_table[pGameObject->m_tableIndex] == pGameObject);
		assert(pGameObject->m_tableIndex != std::numeric_limits<uint32_t>::max());
		assert(pGameObject->m_activeIndex != std::numeric_limits<uint32_t>::max());

		const uint32_t activeIndex = pGameObject->m_activeIndex;
		const uint32_t lastIndex = activeSize - 1;

		if (activeIndex != lastIndex)
		{
			// ������ ���� ������Ʈ�� ��ġ�� �¹ٲ� ��
			std::swap(vector[activeIndex], vector[lastIndex]);
			vector[activeIndex]->m_activeIndex = activeIndex;
		}

		// Active ��Ͽ��� ����
		vector.pop_back();

		// ���̺��� ����
		m_table[pGameObject->m_tableIndex] = nullptr;

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
