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

	this->OnFlag(CF_ENABLED);

	// disabled ���Ϳ��� enabled ���ͷ� �̵�
	IComponentManager* pComponentManager = this->GetComponentManager();
	pComponentManager->MoveToEnabledVectorFromDisabledVector(this);

	// ��ũ��Ʈ�� ��� SystemJobOnEnable() -> OnEnable()���� �ٽ� Disable()�� ȣ���ϴ� ��쿡��
	// �ùٸ��� �۵��� �� �ֵ��� �÷��� ������Ʈ �� ���� �̵��� �Ϸ�� �ڿ� ȣ���ؾ� �Ѵ�.
	return true;
}

bool IComponent::Disable()
{
	if (!this->IsEnabled())
		return false;

	this->OffFlag(CF_ENABLED);

	// enabled ���Ϳ��� disabled ���ͷ� �̵�
	IComponentManager* pComponentManager = this->GetComponentManager();
	pComponentManager->MoveToDisabledVectorFromEnabledVector(this);

	return true;
}

ComponentHandleBase IComponent::ToHandleBase() const
{
	assert(this->GetComponentManager()->m_table[m_tableIndex] == this);

	return ComponentHandleBase(m_tableIndex, m_id);
}
