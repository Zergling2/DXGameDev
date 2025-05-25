#include <ZergEngine\CoreSystem\GamePlayBase\Component\MonoBehaviour.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>

using namespace ze;

MonoBehaviour::MonoBehaviour()
	: IComponent(MonoBehaviourManager.AssignUniqueId())
	, m_startingQueueIndex(std::numeric_limits<uint32_t>::max())
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

IComponentManager* MonoBehaviour::GetComponentManager() const
{
	return &MonoBehaviourManager;
}

bool MonoBehaviour::Enable()
{
	if (!IComponent::Enable())
		return false;

	this->OnEnable();

	// Start() �Լ��� ȣ��� ���� ���� Start() �Լ� ȣ�� ��⿭�� ��������� ���� ���
	if (this->IsStartCalled() == false && this->IsOnTheStartingQueue() == false)
		MonoBehaviourManager.AddToStartingQueue(this);

	return true;
}

bool MonoBehaviour::Disable()
{
	if (!IComponent::Disable())
		return false;

	this->OnDisable();

	return true;
}
