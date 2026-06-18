#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class AudioSourceManager : public IComponentManager
	{
		friend class Runtime;
		friend class AudioSystem;
		friend class AudioSource;
	public:
		static AudioSourceManager* GetInstance() { return s_pInstance; }
	private:
		AudioSourceManager() = default;
		virtual ~AudioSourceManager() = default;
		
		static void CreateInstance();
		static void DestroyInstance();

		void RecreateSoundEffectInstances();
	private:
		static AudioSourceManager* s_pInstance;
	};
}
