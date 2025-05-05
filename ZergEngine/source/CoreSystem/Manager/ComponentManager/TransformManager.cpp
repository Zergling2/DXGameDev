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


		// 1. Transform 자식 부모 연결 제거
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
			pChild->m_pParentTransform = nullptr;	// 밑에서 곧 delete될 자신을 접근하는 것을 방지 (댕글링포인터 제거)
		}
		// pTransform->m_children.clear();	// 객체 delete시 자동 소멸


		// 2. 자신을 소유하는 GameObject에서 자신의 포인터 제거
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
