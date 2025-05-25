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

	// 이미 해당 활성 상태가 설정되어 있는 경우 함수 리턴
	const bool isActive = this->IsActive();
	if (isActive == active)
		return;

	if (active)
	{
		for (IComponent* pComponent : m_components)
			pComponent->Enable();

		this->OnFlag(GAMEOBJECT_FLAG::ACTIVE);

		// 지연되지 않은 경우에만 포인터 이동 (지연된 상태에서는 Active/Inactive 벡터에 포인터가 존재하지 않는다.)
		if (!this->IsDeferred())
			GameObjectManager.MoveToActiveVectorFromInactiveVector(this);
	}
	else
	{
		for (IComponent* pComponent : m_components)
			pComponent->Disable();

		this->OffFlag(GAMEOBJECT_FLAG::ACTIVE);

		// 지연되지 않은 경우에만 포인터 이동 (지연된 상태에서는 Active/Inactive 벡터에 포인터가 존재하지 않는다.)
		if (!this->IsDeferred())
			GameObjectManager.MoveToInactiveVectorFromActiveVector(this);
	}
}

const GameObjectHandle GameObject::ToHandle() const
{
	// 댕글링 포인터 접근 감지
	assert(GameObjectManager.m_table[m_tableIndex] == this);

	return GameObjectHandle(m_tableIndex, m_id);
}
