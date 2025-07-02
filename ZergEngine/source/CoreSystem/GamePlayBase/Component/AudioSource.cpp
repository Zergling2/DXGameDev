#include <ZergEngine\CoreSystem\GamePlayBase\Component\AudioSource.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\AudioSourceManager.h>

using namespace ze;

IComponentManager* AudioSource::GetComponentManager() const
{
    return AudioSourceManager::GetInstance();
}
