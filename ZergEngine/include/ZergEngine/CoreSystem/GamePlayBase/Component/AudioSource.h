#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class AudioSource : public IComponent
	{
	public:
		static constexpr ComponentType TYPE = ComponentType::AUDIO_SOURCE;
		static constexpr bool IsCreatable() { return true; }

		AudioSource() = default;
		virtual ~AudioSource() = default;

		virtual ComponentType GetType() const override { return ComponentType::AUDIO_SOURCE; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	};
}
