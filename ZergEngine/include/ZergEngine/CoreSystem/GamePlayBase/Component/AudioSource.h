#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class AudioSource : public IComponent
	{
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::AUDIO_SOURCE;
		static constexpr bool IsCreatable() { return true; }

		AudioSource() = default;
		virtual ~AudioSource() = default;

		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::AUDIO_SOURCE; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	};
}
