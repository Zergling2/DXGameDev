#include <ZergEngine\CoreSystem\Manager\ComponentManager\AudioSourceManager.h>

using namespace ze;

AudioSourceManager* AudioSourceManager::s_pInstance = nullptr;

void AudioSourceManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new AudioSourceManager();
}

void AudioSourceManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}
