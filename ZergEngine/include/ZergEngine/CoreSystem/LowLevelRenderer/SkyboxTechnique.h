#pragma once

#include <ZergEngine\CoreSystem\LowLevelRenderer\TechniqueInterface.h>
#include <ZergEngine\Common\EngineConstants.h>

namespace ze
{
	class Skybox;

	class SkyboxTechnique : public ITechnique
	{
	public:
		SkyboxTechnique()
			: m_vp()
			, m_pCbPerSkybox(nullptr)
		{
		}
		virtual ~SkyboxTechnique() = default;

		void SetViewProjMatrix(FXMMATRIX vp) { XMStoreFloat4x4A(&m_vp, vp); }
		inline void SetConstantBuffer(ID3D11Buffer* pCbPerSkybox) { m_pCbPerSkybox = pCbPerSkybox; }

		void Apply(const D3D11_VIEWPORT* pViewport, ID3D11RenderTargetView* pRTVColorBuffer, ID3D11DepthStencilView* pDSVColorBuffer,
			TEXTURE_FILTERING_OPTION tfo);
		void Render(FXMVECTOR cameraPos, const Skybox* pSkybox);
	private:
		XMFLOAT4X4A m_vp;
		ID3D11Buffer* m_pCbPerSkybox;
	};
}
