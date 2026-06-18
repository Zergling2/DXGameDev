#pragma once

#include <DirectXTK/Audio.h>
#include <memory>
#include <unordered_map>

namespace ze
{
	class AudioClip;

	class AudioSystem
	{
		friend class Runtime;
		friend class ResourceLoader;
	public:
		static AudioSystem* GetInstance() { return s_pInstance; }

		AudioSystem();
	private:
		static void CreateInstance();
		static void DestroyInstance();

		bool Init();
		void Shutdown();
		void Update();

		std::shared_ptr<AudioClip> GetAudioClip(PCWSTR path);
		std::shared_ptr<AudioClip> CreateAudioClip(PCWSTR path);
		void RecreateAudioEngine();
	private:
		static AudioSystem* s_pInstance;
		std::unique_ptr<DirectX::AudioEngine> m_upAudioEngine;
		std::unordered_map<std::wstring, std::shared_ptr<AudioClip>> m_audioClips;
	};
}
