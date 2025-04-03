#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

using namespace ze;

IComponentManager::IComponentManager()
	: m_uniqueId(0)
	, m_activeComponents()
	, m_ptrTable{}
{
}

ComponentHandle IComponentManager::Register(IComponent* pComponent)
{
	assert(pComponent != nullptr);

	ComponentHandle hComponent;

	do
	{
		// 빈 자리 검색
		uint32_t emptyIndex = std::numeric_limits<uint32_t>::max();
		for (uint32_t i = 0; i < _countof(m_ptrTable); ++i)
		{
			if (m_ptrTable[i] == nullptr)
			{
				emptyIndex = i;
				break;
			}
		}

		if (emptyIndex == std::numeric_limits<uint32_t>::max())
			break;

		hComponent = ComponentHandle(emptyIndex, pComponent->GetType(), pComponent->GetId());	// 유효한 핸들 준비
		m_activeComponents.push_back(pComponent);
		pComponent->m_activeIndex = static_cast<uint32_t>(m_activeComponents.size() - 1);
		m_ptrTable[emptyIndex] = pComponent;
		pComponent->m_index = emptyIndex;
	} while (false);

	return hComponent;
}

void IComponentManager::Unregister(IComponent* pComponent)
{
	uint32_t vectorSize = static_cast<uint32_t>(m_activeComponents.size());
	assert(vectorSize > 0);
	assert(m_ptrTable[pComponent->m_index] == pComponent);
	assert(pComponent->m_index != std::numeric_limits<uint32_t>::max());
	assert(pComponent->m_activeIndex != std::numeric_limits<uint32_t>::max());

	const uint32_t activeIndex = pComponent->m_activeIndex;
	const uint32_t lastIndex = vectorSize - 1;

	assert(activeIndex != std::numeric_limits<uint32_t>::max());

	if (activeIndex != lastIndex)
	{
		// 마지막 컴포넌트와 위치를 맞바꾼 후
		std::swap(m_activeComponents[activeIndex], m_activeComponents[lastIndex]);
		m_activeComponents[activeIndex]->m_activeIndex = activeIndex;
	}

	// Active 목록에서 제거
	m_activeComponents.pop_back();
	pComponent->m_activeIndex = std::numeric_limits<uint32_t>::max();

	// 테이블에서 제거
	m_ptrTable[pComponent->m_index] = nullptr;
	pComponent->m_index = std::numeric_limits<uint32_t>::max();
}
