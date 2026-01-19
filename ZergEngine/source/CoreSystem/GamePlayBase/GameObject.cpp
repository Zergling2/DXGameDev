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

void GameObject::Destroy()
{
	if (this->IsPending())
		return;

	GameObjectManager::GetInstance()->RequestDestroy(this);
}

void GameObject::SetActive(bool active)
{
	// 자식 오브젝트들 재귀적 활성화
	for (Transform* pChildTransform : m_transform.m_children)
		pChildTransform->m_pGameObject->SetActive(active);

	// 이미 해당 활성 상태가 설정되어 있는 경우 함수 리턴
	if (this->IsActive() == active)
		return;

	if (active)
		this->OnFlag(GAMEOBJECT_FLAG::ACTIVE);
	else
		this->OffFlag(GAMEOBJECT_FLAG::ACTIVE);

	if (this->IsPending())	// 지연 오브젝트인 경우 플래그만 설정하고 리턴
		return;

	if (active)
		this->OnActivationSysJob();
	else
		this->OnDeactivationSysJob();
}

const GameObjectHandle GameObject::ToHandle() const
{
	// 댕글링 포인터 접근 감지
	assert(GameObjectManager::GetInstance()->m_handleTable[m_tableIndex] == this);

	return GameObjectHandle(m_tableIndex, m_id);
}

ComponentHandleBase GameObject::AddComponentImpl(IComponent* pComponent)
{
	m_components.push_back(pComponent);

	if (!this->IsActive())
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

	if (this->IsActive())
		GameObjectManager::GetInstance()->AddToActiveGroup(this);
	else
		GameObjectManager::GetInstance()->AddToInactiveGroup(this);
}

void GameObject::OnActivationSysJob()
{
	for (IComponent* pComponent : m_components)
		pComponent->Enable();

	GameObjectManager::GetInstance()->MoveToActiveGroup(this);
}

void GameObject::OnDeactivationSysJob()
{
	for (IComponent* pComponent : m_components)
		pComponent->Disable();

	GameObjectManager::GetInstance()->MoveToInactiveGroup(this);
}
