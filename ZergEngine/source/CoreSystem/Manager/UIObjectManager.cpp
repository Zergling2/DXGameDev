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

	// 테이블에 등록
	// 빈 자리 검색
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

	// 만약 빈 자리를 찾지 못했을 경우
	if (emptyIndex == std::numeric_limits<uint32_t>::max())
	{
		emptyIndex = tableSize;
		m_table.push_back(nullptr);	// 테이블 공간 확보
	}

	m_table[emptyIndex] = pUIObject;
	pUIObject->m_tableIndex = emptyIndex;

	hUIObject = UIObjectHandle(emptyIndex, pUIObject->GetId());	// 유효한 핸들 준비

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
	// 파괴된 게임오브젝트 제거 도중 비동기 씬 로드 과정에서의 게임오브젝트 생성으로 인해 m_table의 재할당이 일어날 수 있으므로
	// 락 획득이 반드시 필요하다.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (IUIObject* pUIObject : m_destroyed)
	{
		assert(pUIObject->IsDeferred() == false);
		assert(pUIObject->IsOnTheDestroyQueue() == true);

		// 1. Transform 자식 부모 연결 제거
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
			assert(pChildTransform->m_pParentTransform == pTransform);	// 자신과의 연결 확인

			pChildTransform->m_pParentTransform = nullptr;	// 밑에서 곧 delete될 자신을 접근하는 것을 방지 (댕글링포인터 제거)
		}
		// pTransform->m_children.clear();	// 객체 delete시 자동 소멸

		if (pUIObject->IsRootObject())
		{
			RemovePtrFromRootVector(pUIObject);
			pUIObject->OffFlag(UIOBJECT_FLAG::REAL_ROOT);
		}

		if (pUIObject->IsActive())
			RemovePtrFromActiveVector(pUIObject);
		else
			RemovePtrFromInactiveVector(pUIObject);

		// 테이블에서 제거
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
	// 비활성화된 게임오브젝트인지 확인 assert
	assert(!pUIObject->IsActive());
	RemovePtrFromActiveVector(pUIObject);
	AddPtrToInactiveVector(pUIObject);
}

void UIObjectManagerImpl::MoveToActiveVectorFromInactiveVector(IUIObject* pUIObject)
{
	// 활성화된 게임오브젝트인지 확인 assert
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
	* 대부분 UI 구성 시 루트 오브젝트의 개수는 적으므로 루트 포인터들을 선형 구조로 저장 및 검색한다.
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
	assert(vector[activeIndex] == pUIObject);	// 중요!

	// 지우려는 게임오브젝트 포인터가 맨 뒤에 있는것이 아닌경우
	// 마지막에 위치한 포인터와 위치를 바꾼다.
	if (activeIndex != lastIndex)
	{
		std::swap(vector[activeIndex], vector[lastIndex]);
		vector[activeIndex]->m_activeIndex = activeIndex;	// 마지막에 있던 게임오브젝트의 activeIndex를 올바르게 업데이트 해주어야 한다!
	}

	// 소속되어 있던 vector에서 제거
	vector.pop_back();
}
