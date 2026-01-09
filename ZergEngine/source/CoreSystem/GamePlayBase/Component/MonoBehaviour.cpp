#include <ZergEngine\CoreSystem\GamePlayBase\Component\MonoBehaviour.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>
#include <ZergEngine\CoreSystem\Runtime.h>

using namespace ze;

MonoBehaviour::MonoBehaviour()
	: IComponent(MonoBehaviourManager::GetInstance()->AssignUniqueId())
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

void MonoBehaviour::Enable()
{
	MonoBehaviourManager::GetInstance()->RequestEnable(this);
}

void MonoBehaviour::Disable()
{
	MonoBehaviourManager::GetInstance()->RequestDisable(this);
}
