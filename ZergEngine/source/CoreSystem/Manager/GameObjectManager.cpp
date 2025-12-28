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
	, m_emptyHandleTableIndex(HANDLE_TABLE_INIT_SIZE)	// 단편화를 최소화하기 위해서 최초에 핸들 테이블의 사이즈와 동일한 개수만큼 인덱스 버퍼를 할당한다.
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

	// 벡터를 스택처럼 사용할 것이므로 가장 밑에 높은 인덱스를 넣어두어 0번 인덱스부터 사용할 수 있도록 한다.
	const size_t handleTableEndIndex = m_handleTable.size() - 1;
	for (size_t i = 0; i < m_emptyHandleTableIndex.size() - 1; ++i)
		m_emptyHandleTableIndex[i] = static_cast<uint32_t>(handleTableEndIndex - i);
}

void GameObjectManager::UnInit()
{
	// 런타임에서 미리 모두 해제시켰어야 하므로
	assert(m_activeGroup.size() == 0);
	assert(m_inactiveGroup.size() == 0);

	m_handleTable.clear();
}

void GameObjectManager::Deploy(GameObject* pGameObject)
{
	// 중요 (플래그 제거)
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
	// 지연된 오브젝트를 제거하는 경우는 OnLoadScene에서 Destroy를 한다는 의미인데 이것은 허용하지 않는다.
	assert(!pGameObject->IsPending());

	// 자식 오브젝트까지 Destroy
	for (Transform* pChildTransform : pGameObject->m_transform.m_children)
		pChildTransform->m_pGameObject->Destroy();

	// 소유하고 있는 모든 컴포넌트 제거
	for (IComponent* pComponent : pGameObject->m_components)
		pComponent->Destroy();	// 내부에서 곧바로 GameObject <-> Component간 연결 끊으면 안됨 (지금 순회하는 이터레이터 망가짐!) (지연삭제)
	
	if (!pGameObject->IsOnTheDestroyQueue())	// 매우 중요! (중복 삽입으로 인한 중복 delete 힙 손상 방지)
		this->AddToDestroyQueue(pGameObject);
}

GameObject* GameObjectManager::ToPtr(uint32_t tableIndex, uint64_t id) const
{
	// ※ 비동기 씬 로드 지원 시 고려해야 할 사항
	// 가변 길이 테이블을 지원하려면 ToPtr에서 락을 걸어야 한다.
	// (비동기 로드 씬으로 인해서 ToPtr 도중에 핸들 테이블의 주소가 바뀌어버릴 수 있으므로.)
	// 고정 길이 테이블을 사용하면 락을 걸지 않아도 되나, 런타임에 오브젝트의 개수가 제한된다는 점이 아쉬워진다.

	// 현재는 Thread unsafe 상태.

	assert(tableIndex < GameObjectManager::GetInstance()->m_handleTable.size());

	GameObject* pGameObject = GameObjectManager::GetInstance()->m_handleTable[tableIndex];

	if (pGameObject == nullptr || pGameObject->GetId() != id || pGameObject->IsOnTheDestroyQueue())
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

		// 핸들 테이블의 빈 자리를 검색
		uint32_t emptyIndex;
		if (!m_emptyHandleTableIndex.empty())
		{
			emptyIndex = m_emptyHandleTableIndex.back();
			m_emptyHandleTableIndex.pop_back();
		}
		else
		{
			// 만약 핸들 테이블에 빈 공간이 없는 경우
			m_handleTable.push_back(nullptr);	// 핸들 테이블에 빈 공간 추가
			emptyIndex = static_cast<uint32_t>(m_handleTable.size() - 1);
		}

		m_handleTable[emptyIndex] = pGameObject;
		pGameObject->m_tableIndex = emptyIndex;
	}

	hGameObject = GameObjectHandle(pGameObject->m_tableIndex, pGameObject->GetId());	// 유효한 핸들 준비

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
	assert(!pGameObject->IsPending());	// 비-지연 상태였어야 한다.
	assert(!pGameObject->IsActive());	// Inactive 상태였어야 한다.

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 1. Inactive group에서 제거
	this->RemoveFromGroup(pGameObject);
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━


	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 2. Active group에 추가
	this->AddToActiveGroup(pGameObject);
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
}

void GameObjectManager::MoveToInactiveGroup(GameObject* pGameObject)
{
	assert(!pGameObject->IsPending());	// 비-지연 상태였어야 한다.
	assert(pGameObject->IsActive());	// Active 상태였어야 한다.

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 1. Active group에서 제거
	this->RemoveFromGroup(pGameObject);
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━


	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 2. Inactive group에 추가
	this->AddToInactiveGroup(pGameObject);
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
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
	assert(group[groupIndex] == pGameObject);	// 중요!

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

void GameObjectManager::RemoveDestroyedGameObjects()
{
	// 파괴된 게임오브젝트 제거 도중 비동기 씬 로드 과정에서의 게임오브젝트 생성으로 인해 m_table의 재할당이 일어날 수 있으므로
	// 락 획득이 반드시 필요하다.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (GameObject* pGameObject : m_destroyed)
	{
		/* 검증 */
		assert(!pGameObject->IsPending());			// 로딩 씬 소속의 오브젝트는 파괴될 수 없다.
		assert(pGameObject->IsOnTheDestroyQueue());	// 파괴 큐에 들어온 경우에는 이 ON_DESTROY_QUEUE 플래그가 켜져 있어야만 한다.

		// Step 1. Transform 자식 부모 연결 제거
		Transform* pTransform = &pGameObject->m_transform;
		Transform* pParentTransform = pTransform->m_pParent;
		if (pParentTransform != nullptr)
		{
#if defined(DEBUG) || defined(_DEBUG)
			bool found = false;
#endif
			std::vector<Transform*>::const_iterator iter = pParentTransform->m_children.cbegin();
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

		for (Transform* pChildTransform : pTransform->m_children)
		{
			assert(pChildTransform->GetParent() == pTransform);	// 자신과의 연결 확인

			// [중요!]
			// 파괴가 부모 자식 계층 구조대로 진행되지 않으므로 댕글링 포인터가 생기지 않게 유의해야 한다.
			// 
			// 자식들이 delete될 자신의 댕글링 포인터를 갖고 있지 않도록 m_pParent를 nullptr로 설정해준다.
			// 그런데 루트 오브젝트들은 파괴될 때 RootGroup내에서 포인터가 제거되어야 하는데, 이렇게 자식의 m_pParent를 nullptr로 설정하면
			// RootGroup에서 제거되어야 하는 '루트 객체'였는지를 'm_pParent == nullptr'로는 판단할 수 없게 된다.
			//
			// 이 문제는 런타임이 객체를 Deploy하며 만약 루트 객체인 경우 UIOBJECT_FLAG::REAL_ROOT 플래그를 표시하여 이 문제를 해결한다.
			// SetParent 등으로 루트가 아니었던 오브젝트가 루트로 되는 경우 혹은 그 반대의 경우에 REAL_ROOT 플래그 역시 알맞게 업데이트된다.

			pChildTransform->m_pParent = nullptr;
		}
		// pTransform->m_children.clear();	// 객체 delete시 자동 소멸

		this->RemoveFromGroup(pGameObject);

		// 핸들 테이블에서 제거
		assert(m_handleTable[pGameObject->m_tableIndex] == pGameObject);
		m_handleTable[pGameObject->m_tableIndex] = nullptr;
		m_emptyHandleTableIndex.push_back(pGameObject->m_tableIndex);		// 파괴되는 오브젝트가 사용하던 핸들 테이블 인덱스를 다시 재사용

		delete pGameObject;
	}

	m_destroyed.clear();
}

void GameObjectManager::SetActive(GameObject* pGameObject, bool active)
{
	// 이미 해당 활성 상태가 설정되어 있는 경우 함수 리턴
	if (pGameObject->IsActive() == active)
		return;
	
	if (active)
	{
		for (IComponent* pComponent : pGameObject->m_components)
			pComponent->Enable();

		// PENDING 상태가 아닌 경우에만 포인터 이동 (PENDING 상태에서는 Active/Inactive 벡터에 포인터가 존재하지 않는다.)
		if (!pGameObject->IsPending())
			this->MoveToActiveGroup(pGameObject);

		
		pGameObject->OnFlag(GAMEOBJECT_FLAG::ACTIVE);
	}
	else
	{
		for (IComponent* pComponent : pGameObject->m_components)
			pComponent->Disable();

		// PENDING 상태가 아닌 경우에만 포인터 이동 (PENDING 상태에서는 Active/Inactive 벡터에 포인터가 존재하지 않는다.)
		if (!pGameObject->IsPending())
			this->MoveToInactiveGroup(pGameObject);


		pGameObject->OffFlag(GAMEOBJECT_FLAG::ACTIVE);
	}
}

bool GameObjectManager::SetParent(Transform* pTransform, Transform* pNewParentTransform)
{
	GameObject* pGameObject = pTransform->m_pGameObject;
	Transform* pOldParentTransform = pTransform->m_pParent;

	if (pGameObject->IsOnTheDestroyQueue())
		return false;

	// 자기 자신을 부모로 설정하려고 하거나 설정하려는 부모가 이미 부모인 경우에는 실패
	if (pTransform == pNewParentTransform || pOldParentTransform == pNewParentTransform)
		return false;

	// 사이클 검사
	// pTransform이 이미 나를 조상으로 하고 있는 경우 or 파괴 예정인 오브젝트를 부모로 설정하려는 경우
	// 예외처리 해주지 않으면 객체의 Destroy 함수에서 자식 오브젝트들까지 모두 Destroy 큐에 넣어주는 정책과 일관성이 맞지 않는다.
	// (부모는 파괴되는데 자식은 파괴되지 않는 모순 발생)
	if (pNewParentTransform != nullptr)
		if (pNewParentTransform->IsDescendantOf(pTransform) || pNewParentTransform->m_pGameObject->IsOnTheDestroyQueue())
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
		std::vector<Transform*>::const_iterator iter = pOldParentTransform->m_children.cbegin();
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

	// 만약 부모가 nullptr이 아니라면
	if (pNewParentTransform != nullptr)
	{
		// 부모의 자식 목록을 업데이트
		pNewParentTransform->m_children.push_back(pTransform);

		// 부모가 비활성 상태이면 자식도 비활성화
		if (pNewParentTransform->m_pGameObject->IsActive() == false)
			pTransform->m_pGameObject->SetActive(false);
	}

	// 멤버 부모 포인터 업데이트
	pTransform->m_pParent = pNewParentTransform;

	return true;
}
