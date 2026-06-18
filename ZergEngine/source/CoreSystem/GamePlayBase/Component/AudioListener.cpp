#include <ZergEngine\CoreSystem\GamePlayBase\Component\AudioListener.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\AudioListenerManager.h>

using namespace ze;

AudioListener::AudioListener(GameObject& owner)
    : IComponent(owner, AudioListenerManager::GetInstance()->AssignUniqueId())
{
}

IComponentManager* AudioListener::GetComponentManager() const
{
    return AudioListenerManager::GetInstance();
}
