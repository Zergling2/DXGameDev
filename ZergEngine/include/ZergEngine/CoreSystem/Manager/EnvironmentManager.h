#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

namespace ze
{
	class EnvironmentManagerImpl : ISubsystem
	{
		ZE_DECLARE_SINGLETON(EnvironmentManagerImpl);
		friend class RuntimeImpl;
		friend class RendererImpl;
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	public:
		void SetSkyboxCubeMap(const Texture2D& cubeMapTexture) { m_skyboxCubeMap = cubeMapTexture; }
		Texture2D GetSkyboxCubeMap() const { return m_skyboxCubeMap; }
		void RemoveSkybox() { m_skyboxCubeMap.Reset(); }
	private:
		Texture2D m_skyboxCubeMap;
	};

	extern EnvironmentManagerImpl Environment;
}
