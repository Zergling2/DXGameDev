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
	, m_emptyHandleTableIndex(HANDLE_TABLE_INIT_SIZE)	// ����ȭ�� �ּ�ȭ�ϱ� ���ؼ� ���ʿ� �ڵ� ���̺��� ������� ������ ������ŭ �ε��� ���۸� �Ҵ��Ѵ�.
	, m_handleTable(HANDLE_TABLE_INIT_SIZE, nullptr)
{
	m_lock.Init();
}

void IComponentManager::Init()
{
	m_uniqueId = 0;

	// ���͸� ����ó�� ����� ���̹Ƿ� ���� �ؿ� ���� �ε����� �־�ξ� 0�� �ε������� ����� �� �ֵ��� �Ѵ�.
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
	// ������Ʈ�ڵ� ���ø��� �߸� ����ϸ� ������ ������Ʈ �������� ���̺� Query�� �� �� �ִ�. �� ��� ������ ���� �߻� ����.

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

		// �ڵ� ���̺��� �� �ڸ��� �˻�
		uint32_t emptyIndex;
		const size_t ehtIdxSize = m_emptyHandleTableIndex.size();
		if (ehtIdxSize > 0)
		{
			emptyIndex = m_emptyHandleTableIndex[ehtIdxSize - 1];
			m_emptyHandleTableIndex.pop_back();
		}
		else
		{
			// ���� �ڵ� ���̺� �� ������ ���� ���
			m_handleTable.push_back(nullptr);	// �ڵ� ���̺� �� ���� �߰�
			emptyIndex = static_cast<uint32_t>(m_handleTable.size() - 1);
		}

		m_handleTable[emptyIndex] = pComponent;
		pComponent->m_tableIndex = emptyIndex;
	}

	hComponent = ComponentHandleBase(pComponent->m_tableIndex, pComponent->GetId());	// ��ȿ�� �ڵ� �غ�

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
		m_emptyHandleTableIndex.push_back(pComponent->m_tableIndex);		// �ı��Ǵ� ������Ʈ�� ����ϴ� �ڵ� ���̺� �ε����� �ٽ� ����

		delete pComponent;
	}

	m_destroyed.clear();	// �ı��� ������Ʈ ������ ��� ����
}

void IComponentManager::AddToDestroyQueue(IComponent* pComponent)
{
	assert(!pComponent->IsOnTheDestroyQueue());

	pComponent->OnFlag(ComponentFlag::OnDestroyQueue);		// �ߺ� ���� ����!
	m_destroyed.push_back(pComponent);
}
