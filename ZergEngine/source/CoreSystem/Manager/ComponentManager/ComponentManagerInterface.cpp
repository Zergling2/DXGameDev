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

	m_table[emptyIndex] = pComponent;
	pComponent->m_tableIndex = emptyIndex;
	hComponent = ComponentHandleBase(emptyIndex, pComponent->GetId());	// ��ȿ�� �ڵ� �غ�

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

		// GameObject�� ���ŵǴ°� �ƴ� ���� GameObject�� ����Ʈ���� ������Ʈ�� ã�Ƽ� �����ؾ� ��
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

		if (pComponent->IsEnabled())
			RemovePtrFromEnabledVector(pComponent);
		else
			RemovePtrFromDisabledVector(pComponent);

		// ���̺��� ����
		assert(m_table[pComponent->m_tableIndex] == pComponent);
		assert(pComponent->m_tableIndex != std::numeric_limits<uint32_t>::max());
		m_table[pComponent->m_tableIndex] = nullptr;

		delete pComponent;
	}

	m_destroyed.clear();	// �ı��� ������Ʈ ������ ��� ����
}

void IComponentManager::MoveToDisabledVectorFromEnabledVector(IComponent* pComponent)
{
	// ��Ȱ��ȭ�� ������Ʈ���� Ȯ�� assert
	assert(pComponent->IsEnabled() == false);
	RemovePtrFromEnabledVector(pComponent);
	AddPtrToDisabledVector(pComponent);
}

void IComponentManager::MoveToEnabledVectorFromDisabledVector(IComponent* pComponent)
{
	// Ȱ��ȭ�� ������Ʈ���� Ȯ�� assert
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
	assert(vector[activeIndex] == pComponent);	// �߿�!

	// ������� ������Ʈ �����Ͱ� �� �ڿ� �ִ°��� �ƴѰ��
	// �������� ��ġ�� �����Ϳ� ��ġ�� �ٲ۴�.
	if (activeIndex != lastIndex)
	{
		std::swap(vector[activeIndex], vector[lastIndex]);
		vector[activeIndex]->m_activeIndex = activeIndex;	// �������� �ִ� ������Ʈ�� activeIndex�� �ùٸ��� ������Ʈ ���־�� �Ѵ�!
	}

	// �ҼӵǾ� �ִ� vector���� ����
	vector.pop_back();
}
