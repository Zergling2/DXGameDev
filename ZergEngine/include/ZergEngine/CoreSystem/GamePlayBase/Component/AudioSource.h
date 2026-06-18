#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <memory>

namespace DirectX
{
	class SoundEffectInstance;
}

namespace ze
{
	class AudioClip;

	enum class AudioSourceState
	{
		Stopped = 0,
		Playing,
		Paused,
		Unknown
	};

	class AudioSource : public IComponent
	{
	public:
		static constexpr ComponentType TYPE = ComponentType::AudioSource;
		static constexpr bool IsCreatable() { return true; }

		AudioSource(GameObject& owner);
		virtual ~AudioSource() = default;

		virtual ComponentType GetType() const override { return ComponentType::AudioSource; }

		void Play(bool loop = false);
		void Stop(bool immediate = true);
		void Pause();
		void Resume();

		float GetVolume() const { return m_volume; }
		void SetVolume(float volume);
		void SetPitch(float pitch);
		void SetPan(float pan);

		bool IsLooped() const;
		AudioSourceState GetState() const;

		std::shared_ptr<AudioClip> GetAudioClip() const { return m_spAudioClip; }
		void SetAudioClip(std::shared_ptr<AudioClip> spAudioClip);
		void RecreateSoundEffectInstance();
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		float m_volume;
		std::shared_ptr<AudioClip> m_spAudioClip;
		std::unique_ptr<DirectX::SoundEffectInstance> m_upSoundEffectInstance;
	};
}
