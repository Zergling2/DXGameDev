#include <ZergEngine\CoreSystem\Manager\ComponentManager\TransformManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>

namespace ze
{
    TransformManagerImpl TransformManager;
}

using namespace ze;

TransformManagerImpl::TransformManagerImpl()
{
}

TransformManagerImpl::~TransformManagerImpl()
{
}

void TransformManagerImpl::Init(void* pDesc)
{
}

void TransformManagerImpl::Release()
{
}

void TransformManagerImpl::RemoveDestroyedComponents()
{
	for (IComponent* pComponent : m_destroyed)
	{
		assert(pComponent->IsOnTheDestroyQueue());


		// 1. Transform �ڽ� �θ� ���� ����
		Transform* pTransform = static_cast<Transform*>(pComponent);
		if (pTransform->m_pParentTransform != nullptr)
		{
#if defined(DEBUG) || defined(_DEBUG)
			bool find = false;
#endif
			std::vector<Transform*>::const_iterator end = pTransform->m_pParentTransform->m_children.cend();
			std::vector<Transform*>::const_iterator iter = pTransform->m_pParentTransform->m_children.cbegin();
			while (iter != end)
			{
				if (*iter == pTransform)
				{
#if defined(DEBUG) || defined(_DEBUG)
					find = true;
#endif
					pTransform->m_pParentTransform->m_children.erase(iter);
					break;
				}
			}
			assert(find == true);
		}

		for (Transform* pChild : pTransform->m_children)
		{
			assert(pChild->m_pParentTransform == pTransform);
			assert(pChild->IsOnTheDestroyQueue() == true);
			pChild->m_pParentTransform = nullptr;	// �ؿ��� �� delete�� �ڽ��� �����ϴ� ���� ���� (��۸������� ����)
		}
		// pTransform->m_children.clear();	// ��ü delete�� �ڵ� �Ҹ�


		// 2. �ڽ��� �����ϴ� GameObject���� �ڽ��� ������ ����
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
