#include <ZergEngine\CoreSystem\GamePlayBase\Component\ScriptInterface.h>
#include <ZergEngine\CoreSystem\ComponentSystem\ScriptManager.h>

using namespace ze;

IScript::IScript()
{
	this->SetId(ScriptManager.AssignUniqueId());
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
