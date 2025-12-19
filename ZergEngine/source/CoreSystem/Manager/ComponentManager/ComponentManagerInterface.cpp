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
	, m_emptyHandleTableIndex(HANDLE_TABLE_INIT_SIZE)	// 단편화를 최소화하기 위해서 최초에 핸들 테이블의 사이즈와 동일한 개수만큼 인덱스 버퍼를 할당한다.
	, m_handleTable(HANDLE_TABLE_INIT_SIZE, nullptr)
{
	m_lock.Init();
}

void IComponentManager::Init()
{
	m_uniqueId = 0;

	// 벡터를 스택처럼 사용할 것이므로 가장 밑에 높은 인덱스를 넣어두어 0번 인덱스부터 사용할 수 있도록 한다.
	const size_t handleTableEndIndex = m_handleTable.size() - 1;
	for (size_t i = 0; i < m_emptyHandleTableIndex.size() - 1; ++i)
		m_emptyHandleTableIndex[i] = static_cast<uint32_t>(handleTableEndIndex - i);
}

void IComponentManager::UnInit()
{
}

void IComponentManager::Deploy(IComponent* pComponent)
{
	this->AddToDirectAccessGroup(pComponent);
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
	if (pComponent == nullptr || pComponent->GetId() != id)
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
			bool find = false;
#endif
			const auto end = componentList.cend();
			auto iter = componentList.cbegin();
			while (iter != end)
			{
				if (pComponent == *iter)
				{
#if defined(DEBUG) || defined(_DEBUG)
					find = true;
#endif
					componentList.erase(iter);
					break;
				}
				++iter;
			}

			assert(find == true);
		}

		// Step 2. Enabled group에서 현재 파괴되는 컴포넌트 포인터를 제거
		uint32_t groupSize = static_cast<uint32_t>(m_directAccessGroup.size());
		assert(groupSize > 0);

		const uint32_t groupIndex = pComponent->m_groupIndex;
		const uint32_t lastIndex = groupSize - 1;

		assert(groupIndex < groupSize);
		// assert(groupIndex != std::numeric_limits<uint32_t>::max());
		assert(m_directAccessGroup[groupIndex] == pComponent);	// 중요!

		// 지우려는 컴포넌트 포인터가 맨 뒤에 있는것이 아닌경우
		// 마지막에 위치한 포인터와 위치를 바꾼다.
		if (groupIndex != lastIndex)
		{
			std::swap(m_directAccessGroup[groupIndex], m_directAccessGroup[lastIndex]);
			m_directAccessGroup[groupIndex]->m_groupIndex = groupIndex;	// 마지막에 있던 컴포넌트의 groupIndex를 올바르게 업데이트 해주어야 한다!
		}

		// 가장 끝으로 이동된 포인터를 벡터에서 제거
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
