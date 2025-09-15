#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

IComponent::IComponent(uint64_t id) noexcept
	: m_pGameObject(nullptr)
	, m_id(id)
	, m_tableIndex((std::numeric_limits<uint32_t>::max)())
	, m_groupIndex((std::numeric_limits<uint32_t>::max)())
	, m_flag(ComponentFlag::Enabled)	// �⺻ ���´� enable ����
{
}

void IComponent::Enable()
{
	if (this->IsEnabled())
		return;

	this->OnFlag(ComponentFlag::Enabled);
}

void IComponent::Disable()
{
	if (!this->IsEnabled())
		return;

	this->OffFlag(ComponentFlag::Enabled);
}

const GameObjectHandle IComponent::GetGameObjectHandle() const
{
	assert(m_pGameObject != nullptr);

	return m_pGameObject->ToHandle();
}

void IComponent::Destroy()
{
	assert(m_pGameObject != nullptr);

	// ������ ������Ʈ���� ������Ʈ�� �����ϴ� ���� OnLoadScene���� Destroy�� �Ѵٴ� �ǹ��ε� �̰��� ������� �ʴ´�.
	if (m_pGameObject->IsPending())
		return;

	IComponentManager* pComponentManager = this->GetComponentManager();

	pComponentManager->RequestDestroy(this);
}

const ComponentHandleBase IComponent::ToHandle() const
{
	IComponentManager* pComponentManager = this->GetComponentManager();
	assert(pComponentManager->ToPtr(m_tableIndex, m_id) == this);

	return ComponentHandleBase(m_tableIndex, m_id);
}
