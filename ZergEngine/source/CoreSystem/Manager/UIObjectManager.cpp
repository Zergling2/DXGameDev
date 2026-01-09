#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\InputField.h>
#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>

using namespace ze;

UIObjectManager* UIObjectManager::s_pInstance = nullptr;

static constexpr size_t HANDLE_TABLE_INIT_SIZE = 256;

UIObjectManager::UIObjectManager()
	: m_uniqueId(0)
	, m_lock()
	, m_destroyed()
	, m_roots()
	, m_activeGroup()
	, m_inactiveGroup()
	, m_emptyHandleTableIndex()
	, m_handleTable(HANDLE_TABLE_INIT_SIZE, nullptr)
	, m_pLButtonPressedObject(nullptr)
	, m_pMButtonPressedObject(nullptr)
	, m_pRButtonPressedObject(nullptr)
	, m_pFocusedUIObject(nullptr)
{
	m_emptyHandleTableIndex.reserve(HANDLE_TABLE_INIT_SIZE);
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

	// 가장 밑에 높은 인덱스를 넣어두어 0번 인덱스부터 사용할 수 있도록 한다.
	const size_t handleTableEndIndex = m_handleTable.size() - 1;
	for (size_t i = 0; i < m_emptyHandleTableIndex.size() - 1; ++i)
		m_emptyHandleTableIndex.push_back(static_cast<uint32_t>(handleTableEndIndex - i));
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

	if (pUIObject->m_transform.GetParent() == nullptr)
		this->AddToRootArray(pUIObject);

	pUIObject->IsActive() ? this->DeployToActiveGroup(pUIObject) : this->DeployToInactiveGroup(pUIObject);
}

void UIObjectManager::RequestDestroy(IUIObject* pUIObject)
{
	// 자식 오브젝트까지 Destroy
	for (RectTransform* pChildTransform : pUIObject->m_transform.m_children)
		pChildTransform->m_pUIObject->Destroy();

	if (!pUIObject->IsOnTheDestroyQueue())
		this->AddToDestroyQueue(pUIObject);
}

IUIObject* UIObjectManager::ToPtr(uint32_t tableIndex, uint64_t id) const
{
	// ※ 비동기 씬 로드 지원 시 고려해야 할 사항
	// 가변 길이 테이블을 지원하려면 ToPtr에서 락을 걸어야 한다.
	// (비동기 로드 씬으로 인해서 ToPtr 도중에 핸들 테이블의 주소가 바뀌어버릴 수 있으므로.)
	// 고정 길이 테이블을 사용하면 락을 걸지 않아도 되나, 런타임에 오브젝트의 개수가 제한된다는 점이 아쉬워진다.

	// 현재는 Thread unsafe 상태.

	assert(tableIndex < UIObjectManager::GetInstance()->m_handleTable.size());

	IUIObject* pUIObject = UIObjectManager::GetInstance()->m_handleTable[tableIndex];

	if (pUIObject == nullptr || pUIObject->GetId() != id || pUIObject->IsOnTheDestroyQueue())
		return nullptr;
	else
		return pUIObject;
}

UIObjectHandle UIObjectManager::RegisterToHandleTable(IUIObject* pUIObject)
{
	assert(pUIObject->m_tableIndex == (std::numeric_limits<uint32_t>::max)());
	assert(pUIObject->m_groupIndex == (std::numeric_limits<uint32_t>::max)());

	UIObjectHandle hUIObject;

	{
		// Auto Exclusive Lock
		AutoAcquireSlimRWLockExclusive autolock(m_lock);

		// 핸들 테이블의 빈 자리를 검색
		if (m_emptyHandleTableIndex.empty())	// 핸들 테이블에 빈 공간이 없는 경우
		{
			const size_t newHandleTableBeginIndex = m_handleTable.size();
			m_handleTable.push_back(nullptr);	// 핸들 테이블에 빈 공간 추가
			const size_t newHandleTableEndIndex = m_handleTable.size() - 1;

			for (size_t i = newHandleTableBeginIndex; i <= newHandleTableEndIndex; ++i)
				m_emptyHandleTableIndex.push_back(static_cast<uint32_t>(i));
		}

		assert(m_emptyHandleTableIndex.empty() == false);

		const uint32_t emptyIndex = m_emptyHandleTableIndex.back();
		m_emptyHandleTableIndex.pop_back();

		// 핸들 테이블 사용
		m_handleTable[emptyIndex] = pUIObject;
		pUIObject->m_tableIndex = emptyIndex;
	}

	hUIObject = UIObjectHandle(pUIObject->m_tableIndex, pUIObject->GetId());	// 유효한 핸들 준비

	return hUIObject;
}

void UIObjectManager::AddToDestroyQueue(IUIObject* pUIObject)
{
	assert(!pUIObject->IsOnTheDestroyQueue());

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
			hUIObject = pUIObject->ToHandle();
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
	// 0. UI 상호작용 중(클릭되거나 포커싱된 경우)이었다면 해제
	this->DetachUIFromManager(pUIObject);

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
	assert(pUIObject->m_transform.GetParent() == nullptr);
	assert(pUIObject->IsRoot() == false);

	m_roots.push_back(pUIObject);
	pUIObject->OnFlag(UIOBJECT_FLAG::REAL_ROOT);	// 투입 시 REAL_ROOT 플래그를 켠다.
}

void UIObjectManager::DeployToActiveGroup(IUIObject* pUIObject)
{
	assert(pUIObject->IsActive());

	AddToActiveGroup(pUIObject);
}

void UIObjectManager::DeployToInactiveGroup(IUIObject* pUIObject)
{
	assert(!pUIObject->IsActive());

	AddToInactiveGroup(pUIObject);
}

void UIObjectManager::AddToActiveGroup(IUIObject* pUIObject)
{
	m_activeGroup.push_back(pUIObject);
	pUIObject->m_groupIndex = static_cast<uint32_t>(m_activeGroup.size() - 1);

	pUIObject->OnFlag(UIOBJECT_FLAG::ACTIVE);
}

void UIObjectManager::AddToInactiveGroup(IUIObject* pUIObject)
{
	m_inactiveGroup.push_back(pUIObject);
	pUIObject->m_groupIndex = static_cast<uint32_t>(m_inactiveGroup.size() - 1);

	pUIObject->OffFlag(UIOBJECT_FLAG::ACTIVE);
}

void UIObjectManager::RemoveFromRootArray(IUIObject* pUIObject)
{
	/*
	대부분 UI 구성 시 루트 오브젝트의 개수는 적으므로 루트 포인터들을 선형 구조로 저장 및 검색한다.
	*/
	assert(pUIObject->m_transform.GetParent() == nullptr);
	assert(m_roots.size() > 0);
	assert(pUIObject->IsRoot() == true);

#if defined (DEBUG) || (_DEBUG)
	bool found = false;
#endif
	auto end = m_roots.cend();
	auto iter = m_roots.cbegin();
	while (iter != end)
	{
		if (*iter == pUIObject)
		{
#if defined (DEBUG) || (_DEBUG)
			found = true;
#endif
			m_roots.erase(iter);
			break;
		}

		++iter;
	}

	assert(found == true);

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
		assert(pUIObject->IsPending() == false);	// 로딩 씬 소속의 오브젝트는 파괴될 수 없다.
		assert(pUIObject->IsOnTheDestroyQueue() == true);	// 파괴 큐에 들어온 경우에는 이 ON_DESTROY_QUEUE 플래그가 켜져 있어야만 한다.

		// UI 오브젝트별 파괴될 때 수행되어야 하는 작업 수행 (RadioButton의 경우 그룹에서 포인터 제거)
		pUIObject->OnDestroy();

		// Detach from UI system
		this->DetachUIFromManager(pUIObject);
		 
		
		// Step 1. Transform 자식 부모 연결 제거
		RectTransform* pTransform = &pUIObject->m_transform;
		RectTransform* pParentTransform = pTransform->GetParent();
		if (pParentTransform != nullptr)
		{
#if defined(DEBUG) || defined(_DEBUG)
			bool found = false;
#endif
			std::vector<RectTransform*>::const_iterator iter = pParentTransform->m_children.cbegin();
			while (iter != pParentTransform->m_children.cend())
			{
				if (*iter == pTransform)
				{
#if defined(DEBUG) || defined(_DEBUG)
					found = true;
#endif
					pParentTransform->m_children.erase(iter);
					break;
				}
				++iter;
			}
			assert(found == true);
		}

		for (RectTransform* pChildTransform : pTransform->m_children)
		{
			assert(pChildTransform->m_pParent == pTransform);	// 자신과의 연결 확인

			// [중요!]
			// 파괴가 부모 자식 계층 구조대로 진행되지 않으므로 댕글링 포인터가 생기지 않게 유의해야 한다.
			// 
			// 자식들이 delete될 자신의 댕글링 포인터를 갖고 있지 않도록 m_pParent를 nullptr로 설정해준다.
			// 그런데 루트 오브젝트들은 파괴될 때 RootGroup내에서 포인터가 제거되어야 하는데, 이렇게 자식의 m_pParent를 nullptr로 설정하면
			// RootGroup에서 제거되어야 하는 '루트 객체'였는지를 'm_pParent == nullptr'로는 판단할 수 없게 된다.
			//
			// 이 문제는 런타임이 객체를 Deploy하며 만약 루트 객체인 경우 UIOBJECT_FLAG::REAL_ROOT 플래그를 표시하여 이 문제를 해결한다.
			// SetParent 등으로 루트가 아니었던 오브젝트가 루트로 되는 경우 혹은 그 반대의 경우에 REAL_ROOT 플래그 역시 알맞게 업데이트된다.

			pChildTransform->m_pParent = nullptr;	// 중요
		}
		// pTransform->m_children.clear();	// 객체 delete시 자동 소멸

		if (pUIObject->IsRoot())	// REAL_ROOT 플래그 체크
			this->RemoveFromRootArray(pUIObject);

		this->RemoveFromGroup(pUIObject);

		// 핸들 테이블에서 제거
		assert(m_handleTable[pUIObject->m_tableIndex] == pUIObject);
		m_handleTable[pUIObject->m_tableIndex] = nullptr;
		m_emptyHandleTableIndex.push_back(pUIObject->m_tableIndex);		// 파괴되는 오브젝트가 사용하던 핸들 테이블 인덱스를 다시 재사용

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
		else
			pUIObject->OnFlag(UIOBJECT_FLAG::ACTIVE);
	}
	else
	{
		// PENDING 상태가 아닌 경우에만 포인터 이동 (PENDING 상태에서는 Active/Inactive 벡터에 포인터가 존재하지 않는다.)
		if (!pUIObject->IsPending())
			this->MoveToInactiveGroup(pUIObject);
		else
			pUIObject->OffFlag(UIOBJECT_FLAG::ACTIVE);
	}
}

bool UIObjectManager::SetParent(RectTransform* pTransform, RectTransform* pNewParentTransform)
{
	IUIObject* pUIObject = pTransform->m_pUIObject;
	RectTransform* pOldParentTransform = pTransform->GetParent();

	if (pUIObject->IsOnTheDestroyQueue())
		return false;

	// 자기 자신을 부모로 설정하려고 하거나 설정하려는 부모가 이미 부모인 경우에는 실패
	if (pTransform == pNewParentTransform || pOldParentTransform == pNewParentTransform)
		return false;

	// 사이클 검사
	// pTransform이 이미 나를 조상으로 하고 있는 경우 or 파괴 예정인 오브젝트를 부모로 설정하려는 경우
	// 예외처리 해주지 않으면 자식 오브젝트들까지 모두 Destroy 큐에 넣어주는 정책과 일관성이 맞지 않는다.
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
		bool found = false;
#endif
		std::vector<RectTransform*>::const_iterator iter = pOldParentTransform->m_children.cbegin();
		while (iter != pOldParentTransform->m_children.cend())
		{
			if (*iter == pTransform)
			{
				pOldParentTransform->m_children.erase(iter);
#if defined(DEBUG) || defined(_DEBUG)
				found = true;
#endif
				break;
			}
			++iter;
		}
		assert(found == true);	// 자식으로 존재했었어야 함
	}

	// 만약 새 부모가 nullptr이 아니라면
	if (pNewParentTransform != nullptr)
	{
		// 부모의 자식 목록을 업데이트
		pNewParentTransform->m_children.push_back(pTransform);

		// 부모가 비활성 상태이면 자식도 비활성화
		if (pNewParentTransform->m_pUIObject->IsActive() == false)
			pTransform->m_pUIObject->SetActive(false);
	}

	// 멤버 부모 포인터 업데이트
	pTransform->m_pParent = pNewParentTransform;

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

IUIObject* XM_CALLCONV UIObjectManager::SearchForHitUI(POINT pt) const
{
	// UI 오브젝트를 후위 순회하며 검색

	IUIObject* pHitUI = nullptr;

	for (IUIObject* pUIObject : m_roots)
	{
		pHitUI = UIObjectManager::PostOrderHitTest(pt, pUIObject);
		if (pHitUI != nullptr)
			break;
	}

	return pHitUI;
}

IUIObject* XM_CALLCONV UIObjectManager::PostOrderHitTest(POINT pt, const IUIObject* pUIObject)
{
	if (!pUIObject->IsActive())
		return nullptr;

	const std::vector<RectTransform*>& children = pUIObject->m_transform.m_children;

	for (auto it = children.rbegin(); it != children.rend(); ++it)
	{
		const IUIObject* pChild = (*it)->m_pUIObject;
		IUIObject* pHitUI = UIObjectManager::PostOrderHitTest(pt, pChild);
		if (pHitUI)	// Hit UI가 있다면 반환
			return pHitUI;
	}

	// 자식 UI들 중에서 Hit UI가 없다면 자신을 검사
	if (pUIObject->HitTest(pt))
		return const_cast<IUIObject*>(pUIObject);
	else
		return nullptr;
}

void UIObjectManager::OnChar(WPARAM wParam, LPARAM lParam)
{
	if (!this->GetFocusedUI())
		return;

	this->GetFocusedUI()->OnChar(static_cast<TCHAR>(wParam));
}

void UIObjectManager::OnMouseMove(UINT flags, POINT pt)
{
	if (m_pLButtonPressedObject)
		m_pLButtonPressedObject->OnMouseMove(pt);

	if (m_pMButtonPressedObject)
		m_pMButtonPressedObject->OnMouseMove(pt);

	if (m_pRButtonPressedObject)
		m_pRButtonPressedObject->OnMouseMove(pt);
}

void UIObjectManager::OnLButtonDown(POINT pt)
{
	this->SetFocusedUI(nullptr);

	if (m_pLButtonPressedObject)
		m_pLButtonPressedObject->OnLButtonUp(pt);

	IUIObject* pHitUI = this->SearchForHitUI(pt);
	if (pHitUI)
	{
		m_pLButtonPressedObject = pHitUI;
		pHitUI->OnLButtonDown(pt);
	}
	else
		m_pLButtonPressedObject = nullptr;
}

void UIObjectManager::OnLButtonUp(POINT pt)
{
	if (m_pLButtonPressedObject == nullptr)	// Pressed UI가 없는 경우 검사 생략 가능
		return;

	IUIObject* const pLButtonPressedObject = m_pLButtonPressedObject;
	m_pLButtonPressedObject = nullptr;
	// Button Up은 무조건 Pressed Object에 대해서 호출
	pLButtonPressedObject->OnLButtonUp(pt);

	IUIObject* pHitUI = this->SearchForHitUI(pt);

	if (pHitUI == pLButtonPressedObject)
		pHitUI->OnLButtonClick(pt);
}

void UIObjectManager::OnMButtonDown(POINT pt)
{
	this->SetFocusedUI(nullptr);

	if (m_pMButtonPressedObject)
		m_pMButtonPressedObject->OnMButtonUp(pt);

	IUIObject* pHitUI = this->SearchForHitUI(pt);
	if (pHitUI)
	{
		m_pMButtonPressedObject = pHitUI;
		pHitUI->OnMButtonDown(pt);
	}
	else
		m_pMButtonPressedObject = nullptr;
}

void UIObjectManager::OnMButtonUp(POINT pt)
{
	if (m_pMButtonPressedObject == nullptr)	// Pressed UI가 없는 경우 검사 생략 가능
		return;

	IUIObject* const pMButtonPressedObject = m_pMButtonPressedObject;
	m_pMButtonPressedObject = nullptr;
	// Button Up은 무조건 Pressed Object에 대해서 호출
	pMButtonPressedObject->OnMButtonUp(pt);

	IUIObject* pHitUI = this->SearchForHitUI(pt);

	if (pHitUI == pMButtonPressedObject)
		pHitUI->OnMButtonClick(pt);
}

void UIObjectManager::OnRButtonDown(POINT pt)
{
	this->SetFocusedUI(nullptr);

	if (m_pRButtonPressedObject)
		m_pRButtonPressedObject->OnRButtonUp(pt);

	IUIObject* pHitUI = this->SearchForHitUI(pt);
	if (pHitUI)
	{
		m_pRButtonPressedObject = pHitUI;
		pHitUI->OnRButtonDown(pt);
	}
	else
		m_pRButtonPressedObject = nullptr;
}

void UIObjectManager::OnRButtonUp(POINT pt)
{
	if (m_pRButtonPressedObject == nullptr)	// Pressed UI가 없는 경우 검사 생략 가능
		return;

	IUIObject* const pRButtonPressedObject = m_pRButtonPressedObject;
	m_pRButtonPressedObject = nullptr;
	// Button Up은 무조건 Pressed Object에 대해서 호출
	pRButtonPressedObject->OnRButtonUp(pt);

	IUIObject* pHitUI = this->SearchForHitUI(pt);

	if (pHitUI == pRButtonPressedObject)
		pHitUI->OnRButtonClick(pt);
}

void UIObjectManager::DetachUIFromManager(IUIObject* pUIObject)
{
	// Detach from UI system

	// dummy data
	POINT pt;
	pt.x = 0;
	pt.y = 0;

	if (m_pFocusedUIObject == pUIObject)
	{
		m_pFocusedUIObject = nullptr;
	}

	if (m_pLButtonPressedObject == pUIObject)
	{
		m_pLButtonPressedObject = nullptr;
		pUIObject->OnLButtonUp(pt);
	}

	if (m_pMButtonPressedObject == pUIObject)
	{
		m_pMButtonPressedObject = nullptr;
		pUIObject->OnMButtonUp(pt);
	}

	if (m_pRButtonPressedObject == pUIObject)
	{
		m_pRButtonPressedObject = nullptr;
		pUIObject->OnRButtonUp(pt);
	}
}
