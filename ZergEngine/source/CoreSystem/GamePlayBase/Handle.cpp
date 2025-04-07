#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <ZergEngine\CoreSystem\GameObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerMap.h>

using namespace ze;

ComponentHandle ComponentHandle::MakeFakeHandle(IComponent* pComponent)
{
	ComponentHandle hFakeHandle;

	hFakeHandle.m_type = pComponent->GetType();
	hFakeHandle.m_pComponent = pComponent;

	return hFakeHandle;
}

GameObject* GameObjectHandle::ToPtr() const
{
	assert(m_index < _countof(GameObjectManagerImpl::m_ptrTable));
	GameObject* pGameObject = GameObjectManager.m_ptrTable[m_index];
	
	if (pGameObject == nullptr || pGameObject->GetId() != m_id)
		return nullptr;
	else
		return pGameObject;
}

IComponent* ComponentHandle::ToPtr() const
{
	IComponentManager* pComponentManager = ComponentManagerMap::GetComponentManager(m_type);
	if (!pComponentManager)	// UNKNOWN 타입으로 GetComponentManager를 호출한 경우
		return nullptr;

	IComponent* pComponent = pComponentManager->m_ptrTable[m_index];
	if (pComponent == nullptr || pComponent->GetId() != m_id)
		return nullptr;
	else
		return pComponent;
}

bool ze::operator==(const GameObjectHandle& hA, const GameObjectHandle& hB)
{
	return
		hA.GetIndex() == hB.GetIndex() &&
		hA.GetId() == hB.GetId();
}

bool ze::operator==(const ComponentHandle& hA, const ComponentHandle& hB)
{
	return
		hA.GetIndex() == hB.GetIndex() &&
		hA.GetType() == hB.GetType() &&
		hA.GetId() == hB.GetId();
}
