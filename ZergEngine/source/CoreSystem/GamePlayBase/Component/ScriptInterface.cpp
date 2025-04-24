#include <ZergEngine\CoreSystem\GamePlayBase\Component\ScriptInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ScriptManager.h>

using namespace ze;

IScript::IScript()
	: IComponent(ScriptManager.AssignUniqueId())
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

void IScript::SystemJobOnEnabled()
{
}

void IScript::SystemJobOnDisabled()
{
}
