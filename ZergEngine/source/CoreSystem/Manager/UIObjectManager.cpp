#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>

using namespace ze;

UIObjectManager* UIObjectManager::s_pInstance = nullptr;

UIObjectManager::UIObjectManager()
	: m_uniqueId(0)
	, m_lock()
	, m_destroyed()
	, m_roots()
	, m_activeGroup()
	, m_inactiveGroup()
	, m_handleTable(256, nullptr)
	, m_pObjectPressedByLButton(nullptr)
	, m_pObjectPressedByRButton(nullptr)
	, m_pObjectPressedByMButton(nullptr)
{
	m_lock.Init();
}

UIObjectManager::~UIObjectManager()
{
}

void UIObjectManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new UIObjectManager();
}

void UIObjectManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void UIObjectManager::Init()
{
	m_uniqueId = 0;
}

void UIObjectManager::UnInit()
{
	// ��Ÿ�� Ŭ�������� Ȱ��/��Ȱ�� ������Ʈ���� ��� �����߾�� �ϹǷ�
	assert(m_activeGroup.size() == 0);
	assert(m_inactiveGroup.size() == 0);

	m_handleTable.clear();
	m_uniqueId = 0;
}

void UIObjectManager::Deploy(IUIObject* pUIObject)
{
	pUIObject->OffFlag(UIOBJECT_FLAG::PENDING);	// �߿�!

	if (pUIObject->m_transform.m_pParentTransform == nullptr)
		this->AddToRootArray(pUIObject);

	pUIObject->IsActive() ? this->DeployToActiveGroup(pUIObject) : this->DeployToInactiveGroup(pUIObject);
}

void UIObjectManager::RequestDestroy(IUIObject* pUIObject)
{
	// ������ ������Ʈ�� �����ϴ� ���� OnLoadScene���� Destroy�� �Ѵٴ� �ǹ��ε� �̰��� ������� �ʴ´�.
	if (pUIObject->IsPending())
		return;

	// �ڽ� ������Ʈ���� Destroy
	for (RectTransform* pChildTransform : pUIObject->m_transform.m_children)
		pChildTransform->m_pUIObject->Destroy();

	this->AddToDestroyQueue(pUIObject);
}

IUIObject* UIObjectManager::ToPtr(uint32_t tableIndex, uint64_t id) const
{
	assert(tableIndex < UIObjectManager::GetInstance()->m_handleTable.size());

	IUIObject* pUIObject = UIObjectManager::GetInstance()->m_handleTable[tableIndex];

	if (pUIObject == nullptr || pUIObject->GetId() != id)
		return nullptr;
	else
		return pUIObject;
}

UIObjectHandle THREADSAFE UIObjectManager::RegisterToHandleTable(IUIObject* pUIObject)
{
	UIObjectHandle hUIObject;

	// Auto Exclusive Lock
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	// ���̺� ���
	// �� �ڸ� �˻�
	uint32_t emptyIndex;
	bool find = false;
	const uint32_t tableSize = static_cast<uint32_t>(m_handleTable.size());
	for (uint32_t i = 0; i < tableSize; ++i)
	{
		if (m_handleTable[i] == nullptr)
		{
			emptyIndex = i;
			find = true;
			break;
		}
	}

	// ���� �� �ڸ��� ã�� ������ ���
	if (!find)
	{
		emptyIndex = tableSize;
		m_handleTable.push_back(nullptr);	// ���̺� ���� Ȯ��
	}

	m_handleTable[emptyIndex] = pUIObject;
	pUIObject->m_tableIndex = emptyIndex;

	hUIObject = UIObjectHandle(emptyIndex, pUIObject->GetId());	// ��ȿ�� �ڵ� �غ�

	return hUIObject;
}

void UIObjectManager::AddToDestroyQueue(IUIObject* pUIObject)
{
	if (pUIObject->IsOnTheDestroyQueue())
		return;

	pUIObject->OnFlag(UIOBJECT_FLAG::ON_DESTROY_QUEUE);		// �ſ� �߿�! (���Ϳ� �ߺ� �������� ���� �ߺ� delete �� �ջ� ����)
	m_destroyed.push_back(pUIObject);
}

UIObjectHandle UIObjectManager::FindUIObject(PCWSTR name)
{
	UIObjectHandle hUIObject;	// null handle

	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (wcscmp(pUIObject->GetName(), name) == 0)
		{
			hUIObject = pUIObject->ToHandleBase();
			break;
		}
	}

	return hUIObject;
}

void UIObjectManager::MoveToActiveGroup(IUIObject* pUIObject)
{
	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
	// 1. Inactive group���� ����
	assert(!pUIObject->IsActive());	// Inactive ���¿���� �Ѵ�.
	this->RemoveFromGroup(pUIObject);
	// ������������������������������������������������������������������������������������������������������������������������������������������������������������


	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
	// 2. Active group�� �߰�
	this->AddToActiveGroup(pUIObject);
	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
}

void UIObjectManager::MoveToInactiveGroup(IUIObject* pUIObject)
{
	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
	// 1. Active group���� ����
	assert(pUIObject->IsActive());	// Active ���¿���� �Ѵ�.
	this->RemoveFromGroup(pUIObject);
	// ������������������������������������������������������������������������������������������������������������������������������������������������������������


	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
	// 2. Inactive group�� �߰�
	this->AddToInactiveGroup(pUIObject);
	// ������������������������������������������������������������������������������������������������������������������������������������������������������������
}

void UIObjectManager::AddToRootArray(IUIObject* pUIObject)
{
	assert(pUIObject->m_transform.m_pParentTransform == nullptr);

	m_roots.push_back(pUIObject);
	pUIObject->OnFlag(UIOBJECT_FLAG::REAL_ROOT);	// ���� �� REAL_ROOT �÷��׸� �Ҵ�.
}

void UIObjectManager::DeployToActiveGroup(IUIObject* pUIObject)
{
	assert(pUIObject->IsActive()); AddToActiveGroup(pUIObject);
}

void UIObjectManager::DeployToInactiveGroup(IUIObject* pUIObject)
{
	assert(!pUIObject->IsActive()); AddToInactiveGroup(pUIObject);
}

void UIObjectManager::AddToActiveGroup(IUIObject* pUIObject)
{
	assert(pUIObject->IsActive());	// �� �Լ��� ������ �ε�� ������Ʈ�� ���� �����ڷ� ���Ե� �� ȣ��ǹǷ� ACTIVE �÷��װ� �����־�� �Ѵ�.

	m_activeGroup.push_back(pUIObject);
	pUIObject->m_groupIndex = static_cast<uint32_t>(m_activeGroup.size() - 1);

	pUIObject->OnFlag(UIOBJECT_FLAG::ACTIVE);
}

void UIObjectManager::AddToInactiveGroup(IUIObject* pUIObject)
{
	assert(!pUIObject->IsActive());	// �� �Լ��� ������ �ε�� ������Ʈ�� ���� �����ڷ� ���Ե� �� ȣ��ǹǷ� ACTIVE �÷��װ� �����־�� �Ѵ�.

	m_inactiveGroup.push_back(pUIObject);
	pUIObject->m_groupIndex = static_cast<uint32_t>(m_inactiveGroup.size() - 1);

	pUIObject->OffFlag(UIOBJECT_FLAG::ACTIVE);
}

void UIObjectManager::RemoveFromRootArray(IUIObject* pUIObject)
{
	/*
	��κ� UI ���� �� ��Ʈ ������Ʈ�� ������ �����Ƿ� ��Ʈ �����͵��� ���� ������ ���� �� �˻��Ѵ�.
	*/

	assert(m_roots.size() > 0);

#if defined (DEBUG) || (_DEBUG)
	bool find = false;
#endif
	auto end = m_roots.cend();
	auto iter = m_roots.cbegin();
	while (iter != end)
	{
		if (*iter == pUIObject)
		{
#if defined (DEBUG) || (_DEBUG)
			find = true;
#endif
			m_roots.erase(iter);
			break;
		}

		++iter;
	}

	assert(find == true);

	pUIObject->OffFlag(UIOBJECT_FLAG::REAL_ROOT);
}

void UIObjectManager::RemoveFromGroup(IUIObject* pUIObject)
{
	auto& group = pUIObject->IsActive() ? m_activeGroup : m_inactiveGroup;
	
	uint32_t groupSize = static_cast<uint32_t>(group.size());
	assert(groupSize > 0);

	const uint32_t groupIndex = pUIObject->m_groupIndex;
	const uint32_t lastIndex = groupSize - 1;

	assert(groupIndex < groupSize);
	// assert(groupIndex != std::numeric_limits<uint32_t>::max());
	assert(group[groupIndex] == pUIObject);	// �߿�!

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

void UIObjectManager::RemoveDestroyedUIObjects()
{
	// �ı��� ���ӿ�����Ʈ ���� ���� �񵿱� �� �ε� ���������� ���ӿ�����Ʈ �������� ���� m_table�� ���Ҵ��� �Ͼ �� �����Ƿ�
	// �� ȹ���� �ݵ�� �ʿ��ϴ�.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (IUIObject* pUIObject : m_destroyed)
	{
		/* ���� */
		assert(pUIObject->IsPending() == false);	// �ε� �� �Ҽ��� ������Ʈ�� �ı��� �� ����.
		assert(pUIObject->IsOnTheDestroyQueue() == true);	// �ı� ť�� ���� ��쿡�� �� ON_DESTROY_QUEUE �÷��װ� ���� �־�߸� �Ѵ�.

		if (pUIObject == m_pObjectPressedByLButton)
			m_pObjectPressedByLButton = nullptr;

		// Step 1. Transform �ڽ� �θ� ���� ����
		RectTransform* pTransform = &pUIObject->m_transform;
		RectTransform* pParentTransform = pTransform->m_pParentTransform;
		if (pParentTransform != nullptr)
		{
#if defined(DEBUG) || defined(_DEBUG)
			bool find = false;
#endif
			std::vector<RectTransform*>::const_iterator end = pParentTransform->m_children.cend();
			std::vector<RectTransform*>::const_iterator iter = pParentTransform->m_children.cbegin();
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

		for (RectTransform* pChildTransform : pTransform->m_children)
		{
			assert(pChildTransform->m_pParentTransform == pTransform);	// �ڽŰ��� ���� Ȯ��


			// �߿� ����Ʈ
			pChildTransform->m_pParentTransform = nullptr;	// �ؿ��� �� delete�Ǵ� �ڽ��� ����(Step 1���� ������)�ϴ� ���� ���� (������ ��۸� ������ ����)
			// �̷� ���ؼ� ��ü �ı� �ÿ��� ��Ʈ ������Ʈ������ �Ǻ�(RootGroup���� ������ �����ؾ� ��)�� �� �� �θ� Transform �����ͷ�
			// �Ǵ��� �� ����. ���� ��¥ ��Ʈ ��忴������ üũ�س��� ���� UIOBJECT_FLAG���������� REAL_ROOT��� �÷��׸� ����Ѵ�.
		}
		// pTransform->m_children.clear();	// ��ü delete�� �ڵ� �Ҹ�

		if (pUIObject->IsRoot())
			this->RemoveFromRootArray(pUIObject);

		this->RemoveFromGroup(pUIObject);

		// �ڵ� ���̺��� ����
		assert(m_handleTable[pUIObject->m_tableIndex] == pUIObject);
		m_handleTable[pUIObject->m_tableIndex] = nullptr;

		// for debugging...
		pUIObject->m_groupIndex = std::numeric_limits<uint32_t>::max();
		pUIObject->m_tableIndex = std::numeric_limits<uint32_t>::max();

		delete pUIObject;
	}

	m_destroyed.clear();
}

void UIObjectManager::SetActive(IUIObject* pUIObject, bool active)
{
	// �̹� �ش� Ȱ�� ���°� �����Ǿ� �ִ� ��� �Լ� ����
	if (pUIObject->IsActive() == active)
		return;

	if (active)
	{
		// PENDING ���°� �ƴ� ��쿡�� ������ �̵� (PENDING ���¿����� Active/Inactive ���Ϳ� �����Ͱ� �������� �ʴ´�.)
		if (!pUIObject->IsPending())
			this->MoveToActiveGroup(pUIObject);
	}
	else
	{
		// PENDING ���°� �ƴ� ��쿡�� ������ �̵� (PENDING ���¿����� Active/Inactive ���Ϳ� �����Ͱ� �������� �ʴ´�.)
		if (!pUIObject->IsPending())
			this->MoveToInactiveGroup(pUIObject);
	}
}

bool UIObjectManager::SetParent(RectTransform* pTransform, RectTransform* pNewParentTransform)
{
	IUIObject* pUIObject = pTransform->m_pUIObject;
	RectTransform* pOldParentTransform = pTransform->m_pParentTransform;

	if (pUIObject->IsOnTheDestroyQueue())
		return false;

	// �ڱ� �ڽ��� �θ�� �����Ϸ��� �ϰų� �̹� �����Ϸ��� �θ� �̹� �θ��� ��쿡�� ����
	if (pTransform == pNewParentTransform || pOldParentTransform == pNewParentTransform)
		return false;

	// ����Ŭ �˻�
	// pTransform�� �̹� ���� �������� �ϰ� �ִ� ��� or �ı� ������ ������Ʈ�� �θ�� �����Ϸ��� ���
	// ����ó�� ������ ������ Runtime::Destroy()���� �ڽ� ������Ʈ����� ��� Destroy ť�� �־��ִ� ��å�� �ϰ����� ���� �ʴ´�.
	// (�θ�� �ı��Ǵµ� �ڽ��� �ı����� �ʴ� ��� �߻�)
	if (pNewParentTransform != nullptr)
		if (pNewParentTransform->IsDescendantOf(pTransform) || pNewParentTransform->m_pUIObject->IsOnTheDestroyQueue())
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
		std::vector<RectTransform*>::const_iterator end = pOldParentTransform->m_children.cend();
		std::vector<RectTransform*>::const_iterator iter = pOldParentTransform->m_children.cbegin();
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

	// �θ��� �ڽ� ����� ������Ʈ
	if (pNewParentTransform != nullptr)
		pNewParentTransform->m_children.push_back(pTransform);

	// �θ� �����͸� ���ο� �θ�� ������Ʈ
	pTransform->m_pParentTransform = pNewParentTransform;


	// �ε� ���� �ƴ� ������Ʈ�� Root �׷쿡�� �������� ���θ� ������Ʈ�ؾ���.
	if (!pUIObject->IsPending())
	{
		if (pOldParentTransform == nullptr)
		{
			if (pNewParentTransform != nullptr)
			{
				this->RemoveFromRootArray(pUIObject);
			}
		}
		else
		{
			if (pNewParentTransform == nullptr)
			{
				this->AddToRootArray(pUIObject);
			}
		}
	}

	return true;
}

void UIObjectManager::OnLButtonDown(POINT pt)
{
	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pUIObject->OnLButtonDown();
			m_pObjectPressedByLButton = pUIObject;
			break;
		}
	}
}

void UIObjectManager::OnLButtonUp(POINT pt)
{
	// �����ִ� UI������Ʈ�� ���� ��쿡�� � ó���� ���� �ʿ䰡 ����.
	if (m_pObjectPressedByLButton == nullptr)
		return;

	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	IUIObject* pLBtnUpObject = nullptr;
	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pLBtnUpObject = pUIObject;
			break;
		}
	}

	m_pObjectPressedByLButton->OnLButtonUp();

	if (m_pObjectPressedByLButton == pLBtnUpObject)
		m_pObjectPressedByLButton->OnLClick();

	m_pObjectPressedByLButton = nullptr;
}

void UIObjectManager::OnRButtonDown(POINT pt)
{
	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pUIObject->OnRButtonDown();
			m_pObjectPressedByRButton = pUIObject;
			break;
		}
	}
}

void UIObjectManager::OnRButtonUp(POINT pt)
{
	// �����ִ� UI������Ʈ�� ���� ��쿡�� � ó���� ���� �ʿ䰡 ����.
	if (m_pObjectPressedByRButton == nullptr)
		return;

	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	IUIObject* pRBtnUpObject = nullptr;
	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pRBtnUpObject = pUIObject;
			break;
		}
	}

	m_pObjectPressedByRButton->OnRButtonUp();

	if (m_pObjectPressedByRButton == pRBtnUpObject)
		m_pObjectPressedByRButton->OnRClick();

	m_pObjectPressedByRButton = nullptr;
}

void UIObjectManager::OnMButtonDown(POINT pt)
{
	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pUIObject->OnMButtonDown();
			m_pObjectPressedByMButton = pUIObject;
			break;
		}
	}
}

void UIObjectManager::OnMButtonUp(POINT pt)
{
	// �����ִ� UI������Ʈ�� ���� ��쿡�� � ó���� ���� �ʿ䰡 ����.
	if (m_pObjectPressedByMButton == nullptr)
		return;

	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	IUIObject* pMBtnUpObject = nullptr;
	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pMBtnUpObject = pUIObject;
			break;
		}
	}

	m_pObjectPressedByMButton->OnMButtonUp();

	if (m_pObjectPressedByMButton == pMBtnUpObject)
		m_pObjectPressedByMButton->OnMClick();

	m_pObjectPressedByMButton = nullptr;
}
