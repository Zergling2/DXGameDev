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

namespace ze
{
	class MeshRenderer;
	class Texture2D;

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
	private:
		ID3D11RasterizerState* m_pCullBackRS;
		ID3D11RasterizerState* m_pCullNoneRS;
		ID3D11RasterizerState* m_pWireFrameRS;
		ID3D11DepthStencilState* m_pBasicDSS;
		ID3D11DepthStencilState* m_pSkyboxDSS;
		ID3D11DepthStencilState* m_pCameraMergeDSS;
		ID3D11SamplerState* m_pSamplerState;
		ID3D11BlendState* m_pOpaqueBS;
		ID3D11BlendState* m_pAdditiveBS;
		float m_normalBlendFactor[4];

		EffectDeviceContext m_effectImmediateContext;

		BasicEffectP m_basicEffectP;
		BasicEffectPC m_basicEffectPC;
		BasicEffectPN m_basicEffectPN;
		BasicEffectPT m_basicEffectPT;
		BasicEffectPNT m_basicEffectPNT;
		// TerrainEffect m_terrainEffect;
		SkyboxEffect m_skyboxEffect;
		MSCameraMergeEffect m_msCameraMergeEffect;
	};

	extern RendererImpl Renderer;
}
