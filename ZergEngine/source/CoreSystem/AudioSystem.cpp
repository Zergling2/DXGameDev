#include <ZergEngine\CoreSystem\AudioSystem.h>
#include <ZergEngine\CoreSystem\Resource\AudioClip.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\AudioSourceManager.h>
#include <cassert>

using namespace ze;

AudioSystem* AudioSystem::s_pInstance = nullptr;

AudioSystem::AudioSystem()
	: m_upAudioEngine()
	, m_audioClips()
{
}

void AudioSystem::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new AudioSystem();
}

void AudioSystem::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

bool AudioSystem::Init()
{
	m_upAudioEngine.reset();
	m_upAudioEngine = std::make_unique<DirectX::AudioEngine>();

	return true;
}

void AudioSystem::Shutdown()
{
	m_upAudioEngine.reset();
}

void AudioSystem::Update()
{
	if (!m_upAudioEngine)
		return;

	if (!m_upAudioEngine->Update())
		if (m_upAudioEngine->IsCriticalError())
			RecreateAudioEngine();
}

std::shared_ptr<AudioClip> AudioSystem::GetAudioClip(PCWSTR path)
{
	auto iter = m_audioClips.find(path);

	if (iter == m_audioClips.end())
		return nullptr;
	else
		return iter->second;
}

std::shared_ptr<AudioClip> AudioSystem::CreateAudioClip(PCWSTR path)
{
	std::shared_ptr<AudioClip> spAudioClip = std::make_shared<AudioClip>();
	spAudioClip->CreateSoundEffect(m_upAudioEngine.get(), path);

	m_audioClips.insert(std::make_pair(std::wstring(path), spAudioClip));

	return spAudioClip;
}

void AudioSystem::RecreateAudioEngine()
{
	m_upAudioEngine.reset();
	m_upAudioEngine = std::make_unique<DirectX::AudioEngine>();

	// DirectX::SoundEffects Àç»ý¼º
	for (auto& pair : m_audioClips)
		pair.second->RecreateSoundEffect(m_upAudioEngine.get(), pair.first.c_str());

	AudioSourceManager::GetInstance()->RecreateSoundEffectInstances();
}
