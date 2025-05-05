#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

void IComponentManager::AddToDestroyQueue(IComponent* pComponent)
{
	assert(pComponent->IsOnTheDestroyQueue() == false);

	pComponent->OnFlag(CF_ON_DESTROY_QUEUE);
	m_destroyed.push_back(pComponent);
}

ComponentHandleBase IComponentManager::Register(IComponent* pComponent)
{
	assert(pComponent != nullptr);
	assert(pComponent->m_tableIndex == std::numeric_limits<uint32_t>::max());
	assert(pComponent->m_activeIndex == std::numeric_limits<uint32_t>::max());

	ComponentHandleBase hComponent;

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

	// 만약 빈 자리를 찾지 못한 경우
	if (emptyIndex == std::numeric_limits<uint32_t>::max())
	{
		emptyIndex = tableSize;
		m_table.push_back(nullptr);	// 테이블 공간 확보
	}

	m_table[emptyIndex] = pComponent;
	pComponent->m_tableIndex = emptyIndex;
	hComponent = ComponentHandleBase(emptyIndex, pComponent->GetId());	// 유효한 핸들 준비

	if (pComponent->IsEnabled())
		AddPtrToEnabledVector(pComponent);
	else
		AddPtrToDisabledVector(pComponent);

	return hComponent;
}

void IComponentManager::RemoveDestroyedComponents()
{
	for (IComponent* pComponent : m_destroyed)
	{
		assert(pComponent->IsOnTheDestroyQueue());

		GameObject* pGameObject = pComponent->m_pGameObject;
		assert(pGameObject != nullptr);

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

		if (pComponent->IsEnabled())
			RemovePtrFromEnabledVector(pComponent);
		else
			RemovePtrFromDisabledVector(pComponent);

		// 테이블에서 제거
		assert(m_table[pComponent->m_tableIndex] == pComponent);
		assert(pComponent->m_tableIndex != std::numeric_limits<uint32_t>::max());
		m_table[pComponent->m_tableIndex] = nullptr;

		delete pComponent;
	}

	m_destroyed.clear();	// 파괴된 컴포넌트 포인터 목록 제거
}

void IComponentManager::MoveToDisabledVectorFromEnabledVector(IComponent* pComponent)
{
	// 비활성화된 컴포넌트인지 확인 assert
	assert(pComponent->IsEnabled() == false);
	RemovePtrFromEnabledVector(pComponent);
	AddPtrToDisabledVector(pComponent);
}

void IComponentManager::MoveToEnabledVectorFromDisabledVector(IComponent* pComponent)
{
	// 활성화된 컴포넌트인지 확인 assert
	assert(pComponent->IsEnabled() == true);
	RemovePtrFromDisabledVector(pComponent);
	AddPtrToEnabledVector(pComponent);
}

void IComponentManager::AddPtrToVector(std::vector<IComponent*>& vector, IComponent* pComponent)
{
	vector.push_back(pComponent);
	pComponent->m_activeIndex = static_cast<uint32_t>(vector.size() - 1);
}

void IComponentManager::RemovePtrFromVector(std::vector<IComponent*>& vector, IComponent* pComponent)
{
	uint32_t vectorSize = static_cast<uint32_t>(vector.size());
	assert(vectorSize > 0);

	const uint32_t activeIndex = pComponent->m_activeIndex;
	const uint32_t lastIndex = vectorSize - 1;

	assert(activeIndex < vectorSize);
	// assert(activeIndex != std::numeric_limits<uint32_t>::max());
	assert(vector[activeIndex] == pComponent);	// 중요!

	// 지우려는 컴포넌트 포인터가 맨 뒤에 있는것이 아닌경우
	// 마지막에 위치한 포인터와 위치를 바꾼다.
	if (activeIndex != lastIndex)
	{
		std::swap(vector[activeIndex], vector[lastIndex]);
		vector[activeIndex]->m_activeIndex = activeIndex;	// 마지막에 있던 컴포넌트의 activeIndex를 올바르게 업데이트 해주어야 한다!
	}

	// 소속되어 있던 vector에서 제거
	vector.pop_back();
}
