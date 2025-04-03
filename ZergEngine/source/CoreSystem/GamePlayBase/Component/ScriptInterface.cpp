#include <ZergEngine\CoreSystem\GamePlayBase\Component\ScriptInterface.h>
#include <ZergEngine\CoreSystem\ComponentSystem\ScriptManager.h>

using namespace ze;

IScript::IScript()
{
	this->SetId(ScriptManager::GetInstance().AssignUniqueId());
}

IComponentManager* IScript::GetComponentManager() const
{
	return &ScriptManager::GetInstance();
}

void IScript::SystemJobOnEnabled()
{
}

void IScript::SystemJobOnDisabled()
{
}
