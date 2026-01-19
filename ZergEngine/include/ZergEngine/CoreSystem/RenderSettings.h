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
		void Shutdown();
	public:
		Texture2D GetSkybox() const { return m_skyboxCubeMap; }
		void SetSkybox(Texture2D cubeMapTexture) { m_skyboxCubeMap = std::move(cubeMapTexture); }
		void XM_CALLCONV SetAmbientLightColor(FXMVECTOR color) { XMStoreFloat3(&m_ambientLightColor, color); }
		void SetAmbientLightColor(const XMFLOAT3& color) { m_ambientLightColor = color; }
		const XMFLOAT3& GetAmbientLightColor() const { return m_ambientLightColor; }
		void SetAmbientLightIntensity(FLOAT intensity) { m_ambientLightIntensity = intensity; }
		FLOAT GetAmbientLightIntensity() const { return m_ambientLightIntensity; }
	private:
		static RenderSettings* s_pInstance;
		Texture2D m_skyboxCubeMap;
		XMFLOAT3 m_ambientLightColor;
		FLOAT m_ambientLightIntensity;
	};
}
