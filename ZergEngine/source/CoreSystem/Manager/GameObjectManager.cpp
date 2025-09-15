#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

GameObjectManager* GameObjectManager::s_pInstance = nullptr;

static constexpr size_t HANDLE_TABLE_INIT_SIZE = 8192;

GameObjectManager::GameObjectManager()
	: m_uniqueId(0)
	, m_lock()
	, m_activeGroup()
	, m_inactiveGroup()
	, m_emptyHandleTableIndex(HANDLE_TABLE_INIT_SIZE)	// ����ȭ�� �ּ�ȭ�ϱ� ���ؼ� ���ʿ� �ڵ� ���̺��� ������� ������ ������ŭ �ε��� ���۸� �Ҵ��Ѵ�.
	, m_handleTable(HANDLE_TABLE_INIT_SIZE, nullptr)
	, m_destroyed()
{
	m_lock.Init();
}

GameObjectManager::~GameObjectManager()
{
}

void GameObjectManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new GameObjectManager();
}

void GameObjectManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void GameObjectManager::Init()
{
	m_uniqueId = 0;

	// ���͸� ����ó�� ����� ���̹Ƿ� ���� �ؿ� ���� �ε����� �־�ξ� 0�� �ε������� ����� �� �ֵ��� �Ѵ�.
	const size_t handleTableEndIndex = m_handleTable.size() - 1;
	for (size_t i = 0; i < m_emptyHandleTableIndex.size() - 1; ++i)
		m_emptyHandleTableIndex[i] = static_cast<uint32_t>(handleTableEndIndex - i);
}

void GameObjectManager::UnInit()
{
	// ��Ÿ�ӿ��� �̸� ��� �������׾�� �ϹǷ�
	assert(m_activeGroup.size() == 0);
	assert(m_inactiveGroup.size() == 0);

	m_handleTable.clear();
}

void GameObjectManager::Deploy(GameObject* pGameObject)
{
	// �߿� (�÷��� ����)
	pGameObject->OffFlag(GAMEOBJECT_FLAG::PENDING);

	pGameObject->IsActive() ? this->AddToActiveGroup(pGameObject) : this->AddToInactiveGroup(pGameObject);
}

GameObjectHandle GameObjectManager::FindGameObject(PCWSTR name)
{
	GameObjectHandle hGameObject;	// null handle

	for (GameObject* pGameObject : m_activeGroup)
	{
		if (wcscmp(pGameObject->GetName(), name) == 0)
		{
			hGameObject = pGameObject->ToHandle();
			break;
		}
	}

	return hGameObject;
}

GameObjectHandle GameObjectManager::CreateObject(PCWSTR name)
{
	GAMEOBJECT_FLAG flag = GAMEOBJECT_FLAG::ACTIVE;

	GameObject* pNewGameObject = new GameObject(this->AssignUniqueId(), flag, name);

	GameObjectHandle hNewGameObject = this->RegisterToHandleTable(pNewGameObject);
	this->AddToActiveGroup(pNewGameObject);

	return hNewGameObject;
}

GameObjectHandle GameObjectManager::CreatePendingObject(GameObject** ppNewGameObject, PCWSTR name)
{
	GAMEOBJECT_FLAG flag =
		static_cast<GAMEOBJECT_FLAG>(static_cast<uint16_t>(GAMEOBJECT_FLAG::PENDING) | static_cast<uint16_t>(GAMEOBJECT_FLAG::ACTIVE));

	// PENDING GAME OBJECT
	GameObject* pNewGameObject = new GameObject(this->AssignUniqueId(), flag, name);
	*ppNewGameObject = pNewGameObject;
	return this->RegisterToHandleTable(pNewGameObject);
}

void GameObjectManager::RequestDestroy(GameObject* pGameObject)
{
	// ������ ������Ʈ�� �����ϴ� ���� OnLoadScene���� Destroy�� �Ѵٴ� �ǹ��ε� �̰��� ������� �ʴ´�.
	assert(!pGameObject->IsPending());

	// �ڽ� ������Ʈ���� Destroy
	for (Transform* pChildTransform : pGameObject->m_transform.m_children)
		pChildTransform->m_pGameObject->Destroy();

	// �����ϰ� �ִ� ��� ������Ʈ ����
	for (IComponent* pComponent : pGameObject->m_components)
		pComponent->Destroy();	// ���ο��� ��ٷ� GameObject <-> Component�� ���� ������ �ȵ� (���� ��ȸ�ϴ� ���ͷ����� ������!) (��������)
	
	if (!pGameObject->IsOnTheDestroyQueue())	// �ſ� �߿�! (�ߺ� �������� ���� �ߺ� delete �� �ջ� ����)
		this->AddToDestroyQueue(pGameObject);
}

GameObject* GameObjectManager::ToPtr(uint32_t tableIndex, uint64_t id) const
{
	// �� �񵿱� �� �ε� ���� �� ����ؾ� �� ����
	// ���� ���� ���̺��� �����Ϸ��� ToPtr���� ���� �ɾ�� �Ѵ�.
	// (�񵿱� �ε� ������ ���ؼ� ToPtr ���߿� �ڵ� ���̺��� �ּҰ� �ٲ����� �� �����Ƿ�.)
	// ���� ���� ���̺��� ����ϸ� ���� ���� �ʾƵ� �ǳ�, ��Ÿ�ӿ� ������Ʈ�� ������ ���ѵȴٴ� ���� �ƽ�������.

	// ����� Thread unsafe ����.

	assert(tableIndex < GameObjectManager::GetInstance()->m_handleTable.size());

	GameObject* pGameObject = GameObjectManager::GetInstance()->m_handleTable[tableIndex];

	if (pGameObject == nullptr || pGameObject->GetId() != id)
		return nullptr;
	else
		return pGameObject;
}

GameObjectHandle GameObjectManager::RegisterToHandleTable(GameObject* pGameObject)
{
	assert(pGameObject->m_tableIndex == (std::numeric_limits<uint32_t>::max)());
	assert(pGameObject->m_groupIndex == (std::numeric_limits<uint32_t>::max)());

	GameObjectHandle hGameObject;

	{
		// Auto Exclusive Lock
		AutoAcquireSlimRWLockExclusive autolock(m_lock);

		// �ڵ� ���̺��� �� �ڸ��� �˻�
		uint32_t emptyIndex;
		const size_t ehtIdxSize = m_emptyHandleTableIndex.size();
		if (ehtIdxSize > 0)
		{
			emptyIndex = m_emptyHandleTableIndex[ehtIdxSize - 1];
			m_emptyHandleTableIndex.pop_back();
		}
		else
		{
			// ���� �ڵ� ���̺� �� ������ ���� ���
			m_handleTable.push_back(nullptr);	// �ڵ� ���̺� �� ���� �߰�
			emptyIndex = static_cast<uint32_t>(m_handleTable.size() - 1);
		}

		m_handleTable[emptyIndex] = pGameObject;
		pGameObject->m_tableIndex = emptyIndex;
	}

	hGameObject = GameObjectHandle(pGameObject->m_tableIndex, pGameObject->GetId());	// ��ȿ�� �ڵ� �غ�

	return hGameObject;
}

void GameObjectManager::AddToDestroyQueue(GameObject* pGameObject)
{
	assert(!pGameObject->IsOnTheDestroyQueue());

	m_destroyed.push_back(pGameObject);
	pGameObject->OnFlag(GAMEOBJECT_FLAG::ON_DESTROY_QUEUE);
}

void GameObjectManager::MoveToActiveGroup(GameObject* pGameObject)
{
	assert(!pGameObject->IsPending());	// ��-���� ���¿���� �Ѵ�.
	assert(!pGameObject->IsActive());	// Inactive ���¿���� �Ѵ�.

	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
	// 1. Inactive group���� ����
	this->RemoveFromGroup(pGameObject);
	// ������������������������������������������������������������������������������������������������������������������������������������������������������������


	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
	// 2. Active group�� �߰�
	this->AddToActiveGroup(pGameObject);
	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
}

void GameObjectManager::MoveToInactiveGroup(GameObject* pGameObject)
{
	assert(!pGameObject->IsPending());	// ��-���� ���¿���� �Ѵ�.
	assert(pGameObject->IsActive());	// Active ���¿���� �Ѵ�.

	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
	// 1. Active group���� ����
	this->RemoveFromGroup(pGameObject);
	// ������������������������������������������������������������������������������������������������������������������������������������������������������������


	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
	// 2. Inactive group�� �߰�
	this->AddToInactiveGroup(pGameObject);
	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
}

void GameObjectManager::AddToActiveGroup(GameObject* pGameObject)
{
	m_activeGroup.push_back(pGameObject);
	pGameObject->m_groupIndex = static_cast<uint32_t>(m_activeGroup.size() - 1);
}

void GameObjectManager::AddToInactiveGroup(GameObject* pGameObject)
{
	m_inactiveGroup.push_back(pGameObject);
	pGameObject->m_groupIndex = static_cast<uint32_t>(m_inactiveGroup.size() - 1);
}

void GameObjectManager::RemoveFromGroup(GameObject* pGameObject)
{
	auto& group = pGameObject->IsActive() ? m_activeGroup : m_inactiveGroup;

	uint32_t groupSize = static_cast<uint32_t>(group.size());
	assert(groupSize > 0);

	const uint32_t groupIndex = pGameObject->m_groupIndex;
	const uint32_t lastIndex = groupSize - 1;

	assert(groupIndex < groupSize);
	// assert(groupIndex != std::numeric_limits<uint32_t>::max());
	assert(group[groupIndex] == pGameObject);	// �߿�!

	// ������� ������Ʈ �����Ͱ� �� �ڿ� �ִ°��� �ƴѰ��
	// �������� ��ġ�� �����Ϳ� ��ġ�� �ٲ۴�.
	if (groupIndex != lastIndex)
	{
		std::swap(group[groupIndex], group[lastIndex]);
		group[groupIndex]->m_groupIndex = groupIndex;	// �������� �ִ� ������Ʈ�� groupIndex�� �ùٸ��� ������Ʈ ���־�� ��.
	}

	// �ҼӵǾ� �ִ� vector���� ����
	group.pop_back();
}

void GameObjectManager::RemoveDestroyedGameObjects()
{
	// �ı��� ���ӿ�����Ʈ ���� ���� �񵿱� �� �ε� ���������� ���ӿ�����Ʈ �������� ���� m_table�� ���Ҵ��� �Ͼ �� �����Ƿ�
	// �� ȹ���� �ݵ�� �ʿ��ϴ�.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (GameObject* pGameObject : m_destroyed)
	{
		/* ���� */
		assert(!pGameObject->IsPending());			// �ε� �� �Ҽ��� ������Ʈ�� �ı��� �� ����.
		assert(pGameObject->IsOnTheDestroyQueue());	// �ı� ť�� ���� ��쿡�� �� ON_DESTROY_QUEUE �÷��װ� ���� �־�߸� �Ѵ�.

		// Step 1. Transform �ڽ� �θ� ���� ����
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

		for (Transform* pChildTransform : pTransform->m_children)
		{
			assert(pChildTransform->m_pParentTransform == pTransform);	// �ڽŰ��� ���� Ȯ��
			

			// �߿� ����Ʈ
			pChildTransform->m_pParentTransform = nullptr;	// �ؿ��� �� delete�Ǵ� �ڽ��� ����(Step 1���� ������)�ϴ� ���� ���� (������ ��۸� ������ ����)
			// �̷� ���ؼ� ��ü �ı� �ÿ��� ��Ʈ ������Ʈ������ �Ǻ�(RootGroup���� ������ �����ؾ� ��)�� �� �� �θ� Transform �����ͷ�
			// �Ǵ��� �� ����. ���� ��¥ ��Ʈ ��忴������ üũ�س��� ���� UIOBJECT_FLAG���������� REAL_ROOT��� �÷��׸� ����Ѵ�.
		}
		// pTransform->m_children.clear();	// ��ü delete�� �ڵ� �Ҹ�

		this->RemoveFromGroup(pGameObject);

		// �ڵ� ���̺��� ����
		assert(m_handleTable[pGameObject->m_tableIndex] == pGameObject);
		m_handleTable[pGameObject->m_tableIndex] = nullptr;
		m_emptyHandleTableIndex.push_back(pGameObject->m_tableIndex);		// �ı��Ǵ� ������Ʈ�� ����ϴ� �ڵ� ���̺� �ε����� �ٽ� ����

		delete pGameObject;
	}

	m_destroyed.clear();
}

void GameObjectManager::SetActive(GameObject* pGameObject, bool active)
{
	// �̹� �ش� Ȱ�� ���°� �����Ǿ� �ִ� ��� �Լ� ����
	if (pGameObject->IsActive() == active)
		return;
	
	if (active)
	{
		for (IComponent* pComponent : pGameObject->m_components)
			pComponent->Enable();

		// PENDING ���°� �ƴ� ��쿡�� ������ �̵� (PENDING ���¿����� Active/Inactive ���Ϳ� �����Ͱ� �������� �ʴ´�.)
		if (!pGameObject->IsPending())
			this->MoveToActiveGroup(pGameObject);

		
		pGameObject->OnFlag(GAMEOBJECT_FLAG::ACTIVE);
	}
	else
	{
		for (IComponent* pComponent : pGameObject->m_components)
			pComponent->Disable();

		// PENDING ���°� �ƴ� ��쿡�� ������ �̵� (PENDING ���¿����� Active/Inactive ���Ϳ� �����Ͱ� �������� �ʴ´�.)
		if (!pGameObject->IsPending())
			this->MoveToInactiveGroup(pGameObject);


		pGameObject->OffFlag(GAMEOBJECT_FLAG::ACTIVE);
	}
}

bool GameObjectManager::SetParent(Transform* pTransform, Transform* pNewParentTransform)
{
	GameObject* pGameObject = pTransform->m_pGameObject;
	Transform* pOldParentTransform = pTransform->m_pParentTransform;

	if (pGameObject->IsOnTheDestroyQueue())
		return false;

	// �ڱ� �ڽ��� �θ�� �����Ϸ��� �ϰų� �����Ϸ��� �θ� �̹� �θ��� ��쿡�� ����
	if (pTransform == pNewParentTransform || pOldParentTransform == pNewParentTransform)
		return false;

	// ����Ŭ �˻�
	// pTransform�� �̹� ���� �������� �ϰ� �ִ� ��� or �ı� ������ ������Ʈ�� �θ�� �����Ϸ��� ���
	// ����ó�� ������ ������ ��ü�� Destroy �Լ����� �ڽ� ������Ʈ����� ��� Destroy ť�� �־��ִ� ��å�� �ϰ����� ���� �ʴ´�.
	// (�θ�� �ı��Ǵµ� �ڽ��� �ı����� �ʴ� ��� �߻�)
	if (pNewParentTransform != nullptr)
		if (pNewParentTransform->IsDescendantOf(pTransform) || pNewParentTransform->m_pGameObject->IsOnTheDestroyQueue())
			return false;

	// ��Ʈ ��� ó�� ����� ��
	// 1. pOldParentTransform != nullptr && pNewParentTransform == nullptr (������ ��Ʈ ��尡 �ƴϾ��� ���� ��Ʈ ��尡 �Ƿ��� ���)
	// 2. pOldParentTransform != nullptr && pNewParentTransform != nullptr (������ ��Ʈ ��尡 �ƴϾ��� ���ݵ� ��Ʈ ��尡 �Ǵ� ���� �ƴ� ���)
	// 3. pOldParentTransform == nullptr && pNewParentTransform == nullptr (������ ��Ʈ ��忴�� ���ݵ� ��Ʈ ��尡 �Ƿ��� ���) (�� ���� �Լ� ���� ���� ����ó����)
	// 4. pOldParentTransform == nullptr && pNewParentTransform != nullptr (������ ��Ʈ ��忴�� ���� ��Ʈ ��尡 �ƴϰ� �Ǵ� ���)

	// �θ� nullptr�� �ƴϾ��� ��쿡�� ���� �θ𿡼� �ڽ� �����͸� ã�Ƽ� ����
	if (pOldParentTransform != nullptr)
	{
#if defined(DEBUG) || defined(_DEBUG)
		bool find = false;
#endif
		std::vector<Transform*>::const_iterator end = pOldParentTransform->m_children.cend();
		std::vector<Transform*>::const_iterator iter = pOldParentTransform->m_children.cbegin();
		while (iter != end)
		{
			if (*iter == pTransform)
			{
				pOldParentTransform->m_children.erase(iter);
#if defined(DEBUG) || defined(_DEBUG)
				find = true;
#endif
				break;
			}
			++iter;
		}
		assert(find == true);	// �ڽ����� �����߾���� ��
	}

	// ���� �θ� nullptr�� �ƴ϶��
	if (pNewParentTransform != nullptr)
	{
		// �θ��� �ڽ� ����� ������Ʈ
		pNewParentTransform->m_children.push_back(pTransform);

		// �θ� ��Ȱ�� �����̸� �ڽĵ� ��Ȱ��ȭ
		if (pNewParentTransform->m_pGameObject->IsActive() == false)
			pTransform->m_pGameObject->SetActive(false);
	}

	// �θ� �����͸� ���ο� �θ�� ������Ʈ
	pTransform->m_pParentTransform = pNewParentTransform;

	return true;
}
