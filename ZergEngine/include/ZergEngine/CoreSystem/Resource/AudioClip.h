#pragma once

#include <memory>

namespace DirectX
{
	class AudioEngine;
	class SoundEffect;
	class SoundEffectInstance;
}

namespace ze
{
	class AudioClip
	{
		friend class AudioSystem;
	public:
		AudioClip() = default;
		~AudioClip() = default;

		void Play(float volume = 1.0f, float pitch = 0.0f, float pan = 0.0f);

		std::unique_ptr<DirectX::SoundEffectInstance> CreateSoundEffectInstance();
	private:
		void CreateSoundEffect(DirectX::AudioEngine* pAudioEngine, const wchar_t* path);
		void RecreateSoundEffect(DirectX::AudioEngine* pAudioEngine, const wchar_t* path);
	private:
		std::unique_ptr<DirectX::SoundEffect> m_upSoundEffect;
	};
}
