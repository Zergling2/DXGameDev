#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class AudioListenerManager : public IComponentManager
	{
		friend class Runtime;
		friend class AudioListener;
	public:
		static AudioListenerManager* GetInstance() { return s_pInstance; }
	private:
		AudioListenerManager() = default;
		virtual ~AudioListenerManager() = default;

		static void CreateInstance();
		static void DestroyInstance();
	private:
		static AudioListenerManager* s_pInstance;
	};
}
