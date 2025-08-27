#pragma once

#include <ZergEngine\CoreSystem\DeviceContext\EffectDeviceContext.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectP.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPC.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPN.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPT.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPNT.h>
#include <ZergEngine\CoreSystem\Effect\TerrainEffect.h>
#include <ZergEngine\CoreSystem\Effect\SkyboxEffect.h>
#include <ZergEngine\CoreSystem\Effect\DrawQuadWithMSTextureEffect.h>
#include <ZergEngine\CoreSystem\Effect\ButtonEffect.h>
#include <ZergEngine\CoreSystem\Effect\ImageEffect.h>

namespace ze
{
	class MeshRenderer;
	class Terrain;
	class IUIObject;
	class Panel;
	class Text;
	class Button;
	class Image;

	class Renderer
	{
		friend class Runtime;
	public:
		static Renderer* GetInstance() { return s_pInstance; }
	private:
		Renderer();
		virtual ~Renderer();

		static void CreateInstance();
		static void DestroyInstance();

		void Init();
		void UnInit();

		void RenderFrame();

		void RenderVFPositionMesh(const MeshRenderer* pMeshRenderer);
		void RenderVFPositionColorMesh(const MeshRenderer* pMeshRenderer);
		void RenderVFPositionNormalMesh(const MeshRenderer* pMeshRenderer);
		void RenderVFPositionTexCoordMesh(const MeshRenderer* pMeshRenderer);
		void RenderVFPositionNormalTexCoordMesh(const MeshRenderer* pMeshRenderer);
		void RenderTerrain(const Terrain* pTerrain);
		void RenderSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV);
		void RenderPanel(const Panel* pPanel);
		void RenderImage(const Image* pImage);
		void RenderText(const Text* pText);
		void RenderButton(const Button* pButton);
	private:
		static Renderer* s_pInstance;

		ID3D11RasterizerState* m_pCullBackRS;
		ID3D11RasterizerState* m_pCullNoneRS;
		ID3D11RasterizerState* m_pWireFrameRS;
		ID3D11DepthStencilState* m_pDefaultDSS;
		ID3D11DepthStencilState* m_pSkyboxDSS;
		ID3D11DepthStencilState* m_pDepthReadOnlyDSS;
		ID3D11DepthStencilState* m_pNoDepthStencilTestDSS;
		ID3D11BlendState* m_pOpaqueBS;
		ID3D11BlendState* m_pAlphaBlendBS;
		ID3D11BlendState* m_pNoColorWriteBS;
		ID3D11Buffer* m_pButtonVB;

		EffectDeviceContext m_effectImmediateContext;

		BasicEffectP m_basicEffectP;
		BasicEffectPC m_basicEffectPC;
		BasicEffectPN m_basicEffectPN;
		BasicEffectPT m_basicEffectPT;
		BasicEffectPNT m_basicEffectPNT;
		TerrainEffect m_terrainEffect;
		SkyboxEffect m_skyboxEffect;
		DrawQuadWithMSTextureEffect m_drawQuadWithMSTextureEffect;
		ButtonEffect m_buttonEffect;
		ImageEffect m_imageEffect;
		std::vector<const IUIObject*> m_uiRenderQueue;
	};
}
