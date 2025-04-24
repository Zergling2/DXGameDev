#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

const GameObjectHandle IComponent::GetGameObjectHandle() const
{
	if (m_pGameObject == nullptr)
		Debug::ForceCrashWithMessageBox(L"Fatal Error", L"m_pGameObject was null in the IComponent::GetGameObjectHandle()");

	return m_pGameObject->ToHandle();
}

void IComponent::Enable()
{
	if (!m_enabled)
	{
		m_enabled = true;
		this->SystemJobOnEnabled();
	}
}

void IComponent::Disable()
{
	if (m_enabled)
	{
		m_enabled = false;
		this->SystemJobOnDisabled();
	}
}

ComponentHandleBase IComponent::ToHandleBase() const
{
	assert(this->GetComponentManager()->m_table[m_tableIndex] == this);

	return ComponentHandleBase(m_tableIndex, m_id);
}

void IComponent::SystemJobOnEnabled()
{
}

void IComponent::SystemJobOnDisabled()
{
}
