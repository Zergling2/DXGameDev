#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class RenderContext
	{
	public:
		RenderContext(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		~RenderContext() = default;

		void ClearCache();

		void SetInputLayout(ID3D11InputLayout* pInputLayout);


		// VB, IB...


		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology);
		// Rasterizer state...
		// depth stencil state...
		// blend state...

		void SetVertexShader(ID3D11VertexShader* pVertexShader);
		void SetHullShader(ID3D11HullShader* pHullShader);
		void SetDomainShader(ID3D11DomainShader* pDomainShader);
		void SetGeometryShader(ID3D11GeometryShader* pGeometryShader);
		void SetPixelShader(ID3D11PixelShader* pPixelShader);
	private:
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		// State cache
		ID3D11InputLayout* m_pCurrentInputLayout;
		ID3D11Buffer* m_pCurrentVertexBuffer;
		UINT m_currentVBStride;
		UINT m_currentVBOffset;
		ID3D11Buffer* m_pCurrentIndexBuffer;
		DXGI_FORMAT m_currentIBFormat;
		UINT m_currentIBOffset;

		ID3D11RasterizerState* m_pCurrentRasterizerState;
		ID3D11DepthStencilState* m_pCurrentDepthStencilState;
		UINT m_currentStencilRef;
		ID3D11BlendState* m_pCurrentBlendState;
		float m_currentBlendFactor[4];
		UINT m_currentSampleMask;
		D3D11_PRIMITIVE_TOPOLOGY m_currentPrimitiveTopology;

		ID3D11VertexShader* m_pCurrentVertexShader;
		ID3D11HullShader* m_pCurrentHullShader;
		ID3D11DomainShader* m_pCurrentDomainShader;
		ID3D11GeometryShader* m_pCurrentGeometryShader;
		ID3D11PixelShader* m_pCurrentPixelShader;
	};
}
