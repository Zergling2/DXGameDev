#include <ZergEngine\CoreSystem\GameObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

namespace ze
{
	GameObjectManagerImpl GameObjectManager;
}

using namespace ze;

GameObjectManagerImpl::GameObjectManagerImpl()
	: m_uniqueId(0)
	, m_activeGameObjects()
	, m_ptrTable{}
{
}

GameObjectManagerImpl::~GameObjectManagerImpl()
{
}

void GameObjectManagerImpl::Init(void* pDesc)
{
	m_uniqueId = 0;
	ZeroMemory(m_ptrTable, sizeof(m_ptrTable));
}

void GameObjectManagerImpl::Release()
{
	for (auto p : m_activeGameObjects)
		delete p;
	m_activeGameObjects.clear();

	ZeroMemory(m_ptrTable, sizeof(m_ptrTable));

	m_uniqueId = 0;
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

	GameObjectHandle hGameObject;

	do
	{
		// �� �ڸ� �˻�
		uint32_t emptyIndex = std::numeric_limits<uint32_t>::max();
		for (uint32_t i = 0; i < _countof(m_ptrTable); ++i)
		{
			if (m_ptrTable[i] == nullptr)
			{
				emptyIndex = i;
				break;
			}
		}

		if (emptyIndex == std::numeric_limits<uint32_t>::max())
			break;

		hGameObject = GameObjectHandle(emptyIndex, pGameObject->GetId());	// ��ȿ�� �ڵ� �غ�
		m_activeGameObjects.push_back(pGameObject);
		pGameObject->m_activeIndex = static_cast<uint32_t>(m_activeGameObjects.size() - 1);
		m_ptrTable[emptyIndex] = pGameObject;
		pGameObject->m_index = emptyIndex;

		// deferred �÷��� ���� (AddComponent �ùٸ��� �۵� ����)
		pGameObject->m_flag = static_cast<GAMEOBJECT_FLAG>(pGameObject->m_flag & ~GOF_DEFERRED);
	} while (false);

	return hGameObject;
}

void GameObjectManagerImpl::Unregister(GameObject* pGameObject)
{
	uint32_t vectorSize = static_cast<uint32_t>(m_activeGameObjects.size());
	assert(vectorSize > 0);
	assert(m_ptrTable[pGameObject->m_index] == pGameObject);
	assert(pGameObject->m_index != std::numeric_limits<uint32_t>::max());
	assert(pGameObject->m_activeIndex != std::numeric_limits<uint32_t>::max());

	const uint32_t activeIndex = pGameObject->m_activeIndex;
	const uint32_t lastIndex = vectorSize - 1;

	if (activeIndex != lastIndex)
	{
		// ������ ������Ʈ�� ��ġ�� �¹ٲ� ��
		std::swap(m_activeGameObjects[activeIndex], m_activeGameObjects[lastIndex]);
		m_activeGameObjects[activeIndex]->m_activeIndex = activeIndex;
	}

	// Active ��Ͽ��� ����
	m_activeGameObjects.pop_back();
	pGameObject->m_activeIndex = std::numeric_limits<uint32_t>::max();

	// ���̺��� ����
	m_ptrTable[pGameObject->m_index] = nullptr;
	pGameObject->m_index = std::numeric_limits<uint32_t>::max();
}
