#include <ZergEngine\CoreSystem\Manager\ComponentManager\AudioListenerManager.h>
#include <cassert>

using namespace ze;

AudioListenerManager* AudioListenerManager::s_pInstance = nullptr;

void AudioListenerManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new AudioListenerManager();
}

void AudioListenerManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}
