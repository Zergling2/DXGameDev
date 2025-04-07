#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Pass
	{
	public:
		Pass()
			: m_topology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pHullShader(nullptr)
			, m_pDomainShader(nullptr)
			, m_pGeometryShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_pBlendState(nullptr)
			, m_blendFactor{ 1.0f, 1.0f, 1.0f, 1.0f }
			, m_mask(0xFFFFFFFF)
			, m_pRasterizerState(nullptr)
			, m_pDepthStencilState(nullptr)
			, m_stencilRef(0)
		{
		}
		virtual ~Pass() = default;

		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) { m_topology = topology; }
		void SetInputLayout(ID3D11InputLayout* pInputLayout) { m_pInputLayout = pInputLayout; }

		void SetVertexShader(ID3D11VertexShader* pVertexShader) { m_pVertexShader = pVertexShader; }
		void SetHullShader(ID3D11HullShader* pHullShader) { m_pHullShader = pHullShader; }
		void SetDomainShader(ID3D11DomainShader* pDomainShader) { m_pDomainShader = pDomainShader; }
		void SetGeometryShader(ID3D11GeometryShader* pGeometryShader) { m_pGeometryShader = pGeometryShader; }
		void SetPixelShader(ID3D11PixelShader* pPixelShader) { m_pPixelShader = pPixelShader; }

		void SetBlendState(ID3D11BlendState* pBlendState, const FLOAT blendFactor[4], UINT mask)
		{
			m_pBlendState = pBlendState;
			m_blendFactor[0] = blendFactor[0];
			m_blendFactor[1] = blendFactor[1];
			m_blendFactor[2] = blendFactor[2];
			m_blendFactor[3] = blendFactor[3];
			m_mask = mask;
		}
		void SetRasterizerState(ID3D11RasterizerState* pRasterizerState) { m_pRasterizerState = pRasterizerState; }
		void SetDepthStencilState(ID3D11DepthStencilState* pDepthStencilState, UINT stencilRef)
		{
			m_pDepthStencilState = pDepthStencilState;
			m_stencilRef = stencilRef;
		}

		void Apply(ID3D11DeviceContext* pDeviceContext);
	private:
		D3D11_PRIMITIVE_TOPOLOGY m_topology;
		ID3D11InputLayout* m_pInputLayout;

		ID3D11VertexShader* m_pVertexShader;
		ID3D11HullShader* m_pHullShader;
		ID3D11DomainShader* m_pDomainShader;
		ID3D11GeometryShader* m_pGeometryShader;
		ID3D11PixelShader* m_pPixelShader;

		ID3D11BlendState* m_pBlendState;
		FLOAT m_blendFactor[4];
		UINT m_mask;

		ID3D11RasterizerState* m_pRasterizerState;

		ID3D11DepthStencilState* m_pDepthStencilState;
		UINT m_stencilRef;
	};
}
