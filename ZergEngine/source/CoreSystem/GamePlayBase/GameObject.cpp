#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

GameObject::GameObject(GAMEOBJECT_FLAG flag, PCWSTR name)
	: m_transform(this)
	, m_components()
	, m_id(GameObjectManager.AssignUniqueId())
	, m_tableIndex(std::numeric_limits<uint32_t>::max())
	, m_activeIndex(std::numeric_limits<uint32_t>::max())
	, m_flag(flag)
{
	StringCbCopyW(m_name, sizeof(m_name), name);
}

GameObjectHandle GameObject::Find(PCWSTR name)
{
	return GameObjectManager.FindGameObject(name);
}

void GameObject::SetActive(bool active)
{
	for (Transform* pChildTransform : m_transform.m_children)
		pChildTransform->m_pGameObject->SetActive(active);

	if (active)
	{
		for (IComponent* pComponent : m_components)
			pComponent->Enable();

		this->OnFlag(GOF_ACTIVE);
		GameObjectManager.MoveToInactiveVectorFromActiveVector(this);
	}
	else
	{
		for (IComponent* pComponent : m_components)
			pComponent->Disable();

		this->OffFlag(GOF_ACTIVE);
		GameObjectManager.MoveToActiveVectorFromInactiveVector(this);
	}
}

GameObjectHandle GameObject::ToHandle() const
{
	// 댕글링 포인터 접근 감지
	assert(GameObjectManager.m_table[m_tableIndex] == this);

	return GameObjectHandle(m_tableIndex, m_id);
}
