#include <ZergEngine\CoreSystem\Resource\AudioClip.h>
#include <DirectXTK\Audio.h>

using namespace ze;

void AudioClip::Play(float volume, float pitch, float pan)
{
	if (!m_upSoundEffect)
		return;

	m_upSoundEffect->Play(volume, pitch, pan);
}

std::unique_ptr<DirectX::SoundEffectInstance> AudioClip::CreateSoundEffectInstance()
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
