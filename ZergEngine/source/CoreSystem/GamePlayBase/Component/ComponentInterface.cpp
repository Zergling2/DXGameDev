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

	// disabled 벡터에서 enabled 벡터로 이동
	IComponentManager* pComponentManager = this->GetComponentManager();
	pComponentManager->MoveToEnabledVectorFromDisabledVector(this);

	// 스크립트의 경우 SystemJobOnEnable() -> OnEnable()에서 다시 Disable()을 호출하는 경우에도
	// 올바르게 작동할 수 있도록 플래그 업데이트 및 벡터 이동이 완료된 뒤에 호출해야 한다.
	return true;
}

bool IComponent::Disable()
{
	if (!this->IsEnabled())
		return false;

	this->OffFlag(CF_ENABLED);

	// enabled 벡터에서 disabled 벡터로 이동
	IComponentManager* pComponentManager = this->GetComponentManager();
	pComponentManager->MoveToDisabledVectorFromEnabledVector(this);

	return true;
}

ComponentHandleBase IComponent::ToHandleBase() const
{
	assert(this->GetComponentManager()->m_table[m_tableIndex] == this);

	return ComponentHandleBase(m_tableIndex, m_id);
}
