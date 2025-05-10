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

ComponentHandleBase IComponentManager::RegisterToHandleTable(IComponent* pComponent)
{
	assert(pComponent != nullptr);
	assert(pComponent->m_tableIndex == std::numeric_limits<uint32_t>::max());
	assert(pComponent->m_activeIndex == std::numeric_limits<uint32_t>::max());

	ComponentHandleBase hComponent;

	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	// Table�� �߰�
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

	return hComponent;
}

void IComponentManager::AddPtrToActiveVector(IComponent* pComponent)
{
	// Active vector�� �߰�
	m_activeComponents.push_back(pComponent);
	pComponent->m_activeIndex = static_cast<uint32_t>(m_activeComponents.size() - 1);
}

void IComponentManager::RemoveDestroyedComponents()
{
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (IComponent* pComponent : m_destroyed)
	{
		assert(pComponent->IsOnTheDestroyQueue());

		GameObject* pGameObject = pComponent->m_pGameObject;
		assert(pGameObject != nullptr);

		// Step 1. GameObject�� Component List���� ���� �ı��Ǵ� ������Ʈ �����͸� ã�� ����
		// ���� GameObject�� Destroy Queue�� �ִ� ��쿡�� ���ʿ��� �۾� (��۸� �����Ͱ� �����ִ� ���Ͱ� �ı��ǹǷ�.)
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

		// Step 2. Active ���Ϳ��� ���� �ı��Ǵ� ������Ʈ �����͸� ����
		uint32_t activeVectorSize = static_cast<uint32_t>(m_activeComponents.size());
		assert(activeVectorSize > 0);

		const uint32_t activeIndex = pComponent->m_activeIndex;
		const uint32_t lastIndex = activeVectorSize - 1;

		assert(activeIndex < activeVectorSize);
		// assert(activeIndex != std::numeric_limits<uint32_t>::max());
		assert(m_activeComponents[activeIndex] == pComponent);	// �߿�!

		// ������� ������Ʈ �����Ͱ� �� �ڿ� �ִ°��� �ƴѰ��
		// �������� ��ġ�� �����Ϳ� ��ġ�� �ٲ۴�.
		if (activeIndex != lastIndex)
		{
			std::swap(m_activeComponents[activeIndex], m_activeComponents[lastIndex]);
			m_activeComponents[activeIndex]->m_activeIndex = activeIndex;	// �������� �ִ� ������Ʈ�� activeIndex�� �ùٸ��� ������Ʈ ���־�� �Ѵ�!
		}

		// ���� ������ �̵��� �����͸� ���Ϳ��� ����
		m_activeComponents.pop_back();

		// Step 3. ���̺��� ����
		assert(m_table[pComponent->m_tableIndex] == pComponent);
		assert(pComponent->m_tableIndex != std::numeric_limits<uint32_t>::max());
		m_table[pComponent->m_tableIndex] = nullptr;

		delete pComponent;
	}

	m_destroyed.clear();	// �ı��� ������Ʈ ������ ��� ����
}
