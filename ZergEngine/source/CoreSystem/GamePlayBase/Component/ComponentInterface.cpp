#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

const GameObjectHandle IComponent::GetGameObjectHandle() const
{
	if (m_pGameObject == nullptr)
		Debug::ForceCrashWithMessageBox(L"Error", L"m_pGameObject was null in the IComponent::GetGameObjectHandle()");

	return m_pGameObject->ToHandle();
}

bool IComponent::Enable()
{
	if (this->IsEnabled())
		return false;

	this->OnFlag(COMPONENT_FLAG::ENABLED);

	if (m_pGameObject->IsDeferred())	// ��ũ��Ʈ�� ��� deferred ���¿��� OnEnable�� ȣ����� �ʵ��� �Ѵ�.
		return false;

	return true;
}

bool IComponent::Disable()
{
	if (!this->IsEnabled())
		return false;

	this->OffFlag(COMPONENT_FLAG::ENABLED);

	if (m_pGameObject->IsDeferred())	// ��ũ��Ʈ�� ��� deferred ���¿��� OnDisable�� ȣ����� �ʵ��� �Ѵ�.
		return false;

	return true;
}

const ComponentHandleBase IComponent::ToHandleBase() const
{
	assert(this->GetComponentManager()->m_table[m_tableIndex] == this);

	return ComponentHandleBase(m_tableIndex, m_id);
}
