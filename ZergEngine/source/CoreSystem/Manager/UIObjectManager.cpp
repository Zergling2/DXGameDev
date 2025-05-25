#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObjectInterface.h>

namespace ze
{
	UIObjectManagerImpl UIObjectManager;
}

using namespace ze;

UIObjectManagerImpl::UIObjectManagerImpl()
	: m_uniqueId(0)
	, m_lock()
	, m_destroyed()
	, m_rootUIObjects()
	, m_activeUIObjects()
	, m_inactiveUIObjects()
	, m_table(256, nullptr)
{
	m_lock.Init();
}

UIObjectManagerImpl::~UIObjectManagerImpl()
{
}

void UIObjectManagerImpl::Init(void* pDesc)
{
	m_uniqueId = 0;
}

void UIObjectManagerImpl::Release()
{
	assert(m_activeUIObjects.size() == 0);
	assert(m_inactiveUIObjects.size() == 0);

	m_table.clear();
	m_uniqueId = 0;
}

UIObjectHandle UIObjectManagerImpl::RegisterToHandleTable(IUIObject* pUIObject)
{
	UIObjectHandle hUIObject;

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

	m_table[emptyIndex] = pUIObject;
	pUIObject->m_tableIndex = emptyIndex;

	hUIObject = UIObjectHandle(emptyIndex, pUIObject->GetId());	// ��ȿ�� �ڵ� �غ�

	return hUIObject;
}

void UIObjectManagerImpl::AddToDestroyQueue(IUIObject* pUIObject)
{
	assert(!pUIObject->IsOnTheDestroyQueue());

	pUIObject->OnFlag(UIOBJECT_FLAG::ON_DESTROY_QUEUE);
	m_destroyed.push_back(pUIObject);
}

UIObjectHandle UIObjectManagerImpl::FindUIObject(PCWSTR name)
{
	UIObjectHandle hUIObject;	// null handle

	for (IUIObject* pUIObject : m_activeUIObjects)
	{
		if (wcscmp(pUIObject->GetName(), name) == 0)
		{
			hUIObject = pUIObject->ToHandleBase();
			break;
		}
	}

	return hUIObject;
}

void UIObjectManagerImpl::RemoveDestroyedUIObjects()
{
	// �ı��� ���ӿ�����Ʈ ���� ���� �񵿱� �� �ε� ���������� ���ӿ�����Ʈ �������� ���� m_table�� ���Ҵ��� �Ͼ �� �����Ƿ�
	// �� ȹ���� �ݵ�� �ʿ��ϴ�.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (IUIObject* pUIObject : m_destroyed)
	{
		assert(pUIObject->IsDeferred() == false);
		assert(pUIObject->IsOnTheDestroyQueue() == true);

		// 1. Transform �ڽ� �θ� ���� ����
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

			pChildTransform->m_pParentTransform = nullptr;	// �ؿ��� �� delete�� �ڽ��� �����ϴ� ���� ���� (��۸������� ����)
		}
		// pTransform->m_children.clear();	// ��ü delete�� �ڵ� �Ҹ�

		if (pUIObject->IsRootObject())
		{
			RemovePtrFromRootVector(pUIObject);
			pUIObject->OffFlag(UIOBJECT_FLAG::REAL_ROOT);
		}

		if (pUIObject->IsActive())
			RemovePtrFromActiveVector(pUIObject);
		else
			RemovePtrFromInactiveVector(pUIObject);

		// ���̺��� ����
		m_table[pUIObject->m_tableIndex] = nullptr;

		// for debugging...
		pUIObject->m_activeIndex = std::numeric_limits<uint32_t>::max();
		pUIObject->m_tableIndex = std::numeric_limits<uint32_t>::max();

		delete pUIObject;
	}

	m_destroyed.clear();
}

void UIObjectManagerImpl::MoveToInactiveVectorFromActiveVector(IUIObject* pUIObject)
{
	// ��Ȱ��ȭ�� ���ӿ�����Ʈ���� Ȯ�� assert
	assert(!pUIObject->IsActive());
	RemovePtrFromActiveVector(pUIObject);
	AddPtrToInactiveVector(pUIObject);
}

void UIObjectManagerImpl::MoveToActiveVectorFromInactiveVector(IUIObject* pUIObject)
{
	// Ȱ��ȭ�� ���ӿ�����Ʈ���� Ȯ�� assert
	assert(pUIObject->IsActive());
	RemovePtrFromInactiveVector(pUIObject);
	AddPtrToActiveVector(pUIObject);
}

void UIObjectManagerImpl::AddPtrToRootVector(IUIObject* pUIObject)
{
	m_rootUIObjects.push_back(pUIObject);
}

void UIObjectManagerImpl::RemovePtrFromRootVector(IUIObject* pUIObject)
{
	/*
	* ��κ� UI ���� �� ��Ʈ ������Ʈ�� ������ �����Ƿ� ��Ʈ �����͵��� ���� ������ ���� �� �˻��Ѵ�.
	*/

	assert(m_rootUIObjects.size() > 0);

#if defined (DEBUG) || (_DEBUG)
	bool find = false;
#endif
	auto end = m_rootUIObjects.cend();
	auto iter = m_rootUIObjects.cbegin();
	while (iter != end)
	{
		if (*iter == pUIObject)
		{
#if defined (DEBUG) || (_DEBUG)
			find = true;
#endif
			m_rootUIObjects.erase(iter);
			break;
		}
		++iter;
	}

	assert(find == true);
}

void UIObjectManagerImpl::AddPtrToVector(std::vector<IUIObject*>& vector, IUIObject* pUIObject)
{
	vector.push_back(pUIObject);
	pUIObject->m_activeIndex = static_cast<uint32_t>(vector.size() - 1);
}

void UIObjectManagerImpl::RemovePtrFromVector(std::vector<IUIObject*>& vector, IUIObject* pUIObject)
{
	uint32_t vectorSize = static_cast<uint32_t>(vector.size());
	assert(vectorSize > 0);

	const uint32_t activeIndex = pUIObject->m_activeIndex;
	const uint32_t lastIndex = vectorSize - 1;

	assert(activeIndex < vectorSize);
	// assert(activeIndex != std::numeric_limits<uint32_t>::max());
	assert(vector[activeIndex] == pUIObject);	// �߿�!

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
