#pragma once

#include <ZergEngine\CoreSystem\Resource\Texture.h>

namespace ze
{
	class RenderSettings
	{
		friend class Runtime;
		friend class Renderer;
	public:
		static RenderSettings* GetInstance() { return s_pInstance; }
	private:
		RenderSettings();
		~RenderSettings();

		static void CreateInstance();
		static void DestroyInstance();

		void Init();
		void UnInit();
	public:
		void SetSkybox(const Texture2D& cubeMapTexture) { m_skyboxCubeMap = cubeMapTexture; }
		Texture2D GetSkybox() const { return m_skyboxCubeMap; }
		void DetachSkybox() { m_skyboxCubeMap.Reset(); }
	private:
		static RenderSettings* s_pInstance;
		Texture2D m_skyboxCubeMap;
	};
}
