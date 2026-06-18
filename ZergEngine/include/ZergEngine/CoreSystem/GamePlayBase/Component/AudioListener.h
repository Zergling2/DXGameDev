#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class AudioListener : public IComponent
	{
	public:
		static constexpr ComponentType TYPE = ComponentType::AudioListener;
		static constexpr bool IsCreatable() { return true; }

		AudioListener(GameObject& owner);
		virtual ~AudioListener() = default;

		virtual ComponentType GetType() const override { return ComponentType::AudioListener; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	};
}
