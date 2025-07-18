#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

IComponentManager::IComponentManager()
	: m_uniqueId(0)
	, m_lock()
	, m_destroyed()
	, m_directAccessGroup()
	, m_handleTable(128)
{
	m_lock.Init();
}

void IComponentManager::Init()
{
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
	GameObject* pOwner = pComponent->m_pGameObject;
	assert(pOwner != nullptr);

	// ������ ������Ʈ���� ������Ʈ�� �����ϴ� ���� OnLoadScene���� Destroy�� �Ѵٴ� �ǹ��ε� �̰��� ������� �ʴ´�.
	if (pOwner->IsPending())
		return;

	this->AddToDestroyQueue(pComponent);
}

IComponent* IComponentManager::ToPtr(uint32_t tableIndex, uint64_t id) const
{
	assert(tableIndex < m_handleTable.size());
	// ������Ʈ�ڵ� ���ø��� �߸� ����ϸ� ������ ������Ʈ �������� ���̺� Query�� �� �� �ִ�. �� ��� ������ ���� �߻� ����.

	IComponent* pComponent = m_handleTable[tableIndex];
	if (pComponent == nullptr || pComponent->GetId() != id)
		return nullptr;
	else
		return pComponent;
}

ComponentHandleBase IComponentManager::RegisterToHandleTable(IComponent* pComponent)
{
	assert(pComponent != nullptr);
	assert(pComponent->m_tableIndex == std::numeric_limits<uint32_t>::max());
	assert(pComponent->m_groupIndex == std::numeric_limits<uint32_t>::max());

	ComponentHandleBase hComponent;

	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	// Table�� �߰�
	// �� �ڸ� �˻�
	uint32_t emptyIndex;
	bool find = false;
	const uint32_t tableSize = static_cast<uint32_t>(m_handleTable.size());
	for (uint32_t i = 0; i < tableSize; ++i)
	{
		if (m_handleTable[i] == nullptr)
		{
			emptyIndex = i;
			find = true;
			break;
		}
	}

	// ���� �� �ڸ��� ã�� ���� ���
	if (!find)
	{
		emptyIndex = tableSize;
		m_handleTable.push_back(nullptr);	// ���̺� ���� Ȯ��
	}

	m_handleTable[emptyIndex] = pComponent;
	pComponent->m_tableIndex = emptyIndex;

	hComponent = ComponentHandleBase(emptyIndex, pComponent->GetId());	// ��ȿ�� �ڵ� �غ�

	return hComponent;
}

void IComponentManager::AddToDirectAccessGroup(IComponent* pComponent)
{
	// Active vector�� �߰�
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

		// Step 2. Enabled group���� ���� �ı��Ǵ� ������Ʈ �����͸� ����
		uint32_t groupSize = static_cast<uint32_t>(m_directAccessGroup.size());
		assert(groupSize > 0);

		const uint32_t groupIndex = pComponent->m_groupIndex;
		const uint32_t lastIndex = groupSize - 1;

		assert(groupIndex < groupSize);
		// assert(groupIndex != std::numeric_limits<uint32_t>::max());
		assert(m_directAccessGroup[groupIndex] == pComponent);	// �߿�!

		// ������� ������Ʈ �����Ͱ� �� �ڿ� �ִ°��� �ƴѰ��
		// �������� ��ġ�� �����Ϳ� ��ġ�� �ٲ۴�.
		if (groupIndex != lastIndex)
		{
			std::swap(m_directAccessGroup[groupIndex], m_directAccessGroup[lastIndex]);
			m_directAccessGroup[groupIndex]->m_groupIndex = groupIndex;	// �������� �ִ� ������Ʈ�� groupIndex�� �ùٸ��� ������Ʈ ���־�� �Ѵ�!
		}

		// ���� ������ �̵��� �����͸� ���Ϳ��� ����
		m_directAccessGroup.pop_back();

		// Step 3. ���̺��� ����
		assert(m_handleTable[pComponent->m_tableIndex] == pComponent);
		m_handleTable[pComponent->m_tableIndex] = nullptr;

		delete pComponent;
	}

	m_destroyed.clear();	// �ı��� ������Ʈ ������ ��� ����
}


void IComponentManager::AddToDestroyQueue(IComponent* pComponent)
{
	if (pComponent->IsOnTheDestroyQueue())
		return;

	pComponent->OnFlag(COMPONENT_FLAG::ON_DESTROY_QUEUE);		// �ߺ� ���� ����!
	m_destroyed.push_back(pComponent);
}
