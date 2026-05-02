#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerMap.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MonoBehaviour.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>

using namespace ze;

GameObject::GameObject(uint64_t id, GAMEOBJECT_FLAG flag, PCWSTR name)
	: m_transform(this)
	, m_components()
	, m_id(id)
	, m_tableIndex((std::numeric_limits<uint32_t>::max)())
	, m_actInactGroupIndex((std::numeric_limits<uint32_t>::max)())
	, m_flag(flag)
{
	StringCbCopyW(m_name, sizeof(m_name), name);
}

GameObjectHandle GameObject::Find(PCWSTR name)
{
	return GameObjectManager::GetInstance()->FindGameObject(name);
}

void GameObject::DontDestroyOnLoad()
{
	this->OnFlag(GAMEOBJECT_FLAG::DONT_DESTROY_ON_LOAD);
}

void GameObject::DontDestroyOnLoadRecursively()
{
	this->OnFlag(GAMEOBJECT_FLAG::DONT_DESTROY_ON_LOAD);

	for (auto pChild : m_transform.m_children)
		pChild->m_pGameObject->DontDestroyOnLoadRecursively();
}

void GameObject::Destroy()
{
	if (this->IsPending())
		return;

	GameObjectManager::GetInstance()->RequestDestroy(this);
}

void GameObject::SetActive(bool active)
{
	// 이미 해당 활성 상태가 설정되어 있는 경우 함수 리턴
	if (this->IsActiveSelf() == active)
		return;

	if (active)
		this->OnFlag(GAMEOBJECT_FLAG::ACTIVE_SELF);
	else
		this->OffFlag(GAMEOBJECT_FLAG::ACTIVE_SELF);

	Transform* pParentTransform = m_transform.m_pParent;
	if (pParentTransform)
		this->UpdateActiveState(pParentTransform->m_pGameObject->IsActiveInHierarchy());
	else
		this->UpdateActiveState(true);	// 부모가 없는 경우 부모의 ActiveInHierarchy가 true라고 가정하면 된다.
}

const GameObjectHandle GameObject::ToHandle() const
{
	// 댕글링 포인터 접근 감지
	assert(GameObjectManager::GetInstance()->m_handleTable[m_tableIndex] == this);

	return GameObjectHandle(m_tableIndex, m_id);
}

void GameObject::UpdateActiveState(bool isParentActiveInHierarchy)
{
	Transform* pParentTransform = m_transform.m_pParent;

	// DFS 구조로 상위 계층 -> 하위 계층으로 내려가면서 업데이트하므로 부모의 IsActiveInHierarchy는 이미 업데이트되어있으므로 한 단계 부모의 플래그만 확인해도 된다.
	// bool activeInHierarchy = this->IsActiveSelf() && (pParentTransform ? pParentTransform->m_pGameObject->IsActiveInHierarchy() : true);
	bool activeInHierarchy = this->IsActiveSelf() && isParentActiveInHierarchy;

	if (this->IsActiveInHierarchy() == activeInHierarchy)
		return;

	if (activeInHierarchy)
	{
		this->OnFlag(GAMEOBJECT_FLAG::ACTIVE_IN_HIERARCHY);
		this->OnActivationSysJob();
	}
	else
	{
		this->OffFlag(GAMEOBJECT_FLAG::ACTIVE_IN_HIERARCHY);
		this->OnDeactivationSysJob();
	}

	for (Transform* pChildTransform : m_transform.m_children)
		pChildTransform->m_pGameObject->UpdateActiveState(activeInHierarchy);
}

ComponentHandleBase GameObject::AddComponentImpl(IComponent* pComponent)
{
	m_components.push_back(pComponent);

	if (!this->IsActiveInHierarchy())
		pComponent->OffFlag(ComponentFlag::Enabled);

	IComponentManager* pComponentManager = pComponent->GetComponentManager();
	ComponentHandleBase hComponent = pComponentManager->RegisterToHandleTable(pComponent);

	if (this->IsPending())
		return hComponent;

	// 지연된 게임오브젝트가 아닌 경우에만 바로 포인터 활성화
	pComponentManager->AddToDirectAccessGroup(pComponent);

	if (pComponent->GetType() == ComponentType::MonoBehaviour)
	{
		MonoBehaviour* pMonoBehaviour = static_cast<MonoBehaviour*>(pComponent);

		pMonoBehaviour->Awake();            // 1. Awake는 활성화 여부와 관계 없이 호출

		if (pMonoBehaviour->IsEnabled())    // 2. Enabled된 상태로 씬에 배치된 경우 스크립트의 OnEnable() 호출 및 Start 큐에 등록
			pMonoBehaviour->OnEnableSysJob();   // 작업은 이 함수 내부에서
	}

	return hComponent;
}

void GameObject::OnDeploySysJob()
{
	assert(this->IsPending());

	this->OffFlag(GAMEOBJECT_FLAG::PENDING);

	if (this->IsActiveInHierarchy())
		GameObjectManager::GetInstance()->AddToActiveGroup(this);
	else
		GameObjectManager::GetInstance()->AddToInactiveGroup(this);
}

void GameObject::OnActivationSysJob()
{
	// 소유중인 모든 컴포넌트 활성화
	for (IComponent* pComponent : m_components)
		pComponent->Enable();

	if (this->IsPending())
		return;

	GameObjectManager::GetInstance()->MoveToActiveGroup(this);
}

void GameObject::OnDeactivationSysJob()
{
	// 소유중인 모든 컴포넌트 비활성화
	for (IComponent* pComponent : m_components)
		pComponent->Disable();

	if (this->IsPending())
		return;

	GameObjectManager::GetInstance()->MoveToInactiveGroup(this);
}
