#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerMap.h>

using namespace ze;

GameObject* GameObjectHandle::ToPtr() const
{
	return GameObjectManager::GetInstance()->ToPtr(m_tableIndex, m_id);
}

IUIObject* UIObjectHandle::ToPtr() const
{
	return UIObjectManager::GetInstance()->ToPtr(m_tableIndex, m_id);
}

IComponent* ToPtrHelper::ToComponentPtrImpl(ComponentType type, uint32_t tableIndex, uint64_t id)
{
	IComponentManager* pComponentManager = ComponentManagerMap::GetManager(type);
	assert(pComponentManager != nullptr);

	return pComponentManager->ToPtr(tableIndex, id);
}
