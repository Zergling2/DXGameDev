#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

IComponentManager::IComponentManager()
	: m_uniqueId(0)
	, m_destroyed()
	, m_activeComponents()
	, m_table(128)
{
}

void IComponentManager::AddToDestroyQueue(IComponent* pComponent)
{
	assert(pComponent->IsOnTheDestroyQueue() == false);

	pComponent->SetOnDestroyQueueFlag();
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

	m_activeComponents.push_back(pComponent);
	pComponent->m_activeIndex = static_cast<uint32_t>(m_activeComponents.size() - 1);
	m_table[emptyIndex] = pComponent;
	pComponent->m_tableIndex = emptyIndex;

	hComponent = ComponentHandleBase(emptyIndex, pComponent->GetId());	// 유효한 핸들 준비

	return hComponent;
}

void IComponentManager::RemoveDestroyedComponents()
{
	for (IComponent* pComponent : m_destroyed)
	{
		assert(pComponent->IsOnTheDestroyQueue());

		GameObject* pGameObject = pComponent->GetGameObjectHandle().ToPtr();
		assert(pGameObject != nullptr);

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

		uint32_t activeSize = static_cast<uint32_t>(m_activeComponents.size());
		assert(activeSize > 0);
		assert(m_table[pComponent->m_tableIndex] == pComponent);
		assert(pComponent->m_tableIndex != std::numeric_limits<uint32_t>::max());
		assert(pComponent->m_activeIndex != std::numeric_limits<uint32_t>::max());

		const uint32_t activeIndex = pComponent->m_activeIndex;
		const uint32_t lastIndex = activeSize - 1;

		assert(activeIndex != std::numeric_limits<uint32_t>::max());

		if (activeIndex != lastIndex)
		{
			// 마지막 컴포넌트와 위치를 맞바꾼 후
			std::swap(m_activeComponents[activeIndex], m_activeComponents[lastIndex]);
			m_activeComponents[activeIndex]->m_activeIndex = activeIndex;
		}

		// Active 목록에서 제거
		m_activeComponents.pop_back();

		// 테이블에서 제거
		m_table[pComponent->m_tableIndex] = nullptr;

		delete pComponent;
	}

	m_destroyed.clear();
}
