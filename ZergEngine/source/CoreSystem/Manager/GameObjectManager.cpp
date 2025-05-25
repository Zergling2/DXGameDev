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

	m_table[emptyIndex] = pGameObject;
	pGameObject->m_tableIndex = emptyIndex;

	hGameObject = GameObjectHandle(emptyIndex, pGameObject->GetId());	// 유효한 핸들 준비

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
	// 파괴된 게임오브젝트 제거 도중 비동기 씬 로드 과정에서의 게임오브젝트 생성으로 인해 m_table의 재할당이 일어날 수 있으므로
	// 락 획득이 반드시 필요하다.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (GameObject* pGameObject : m_destroyed)
	{
		assert(pGameObject->IsDeferred() == false);
		assert(pGameObject->IsOnTheDestroyQueue() == true);

		// 1. Transform 자식 부모 연결 제거
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
			pChild->m_pParentTransform = nullptr;	// 밑에서 곧 delete될 자신을 접근하는 것을 방지 (댕글링포인터 제거)
		}
		// pTransform->m_children.clear();	// 객체 delete시 자동 소멸

		if (pGameObject->IsActive())
			RemovePtrFromActiveVector(pGameObject);
		else
			RemovePtrFromInactiveVector(pGameObject);

		// 테이블에서 제거
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
	// 비활성화된 게임오브젝트인지 확인 assert
	assert(pGameObject->IsActive() == false);
	RemovePtrFromActiveVector(pGameObject);
	AddPtrToInactiveVector(pGameObject);
}

void GameObjectManagerImpl::MoveToActiveVectorFromInactiveVector(GameObject* pGameObject)
{
	// 활성화된 게임오브젝트인지 확인 assert
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
	assert(vector[activeIndex] == pGameObject);	// 중요!

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
