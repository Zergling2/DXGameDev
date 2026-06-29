#include <ZergEngine\CoreSystem\GamePlayBase\Component\AudioSource.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\AudioSourceManager.h>
#include <ZergEngine\CoreSystem\Resource\AudioClip.h>
#include <DirectXTK\Audio.h>

using namespace ze;

AudioSource::AudioSource(GameObject& owner)
    : IComponent(owner, AudioSourceManager::GetInstance()->AssignUniqueId())
    , m_volume(1.0f)
    , m_spAudioClip()
    , m_upSoundEffectInstance()
{
}

void AudioSource::Play(bool loop)
{
    if (!m_upSoundEffectInstance)
        return;

    m_upSoundEffectInstance->Play(loop);
}

void AudioSource::Stop(bool immediate)
{
    if (!m_upSoundEffectInstance)
        return;

    m_upSoundEffectInstance->Stop(immediate);
}

void AudioSource::Pause()
{
    if (!m_upSoundEffectInstance)
        return;

    m_upSoundEffectInstance->Pause();
}

void AudioSource::Resume()
{
    if (!m_upSoundEffectInstance)
        return;

    m_upSoundEffectInstance->Resume();
}

void AudioSource::SetVolume(float volume)
{
    if (!m_upSoundEffectInstance)
        return;

    m_upSoundEffectInstance->SetVolume(volume);
    m_volume = volume;
}

void AudioSource::SetPitch(float pitch)
{
    if (!m_upSoundEffectInstance)
        return;

    m_upSoundEffectInstance->SetPitch(pitch);
}

void AudioSource::SetPan(float pan)
{
    if (!m_upSoundEffectInstance)
        return;

    m_upSoundEffectInstance->SetPan(pan);
}

bool AudioSource::IsLooped() const
{
    if (!m_upSoundEffectInstance)
        return false;
    
    return m_upSoundEffectInstance->IsLooped();
}

AudioSourceState AudioSource::GetState() const
{
    if (!m_upSoundEffectInstance)
        return AudioSourceState::Unknown;

    switch (m_upSoundEffectInstance->GetState())
    {
    case DirectX::SoundState::STOPPED:
        return AudioSourceState::Stopped;
    case DirectX::SoundState::PLAYING:
        return AudioSourceState::Playing;
    case DirectX::SoundState::PAUSED:
        return AudioSourceState::Paused;
    default:
        return AudioSourceState::Unknown;
    }
}

void AudioSource::SetAudioClip(std::shared_ptr<AudioClip> spAudioClip)
{
    if (m_spAudioClip == spAudioClip)
        return;

    m_spAudioClip = std::move(spAudioClip);

    if (!m_spAudioClip)
        return;

    m_upSoundEffectInstance = m_spAudioClip->CreateSoundEffectInstance();
}

void AudioSource::RecreateSoundEffectInstance()
{
    if (!m_spAudioClip)
        return;

    m_upSoundEffectInstance = m_spAudioClip->CreateSoundEffectInstance();
}

void AudioSource::OnDisableSysJob()
{
    this->Stop();

    IComponent::OnDisableSysJob();
}

IComponentManager* AudioSource::GetComponentManager() const
{
    return AudioSourceManager::GetInstance();
}
