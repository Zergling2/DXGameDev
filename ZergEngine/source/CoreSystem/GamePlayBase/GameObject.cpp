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
	, m_groupIndex((std::numeric_limits<uint32_t>::max)())
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
	for (Transform* pChildTransform : m_transform.m_children)
		pChildTransform->m_pGameObject->SetActive(active);

	GameObjectManager::GetInstance()->SetActive(this, active);
}

const GameObjectHandle GameObject::ToHandle() const
{
	// 댕글링 포인터 접근 감지
	assert(GameObjectManager::GetInstance()->m_handleTable[m_tableIndex] == this);

	return GameObjectHandle(m_tableIndex, m_id);
}

ComponentHandleBase GameObject::AddComponentImpl(IComponent* pComponent)
{
	pComponent->m_pGameObject = this;
	m_components.push_back(pComponent);

	if (!this->IsActive())
		pComponent->OffFlag(ComponentFlag::Enabled);

	IComponentManager* pComponentManager = pComponent->GetComponentManager();
	ComponentHandleBase hComponent = pComponentManager->RegisterToHandleTable(pComponent);

	// 지연된 게임오브젝트가 아닌 경우에만 바로 포인터 활성화
	if (!this->IsPending())
	{
		pComponentManager->AddToDirectAccessGroup(pComponent);

		if (pComponent->GetType() == ComponentType::MonoBehaviour)
		{
			MonoBehaviour* pMonoBehaviour = static_cast<MonoBehaviour*>(pComponent);
			pMonoBehaviour->Awake();	// Awake는 활성화 여부와 관계 없이 호출
			if (pMonoBehaviour->IsEnabled())
			{
				pMonoBehaviour->OnEnable();
				static_cast<MonoBehaviourManager*>(pComponentManager)->AddToStartQueue(pMonoBehaviour);
			}
		}
	}

	return hComponent;
}
