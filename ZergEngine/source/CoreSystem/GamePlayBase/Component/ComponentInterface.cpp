#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

IComponent::IComponent(uint64_t id) noexcept
	: m_pGameObject(nullptr)
	, m_id(id)
	, m_tableIndex(std::numeric_limits<uint32_t>::max())
	, m_groupIndex(std::numeric_limits<uint32_t>::max())
	, m_flag(COMPONENT_FLAG::ENABLED)	// 기본 상태는 enable 상태
{
}

void IComponent::Enable()
{
	if (this->IsEnabled())
		return;

	this->OnFlag(COMPONENT_FLAG::ENABLED);
}

void IComponent::Disable()
{
	if (!this->IsEnabled())
		return;

	this->OffFlag(COMPONENT_FLAG::ENABLED);
}

const GameObjectHandle IComponent::GetGameObjectHandle() const
{
	assert(m_pGameObject != nullptr);

	return m_pGameObject->ToHandle();
}

void IComponent::Destroy()
{
	IComponentManager* pComponentManager = this->GetComponentManager();

	pComponentManager->RequestDestroy(this);
}

const ComponentHandleBase IComponent::ToHandle() const
{
	IComponentManager* pComponentManager = this->GetComponentManager();
	assert(pComponentManager->ToPtr(m_tableIndex, m_id) == this);

	return ComponentHandleBase(m_tableIndex, m_id);
}
