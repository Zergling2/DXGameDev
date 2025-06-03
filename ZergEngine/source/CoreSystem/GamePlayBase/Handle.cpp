#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerMap.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

using namespace ze;

GameObject* GameObjectHandle::ToPtr() const
{
	assert(m_tableIndex < GameObjectManager.m_table.size());
	GameObject* pGameObject = GameObjectManager.m_table[m_tableIndex];
	
	if (pGameObject == nullptr || pGameObject->GetId() != m_id)
		return nullptr;
	else
		return pGameObject;
}

IUIObject* UIObjectHandle::ToPtr() const
{
	assert(m_tableIndex < UIObjectManager.m_table.size());

	IUIObject* pUIObject = UIObjectManager.m_table[m_tableIndex];
	if (pUIObject == nullptr || pUIObject->GetId() != m_id)
		return nullptr;
	else
		return pUIObject;
}

IComponent* ToPtrHelper::ToComponentPtrImpl(COMPONENT_TYPE type, uint32_t tableIndex, uint64_t id)
{
	IComponentManager* pComponentManager = ComponentManagerMap::GetManager(type);
	assert(pComponentManager != nullptr);
	assert(tableIndex < pComponentManager->m_table.size());

	IComponent* pComponent = pComponentManager->m_table[tableIndex];
	if (pComponent == nullptr || pComponent->GetId() != id)
		return nullptr;
	else
		return pComponent;
}
