#include <ZergEngine\CoreSystem\GamePlayBase\Component\MonoBehaviour.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>
#include <ZergEngine\CoreSystem\Runtime.h>

using namespace ze;

MonoBehaviour::MonoBehaviour(GameObject& owner)
	: IComponent(owner, MonoBehaviourManager::GetInstance()->AssignUniqueId())
	, m_startingQueueIndex((std::numeric_limits<uint32_t>::max)())
{
}

void MonoBehaviour::Awake()
{
}

void MonoBehaviour::Start()
{
}

void MonoBehaviour::FixedUpdate()
{
}

void MonoBehaviour::Update()
{
}

void MonoBehaviour::LateUpdate()
{
}

void MonoBehaviour::OnEnable()
{
}

void MonoBehaviour::OnDisable()
{
}

void MonoBehaviour::OnDestroy()
{
}

bool MonoBehaviour::SetResolution(uint32_t width, uint32_t height, DisplayMode mode)
{
	return Runtime::GetInstance()->__$$SetResolutionImpl(width, height, mode);
}

IComponentManager* MonoBehaviour::GetComponentManager() const
{
	return MonoBehaviourManager::GetInstance();
}

void MonoBehaviour::OnDeploySysJob()
{
	IComponent::OnDeploySysJob();

	MonoBehaviourManager::GetInstance()->AddToAwakeQueue(this);
}

void MonoBehaviour::OnEnableSysJob()
{
	IComponent::OnEnableSysJob();

    this->OnEnable();

    // Start() 함수가 호출된 적이 없고 Start() 함수 호출 대기열에 들어있지도 않은 경우
	if (this->IsStartCalled() == false && this->IsOnTheStartingQueue() == false)
		MonoBehaviourManager::GetInstance()->AddToStartQueue(this);
}

void MonoBehaviour::OnDisableSysJob()
{
	IComponent::OnDisableSysJob();

	this->OnDisable();
}
