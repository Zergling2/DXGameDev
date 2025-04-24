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

	// �� �ڸ� �˻�
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

	// ���� �� �ڸ��� ã�� ���� ���
	if (emptyIndex == std::numeric_limits<uint32_t>::max())
	{
		emptyIndex = tableSize;
		m_table.push_back(nullptr);	// ���̺� ���� Ȯ��
	}

	m_activeComponents.push_back(pComponent);
	pComponent->m_activeIndex = static_cast<uint32_t>(m_activeComponents.size() - 1);
	m_table[emptyIndex] = pComponent;
	pComponent->m_tableIndex = emptyIndex;

	hComponent = ComponentHandleBase(emptyIndex, pComponent->GetId());	// ��ȿ�� �ڵ� �غ�

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
			// GameObject�� ������Ʈ ����Ʈ���� ����
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
			// ������ ������Ʈ�� ��ġ�� �¹ٲ� ��
			std::swap(m_activeComponents[activeIndex], m_activeComponents[lastIndex]);
			m_activeComponents[activeIndex]->m_activeIndex = activeIndex;
		}

		// Active ��Ͽ��� ����
		m_activeComponents.pop_back();

		// ���̺��� ����
		m_table[pComponent->m_tableIndex] = nullptr;

		delete pComponent;
	}

	m_destroyed.clear();
}
