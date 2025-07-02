#pragma once

#include <ZergEngine\CoreSystem\Resource\Texture.h>

namespace ze
{
	class Environment
	{
		friend class Runtime;
		friend class Renderer;
	public:
		static Environment* GetInstance() { return s_pInstance; }
	private:
		Environment();
		~Environment();

		static void CreateInstance();
		static void DestroyInstance();

		void Init();
		void UnInit();
	public:
		void SetSkyboxCubeMap(const Texture2D& cubeMapTexture) { m_skyboxCubeMap = cubeMapTexture; }
		Texture2D GetSkyboxCubeMap() const { return m_skyboxCubeMap; }
		void RemoveSkybox() { m_skyboxCubeMap.Reset(); }
	private:
		static Environment* s_pInstance;
		Texture2D m_skyboxCubeMap;
	};
}
