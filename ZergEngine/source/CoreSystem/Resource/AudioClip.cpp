#include <ZergEngine\CoreSystem\Resource\AudioClip.h>
#include <DirectXTK\Audio.h>

using namespace ze;

std::unique_ptr<DirectX::SoundEffectInstance> ze::AudioClip::CreateSoundEffectInstance()
{
	if (!m_upSoundEffect)
		return nullptr;

	return m_upSoundEffect->CreateInstance();
}

void AudioClip::CreateSoundEffect(DirectX::AudioEngine* pAudioEngine, const wchar_t* path)
{
	m_upSoundEffect = std::make_unique<DirectX::SoundEffect>(pAudioEngine, path);
}

void AudioClip::RecreateSoundEffect(DirectX::AudioEngine* pAudioEngine, const wchar_t* path)
{
	m_upSoundEffect.reset();

	CreateSoundEffect(pAudioEngine, path);
}
