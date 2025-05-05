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

	// 1. 테이블에 등록
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

	// Direct access vector에 접근
	// 지연되었다가 Register 되는경우 Active 플래그가 0일 수 있으므로
	// 분리해야 한다.
	if (pGameObject->IsActive())
		AddPtrToActiveVector(pGameObject);
	else
		AddPtrToInactiveVector(pGameObject);

	// deferred 플래그 해제 (AddComponent 올바르게 작동 보장)
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
			// 마지막 게임 오브젝트와 위치를 맞바꾼 후
			std::swap(vector[activeIndex], vector[lastIndex]);
			vector[activeIndex]->m_activeIndex = activeIndex;
		}

		// Active 목록에서 제거
		vector.pop_back();

		// 테이블에서 제거
		m_table[pGameObject->m_tableIndex] = nullptr;

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
