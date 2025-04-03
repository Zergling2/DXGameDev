#pragma once

#include <ZergEngine\CoreSystem\LowLevelRenderer\TechniqueInterface.h>
#include <ZergEngine\Common\EngineConstants.h>

namespace ze
{
	class MeshRenderer;

	class IStandardTechnique : public ITechnique
	{
	public:
		IStandardTechnique()
			: m_vp()
			, m_pCbPerMesh(nullptr)
			, m_pCbPerSubset(nullptr)
		{
		}
		virtual ~IStandardTechnique() = default;

		void SetViewProjMatrix(FXMMATRIX vp) { XMStoreFloat4x4A(&m_vp, vp); }
		inline void SetConstantBuffer(ID3D11Buffer* pCbPerMesh, ID3D11Buffer* pCbPerSubset)
		{
			m_pCbPerMesh = pCbPerMesh;
			m_pCbPerSubset = pCbPerSubset;
		}

		virtual void Apply(const D3D11_VIEWPORT* pViewport, ID3D11RenderTargetView* pRTVColorBuffer, ID3D11DepthStencilView* pDSVColorBuffer,
			RASTERIZER_FILL_MODE rfm, RASTERIZER_CULL_MODE rcm, TEXTURE_FILTERING_OPTION tfo);
		virtual void Render(const MeshRenderer* pMeshRenderer) = 0;
	protected:
		XMFLOAT4X4A m_vp;
		ID3D11Buffer* m_pCbPerMesh;
		ID3D11Buffer* m_pCbPerSubset;
	};
}
