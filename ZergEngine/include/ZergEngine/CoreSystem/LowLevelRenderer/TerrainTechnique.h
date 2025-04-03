#pragma once

#include <ZergEngine\CoreSystem\LowLevelRenderer\TechniqueInterface.h>
#include <ZergEngine\Common\EngineConstants.h>

namespace ze
{
	class Terrain;

	class TerrainTechnique : public ITechnique
	{
	public:
		TerrainTechnique() = default;
		virtual ~TerrainTechnique() = default;

		void SetViewProjMatrix(FXMMATRIX vp) { XMStoreFloat4x4A(&m_vp, vp); }
		inline void SetConstantBuffer(ID3D11Buffer* pCbPerTerrain) { m_pCbPerTerrain = pCbPerTerrain; }

		void Apply(const D3D11_VIEWPORT* pViewport, ID3D11RenderTargetView* pRTVColorBuffer, ID3D11DepthStencilView* pDSVColorBuffer,
			RASTERIZER_FILL_MODE rfm, RASTERIZER_CULL_MODE rcm, TEXTURE_FILTERING_OPTION tfo);
		void Render(const Terrain* pTerrain);
	private:
		XMFLOAT4X4A m_vp;
		ID3D11Buffer* m_pCbPerTerrain;
	};
}
