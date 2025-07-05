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
	// 런타임 클래스에서 활성/비활성 오브젝트들을 모두 해제했어야 하므로
	assert(m_activeGroup.size() == 0);
	assert(m_inactiveGroup.size() == 0);

	m_handleTable.clear();
	m_uniqueId = 0;
}

void UIObjectManager::Deploy(IUIObject* pUIObject)
{
	pUIObject->OffFlag(UIOBJECT_FLAG::PENDING);	// 중요!

	if (pUIObject->m_transform.m_pParentTransform == nullptr)
		this->AddToRootArray(pUIObject);

	pUIObject->IsActive() ? this->DeployToActiveGroup(pUIObject) : this->DeployToInactiveGroup(pUIObject);
}

void UIObjectManager::RequestDestroy(IUIObject* pUIObject)
{
	// 지연된 오브젝트를 제거하는 경우는 OnLoadScene에서 Destroy를 한다는 의미인데 이것은 허용하지 않는다.
	if (pUIObject->IsPending())
		return;

	// 자식 오브젝트까지 Destroy
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

	// 테이블에 등록
	// 빈 자리 검색
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

	// 만약 빈 자리를 찾지 못했을 경우
	if (!find)
	{
		emptyIndex = tableSize;
		m_handleTable.push_back(nullptr);	// 테이블 공간 확보
	}

	m_handleTable[emptyIndex] = pUIObject;
	pUIObject->m_tableIndex = emptyIndex;

	hUIObject = UIObjectHandle(emptyIndex, pUIObject->GetId());	// 유효한 핸들 준비

	return hUIObject;
}

void UIObjectManager::AddToDestroyQueue(IUIObject* pUIObject)
{
	if (pUIObject->IsOnTheDestroyQueue())
		return;

	pUIObject->OnFlag(UIOBJECT_FLAG::ON_DESTROY_QUEUE);		// 매우 중요! (벡터에 중복 삽입으로 인한 중복 delete 힙 손상 방지)
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
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 1. Inactive group에서 제거
	assert(!pUIObject->IsActive());	// Inactive 상태였어야 한다.
	this->RemoveFromGroup(pUIObject);
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━


	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 2. Active group에 추가
	this->AddToActiveGroup(pUIObject);
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
}

void UIObjectManager::MoveToInactiveGroup(IUIObject* pUIObject)
{
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 1. Active group에서 제거
	assert(pUIObject->IsActive());	// Active 상태였어야 한다.
	this->RemoveFromGroup(pUIObject);
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━


	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 2. Inactive group에 추가
	this->AddToInactiveGroup(pUIObject);
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
}

void UIObjectManager::AddToRootArray(IUIObject* pUIObject)
{
	assert(pUIObject->m_transform.m_pParentTransform == nullptr);

	m_roots.push_back(pUIObject);
	pUIObject->OnFlag(UIOBJECT_FLAG::REAL_ROOT);	// 투입 시 REAL_ROOT 플래그를 켠다.
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
	assert(pUIObject->IsActive());	// 이 함수는 씬에서 로드된 오브젝트가 전역 관리자로 투입될 때 호출되므로 ACTIVE 플래그가 켜져있어야 한다.

	m_activeGroup.push_back(pUIObject);
	pUIObject->m_groupIndex = static_cast<uint32_t>(m_activeGroup.size() - 1);

	pUIObject->OnFlag(UIOBJECT_FLAG::ACTIVE);
}

void UIObjectManager::AddToInactiveGroup(IUIObject* pUIObject)
{
	assert(!pUIObject->IsActive());	// 이 함수는 씬에서 로드된 오브젝트가 전역 관리자로 투입될 때 호출되므로 ACTIVE 플래그가 꺼져있어야 한다.

	m_inactiveGroup.push_back(pUIObject);
	pUIObject->m_groupIndex = static_cast<uint32_t>(m_inactiveGroup.size() - 1);

	pUIObject->OffFlag(UIOBJECT_FLAG::ACTIVE);
}

void UIObjectManager::RemoveFromRootArray(IUIObject* pUIObject)
{
	/*
	대부분 UI 구성 시 루트 오브젝트의 개수는 적으므로 루트 포인터들을 선형 구조로 저장 및 검색한다.
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
	assert(group[groupIndex] == pUIObject);	// 중요!

	// 지우려는 오브젝트 포인터가 맨 뒤에 있는것이 아닌경우
	// 마지막에 위치한 포인터와 위치를 바꾼다.
	if (groupIndex != lastIndex)
	{
		std::swap(group[groupIndex], group[lastIndex]);
		group[groupIndex]->m_groupIndex = groupIndex;	// 마지막에 있던 오브젝트의 groupIndex를 올바르게 업데이트 해주어야 함.
	}

	// 소속되어 있던 vector에서 제거
	group.pop_back();
}

void UIObjectManager::RemoveDestroyedUIObjects()
{
	// 파괴된 게임오브젝트 제거 도중 비동기 씬 로드 과정에서의 게임오브젝트 생성으로 인해 m_table의 재할당이 일어날 수 있으므로
	// 락 획득이 반드시 필요하다.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (IUIObject* pUIObject : m_destroyed)
	{
		/* 검증 */
		assert(pUIObject->IsPending() == false);	// 로딩 씬 소속의 오브젝트는 파괴될 수 없다.
		assert(pUIObject->IsOnTheDestroyQueue() == true);	// 파괴 큐에 들어온 경우에는 이 ON_DESTROY_QUEUE 플래그가 켜져 있어야만 한다.

		if (pUIObject == m_pObjectPressedByLButton)
			m_pObjectPressedByLButton = nullptr;

		// Step 1. Transform 자식 부모 연결 제거
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


			// 중요 포인트
			pChildTransform->m_pParentTransform = nullptr;	// 밑에서 곧 delete되는 자신을 접근(Step 1에서 접근함)하는 것을 방지 (잠재적 댕글링 포인터 제거)
			// 이로 인해서 객체 파괴 시에는 루트 오브젝트였는지 판별(RootGroup에서 포인터 제거해야 함)을 할 때 부모 Transform 포인터로
			// 판단할 수 없다. 따라서 진짜 루트 노드였는지를 체크해놓기 위해 UIOBJECT_FLAG열거형으로 REAL_ROOT라는 플래그를 사용한다.
		}
		// pTransform->m_children.clear();	// 객체 delete시 자동 소멸

		if (pUIObject->IsRoot())
			this->RemoveFromRootArray(pUIObject);

		this->RemoveFromGroup(pUIObject);

		// 핸들 테이블에서 제거
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
	// 이미 해당 활성 상태가 설정되어 있는 경우 함수 리턴
	if (pUIObject->IsActive() == active)
		return;

	if (active)
	{
		// PENDING 상태가 아닌 경우에만 포인터 이동 (PENDING 상태에서는 Active/Inactive 벡터에 포인터가 존재하지 않는다.)
		if (!pUIObject->IsPending())
			this->MoveToActiveGroup(pUIObject);
	}
	else
	{
		// PENDING 상태가 아닌 경우에만 포인터 이동 (PENDING 상태에서는 Active/Inactive 벡터에 포인터가 존재하지 않는다.)
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

	// 자기 자신을 부모로 설정하려고 하거나 이미 설정하려는 부모가 이미 부모인 경우에는 실패
	if (pTransform == pNewParentTransform || pOldParentTransform == pNewParentTransform)
		return false;

	// 사이클 검사
	// pTransform이 이미 나를 조상으로 하고 있는 경우 or 파괴 예정인 오브젝트를 부모로 설정하려는 경우
	// 예외처리 해주지 않으면 Runtime::Destroy()에서 자식 오브젝트들까지 모두 Destroy 큐에 넣어주는 정책과 일관성이 맞지 않는다.
	// (부모는 파괴되는데 자식은 파괴되지 않는 모순 발생)
	if (pNewParentTransform != nullptr)
		if (pNewParentTransform->IsDescendantOf(pTransform) || pNewParentTransform->m_pUIObject->IsOnTheDestroyQueue())
			return false;

	// 루트 노드 처리 경우의 수
	// 1. pOldParentTransform != nullptr && pNewParentTransform == nullptr (기존에 루트 노드가 아니었고 이제 루트 노드가 되려는 경우)
	// 2. pOldParentTransform != nullptr && pNewParentTransform != nullptr (기존에 루트 노드가 아니었고 지금도 루트 노드가 되는 것은 아닌 경우)
	// 3. pOldParentTransform == nullptr && pNewParentTransform == nullptr (기존에 루트 노드였고 지금도 루트 노드가 되려는 경우) (이 경우는 함수 진입 직후 예외처리됨)
	// 4. pOldParentTransform == nullptr && pNewParentTransform != nullptr (기존에 루트 노드였고 이제 루트 노드가 아니게 되는 경우)

	// 부모가 nullptr이 아니었던 경우에는 기존 부모에서 자식 포인터를 찾아서 제거
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
		assert(find == true);	// 자식으로 존재했었어야 함
	}

	// 부모의 자식 목록을 업데이트
	if (pNewParentTransform != nullptr)
		pNewParentTransform->m_children.push_back(pTransform);

	// 부모 포인터를 새로운 부모로 업데이트
	pTransform->m_pParentTransform = pNewParentTransform;


	// 로딩 중이 아닌 오브젝트는 Root 그룹에서 존재할지 여부를 업데이트해야함.
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
	// 눌려있던 UI오브젝트가 없던 경우에는 어떤 처리도 해줄 필요가 없다.
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
	// 눌려있던 UI오브젝트가 없던 경우에는 어떤 처리도 해줄 필요가 없다.
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
	// 눌려있던 UI오브젝트가 없던 경우에는 어떤 처리도 해줄 필요가 없다.
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
