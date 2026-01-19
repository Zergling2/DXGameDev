#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

static constexpr size_t HANDLE_TABLE_INIT_SIZE = 128;

IComponentManager::IComponentManager()
	: m_uniqueId(0)
	, m_lock()
	, m_destroyed()
	, m_directAccessGroup()
	, m_emptyHandleTableIndex()
	, m_handleTable(HANDLE_TABLE_INIT_SIZE, nullptr)
{
	m_emptyHandleTableIndex.reserve(HANDLE_TABLE_INIT_SIZE);
	m_lock.Init();
}

void IComponentManager::Init()
{
	m_uniqueId = 0;

	// 가장 밑에 높은 인덱스를 넣어두어 0번 인덱스부터 사용할 수 있도록 한다.
	const size_t handleTableEndIndex = m_handleTable.size() - 1;
	for (size_t i = 0; i < m_emptyHandleTableIndex.size() - 1; ++i)
		m_emptyHandleTableIndex.push_back(static_cast<uint32_t>(handleTableEndIndex - i));
}

void IComponentManager::Shutdown()
{
}

void IComponentManager::RequestDestroy(IComponent* pComponent)
{
	assert(!pComponent->m_pGameObject->IsPending());

	if (!pComponent->IsOnTheDestroyQueue())
		this->AddToDestroyQueue(pComponent);
}

IComponent* IComponentManager::ToPtr(uint32_t tableIndex, uint64_t id) const
{
	assert(tableIndex < m_handleTable.size());
	// 컴포넌트핸들 템플릿을 잘못 사용하면 엉뚱한 컴포넌트 관리자의 테이블에 Query를 할 수 있다. 이 경우 미정의 동작 발생 가능.

	IComponent* pComponent = m_handleTable[tableIndex];
	if (pComponent == nullptr || pComponent->GetId() != id || pComponent->IsOnTheDestroyQueue())
		return nullptr;
	else
		return pComponent;
}

ComponentHandleBase IComponentManager::RegisterToHandleTable(IComponent* pComponent)
{
	assert(pComponent->m_tableIndex == (std::numeric_limits<uint32_t>::max)());
	assert(pComponent->m_groupIndex == (std::numeric_limits<uint32_t>::max)());

	ComponentHandleBase hComponent;

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
		m_handleTable[emptyIndex] = pComponent;
		pComponent->m_tableIndex = emptyIndex;
	}

	hComponent = ComponentHandleBase(pComponent->m_tableIndex, pComponent->GetId());	// 유효한 핸들 준비

	return hComponent;
}

void IComponentManager::AddToDirectAccessGroup(IComponent* pComponent)
{
	m_directAccessGroup.push_back(pComponent);
	pComponent->m_groupIndex = static_cast<uint32_t>(m_directAccessGroup.size() - 1);
}

void IComponentManager::RemoveDestroyedComponents()
{
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (IComponent* pComponent : m_destroyed)
	{
		assert(pComponent->IsOnTheDestroyQueue());

		GameObject* pGameObject = pComponent->m_pGameObject;

		// Step 1. GameObject의 Component List에서 현재 파괴되는 컴포넌트 포인터를 찾아 제거
		// 만약 GameObject가 Destroy Queue에 있는 경우에는 불필요한 작업 (댕글링 포인터가 남아있는 벡터가 파괴되므로.)
		// GameObject도 제거되는게 아닌 경우는 GameObject의 리스트에서 컴포넌트만 찾아서 제거해야 함
		if (!pGameObject->IsOnTheDestroyQueue())
		{
			// GameObject의 컴포넌트 리스트에서 제거
			auto& componentList = pGameObject->m_components;

#if defined(DEBUG) || defined(_DEBUG)
			bool found = false;
#endif
			const auto end = componentList.cend();
			auto iter = componentList.cbegin();
			while (iter != end)
			{
				if (pComponent == *iter)
				{
#if defined(DEBUG) || defined(_DEBUG)
					found = true;
#endif
					componentList.erase(iter);
					break;
				}
				++iter;
			}

			assert(found == true);
		}

		// Step 2. Enabled group에서 현재 파괴되는 컴포넌트 포인터를 제거
		assert(pComponent->m_groupIndex != (std::numeric_limits<uint32_t>::max)());
		assert(pComponent->m_groupIndex + 1 <= m_directAccessGroup.size());
		assert(m_directAccessGroup[pComponent->m_groupIndex] == pComponent);

		const size_t endIndex = m_directAccessGroup.size() - 1;
		if (pComponent->m_groupIndex != endIndex)	// 제거될 객체가 중간에 있는 경우 한 칸씩 밀지 않기 위해서 끝에 있는 요소와 스왑
		{
			IComponent* pEndItem = m_directAccessGroup[endIndex];

			std::swap(m_directAccessGroup[pComponent->m_groupIndex], m_directAccessGroup[pEndItem->m_groupIndex]);
			std::swap(pComponent->m_groupIndex, pEndItem->m_groupIndex);
		}

		// pComponent->m_groupIndex = (std::numeric_limits<uint32_t>::max)();
		m_directAccessGroup.pop_back();

		// Step 3. 테이블에서 제거
		assert(m_handleTable[pComponent->m_tableIndex] == pComponent);
		m_handleTable[pComponent->m_tableIndex] = nullptr;
		m_emptyHandleTableIndex.push_back(pComponent->m_tableIndex);		// 파괴되는 오브젝트가 사용하던 핸들 테이블 인덱스를 다시 재사용

		delete pComponent;
	}

	m_destroyed.clear();	// 파괴된 컴포넌트 포인터 목록 제거
}

void IComponentManager::AddToDestroyQueue(IComponent* pComponent)
{
	assert(!pComponent->IsOnTheDestroyQueue());

	pComponent->OnFlag(ComponentFlag::OnDestroyQueue);		// 중복 삽입 방지!
	m_destroyed.push_back(pComponent);
}
