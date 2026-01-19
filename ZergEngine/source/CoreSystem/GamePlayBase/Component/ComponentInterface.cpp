#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

IComponent::IComponent(GameObject& owner, uint64_t id) noexcept
	: m_pGameObject(&owner)
	, m_id(id)
	, m_tableIndex((std::numeric_limits<uint32_t>::max)())
	, m_groupIndex((std::numeric_limits<uint32_t>::max)())
	, m_flag(ComponentFlag::Enabled)	// 기본 상태는 enable 상태
{
}

void IComponent::Enable()
{
	if (this->IsEnabled())
		return;

	this->OnFlag(ComponentFlag::Enabled);

	// 지연 오브젝트의 컴포넌트인 경우 플래그만 설정하고 빠지기
	if (m_pGameObject->IsPending())
		return;

	this->OnEnableSysJob();
}

void IComponent::Disable()
{
	if (!this->IsEnabled())
		return;

	this->OffFlag(ComponentFlag::Enabled);

	// 지연 오브젝트의 컴포넌트인 경우 플래그만 설정하고 빠지기
	if (m_pGameObject->IsPending())
		return;

	this->OnDisableSysJob();
}

const GameObjectHandle IComponent::GetGameObjectHandle() const
{
	assert(m_pGameObject != nullptr);

	return m_pGameObject->ToHandle();
}

void IComponent::Destroy()
{
	assert(m_pGameObject != nullptr);

	// 지연된 오브젝트에서 컴포넌트를 제거하는 경우는 OnLoadScene에서 Destroy를 한다는 의미인데 이것은 허용하지 않는다.
	if (m_pGameObject->IsPending())
		return;

	IComponentManager* pComponentManager = this->GetComponentManager();
	pComponentManager->RequestDestroy(this);
}

void IComponent::OnDeploySysJob()
{
	this->GetComponentManager()->AddToDirectAccessGroup(this);
}

void IComponent::OnEnableSysJob()
{
}

void IComponent::OnDisableSysJob()
{
}

const ComponentHandleBase IComponent::ToHandle() const
{
	IComponentManager* pComponentManager = this->GetComponentManager();
	assert(pComponentManager->ToPtr(m_tableIndex, m_id) == this);

	return ComponentHandleBase(m_tableIndex, m_id);
}
