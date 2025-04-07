#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class Skybox;

	class EnvironmentManagerImpl : ISubsystem
	{
		ZE_DECLARE_SINGLETON(EnvironmentManagerImpl);
		friend class RuntimeImpl;
		friend class RendererImpl;
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	public:
		inline void SetSkybox(const std::shared_ptr<Skybox>& skybox) { m_spSkybox = skybox; }
		inline std::shared_ptr<Skybox> GetSkybox() const { return m_spSkybox; }

		inline void RemoveSkybox() { m_spSkybox.reset(); }
	private:
		std::shared_ptr<Skybox> m_spSkybox;
	};

	extern EnvironmentManagerImpl Environment;
}
