#include <ZergEngine\CoreSystem\GamePlayBase\Component\ScriptInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ScriptManager.h>

using namespace ze;

IScript::IScript()
	: IComponent(ScriptManager.AssignUniqueId())
	, m_startingQueueIndex(std::numeric_limits<uint32_t>::max())
{
}

void IScript::Awake()
{
}

void IScript::Start()
{
}

void IScript::FixedUpdate()
{
}

void IScript::Update()
{
}

void IScript::LateUpdate()
{
}

void IScript::OnEnable()
{
}

void IScript::OnDisable()
{
}

void IScript::OnDestroy()
{
}

IComponentManager* IScript::GetComponentManager() const
{
	return &ScriptManager;
}

bool IScript::Enable()
{
	if (!IComponent::Enable())
		return false;

	this->OnEnable();

	// Start() �Լ��� ȣ��� ���� ���� Start() �Լ� ȣ�� ��⿭�� ��������� ���� ���
	if (!(this->GetFlag() & CF_START_CALLED) && !(this->GetFlag() & CF_ON_STARTING_QUEUE))
		ScriptManager.AddToStartingQueue(this);

	return true;
}

bool IScript::Disable()
{
	if (!IComponent::Disable())
		return false;

	this->OnDisable();

	return true;
}
