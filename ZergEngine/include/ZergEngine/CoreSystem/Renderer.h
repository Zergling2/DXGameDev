#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\DeviceContext\EffectDeviceContext.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectP.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPC.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPN.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPT.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPNT.h>
#include <ZergEngine\CoreSystem\Effect\SkyboxEffect.h>
#include <ZergEngine\CoreSystem\Effect\MSCameraMergeEffect.h>
#include <ZergEngine\CoreSystem\Effect\PanelEffect.h>
#include <ZergEngine\CoreSystem\Effect\ButtonEffect.h>
#include <ZergEngine\CoreSystem\Effect\ImageEffect.h>

namespace ze
{
	class MeshRenderer;
	class IUIObject;
	class Panel;
	class Button;
	class Image;

	class RendererImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(RendererImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void InitializeEffects();
		void ReleaseEffects();

		void RenderFrame();

		void RenderVFPositionMesh(const MeshRenderer* pMeshRenderer);
		void RenderVFPositionColorMesh(const MeshRenderer* pMeshRenderer);
		void RenderVFPositionNormalMesh(const MeshRenderer* pMeshRenderer);
		void RenderVFPositionTexCoordMesh(const MeshRenderer* pMeshRenderer);
		void RenderVFPositionNormalTexCoordMesh(const MeshRenderer* pMeshRenderer);
		void RenderSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV);
		void RenderPanel(const Panel* pPanel);
		void RenderButton(const Button* pButton);
		void RenderImage(const Image* pImage);
	private:
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
		// TerrainEffect mTerrainEffect;
		SkyboxEffect m_skyboxEffect;
		MSCameraMergeEffect m_msCameraMergeEffect;
		PanelEffect m_panelEffect;
		ButtonEffect m_buttonEffect;
		ImageEffect m_imageEffect;
		std::vector<const IUIObject*> m_uiRenderQueue;
	};

	extern RendererImpl Renderer;
}
