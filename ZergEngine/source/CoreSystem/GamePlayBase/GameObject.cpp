#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

GameObject::GameObject(GAMEOBJECT_FLAG flag, PCWSTR name)
	: m_pParent(nullptr)
	, m_children()
	, m_components()
	, m_pTransform(nullptr)
	, m_id(GameObjectManager.AssignUniqueId())
	, m_tableIndex(std::numeric_limits<uint32_t>::max())
	, m_activeIndex(std::numeric_limits<uint32_t>::max())
	, m_flag(flag)
{
	Transform* pTransform = new Transform();
	m_pTransform = pTransform;

	pTransform->m_pGameObject = this;
	m_components.push_back(pTransform);

	if (!this->IsDeferred())
	{
		IComponentManager* pComponentManager = pTransform->GetComponentManager();
		assert(pComponentManager != nullptr);

		ComponentHandle<Transform> hComponent = pComponentManager->Register(pTransform);
		assert(hComponent.IsValid());
	}

	StringCbCopyW(m_name, sizeof(m_name), name);
}

GameObjectHandle GameObject::ToHandle() const
{
	// 댕글링 포인터 접근 감지
	assert(GameObjectManager.m_table[m_tableIndex] == this);

	return GameObjectHandle(m_tableIndex, m_id);
}

bool GameObject::SetParent(GameObject* pGameObject)
{
	// 자기 자신을 부모로 설정하려는 경우
	if (pGameObject == this)
		return false;

	// 이미 부모인 경우
	if (m_pParent == pGameObject)
		return false;

	// Check cycling
	// pGameObject가 이미 나를 조상으로 하고 있는 경우
	if (pGameObject != nullptr && pGameObject->IsDescendant(this))
		return false;

	// 기존 부모에서 떠나는 자식 포인터를 찾아 제거
	if (m_pParent != nullptr)
	{
#if defined(DEBUG) || defined(_DEBUG)
		bool find = false;
#endif
		std::list<GameObject*>::const_iterator end = m_pParent->m_children.cend();
		std::list<GameObject*>::const_iterator iter = m_pParent->m_children.cbegin();
		while (iter != end)
		{
			if ((*iter) == this)
			{
				m_pParent->m_children.erase(iter);
#if defined(DEBUG) || defined(_DEBUG)
				find = true;
#endif
				break;
			}
		}
		assert(find == true);
	}

	// 부모의 자식 목록을 업데이트
	if (pGameObject != nullptr)
		pGameObject->m_children.push_back(this);

	// 자식의 부모 포인터를 업데이트
	m_pParent = pGameObject;

	return true;
}

bool GameObject::IsDescendant(GameObject* pGameObject) const
{
	const GameObject* pCurrent = m_pParent;

	while (pCurrent != nullptr)
	{
		if (pCurrent == pGameObject)
			return true;

		pCurrent = pCurrent->m_pParent;
	}

	return false;
}

XMMATRIX GameObject::CalcWorldTransformMatrix() const noexcept
{
	if (m_pParent != nullptr)
		return m_pParent->CalcWorldTransformMatrix() * CalcLocalTransformMatrix();
	else
		return CalcLocalTransformMatrix(); 
}

XMMATRIX GameObject::CalcLocalTransformMatrix() const noexcept
{
	return
		XMMatrixScalingFromVector(XMLoadFloat3A(&m_pTransform->m_scale)) *
		XMMatrixRotationQuaternion(XMLoadFloat4A(&m_pTransform->m_rotation)) *
		XMMatrixTranslationFromVector(XMLoadFloat3A(&m_pTransform->m_position));
}
