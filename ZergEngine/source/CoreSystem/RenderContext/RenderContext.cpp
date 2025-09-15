#include <ZergEngine\CoreSystem\RenderContext\RenderContext.h>

using namespace ze;

RenderContext::RenderContext(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pCurrentInputLayout(nullptr)
	, m_pCurrentVertexBuffer(nullptr)
	, m_currentVBStride(0)
	, m_currentVBOffset(0)
	, m_pCurrentIndexBuffer(nullptr)
	, m_currentIBFormat(DXGI_FORMAT_UNKNOWN)
	, m_currentIBOffset(0)
	, m_pCurrentRasterizerState(nullptr)
	, m_pCurrentDepthStencilState(nullptr)
	, m_currentStencilRef(0)
	, m_pCurrentBlendState(nullptr)
	, m_currentBlendFactor{0.0f, 0.0f, 0.0f, 0.0f}
	, m_currentSampleMask(0xFFFFFFFF)
	, m_currentPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
	, m_pCurrentVertexShader(nullptr)
	, m_pCurrentHullShader(nullptr)
	, m_pCurrentDomainShader(nullptr)
	, m_pCurrentGeometryShader(nullptr)
	, m_pCurrentPixelShader(nullptr)
{
	this->ClearCache();
}

#include <stdio.h>

void RenderContext::ClearCache()
{
	m_pCurrentInputLayout = nullptr;
	m_pCurrentVertexBuffer = nullptr;
	m_currentVBStride = 0;
	m_currentVBOffset = 0;
	m_pCurrentIndexBuffer = nullptr;
	m_currentIBFormat = DXGI_FORMAT_UNKNOWN;
	m_currentIBOffset = 0;
	m_pCurrentRasterizerState = nullptr;
	m_pCurrentDepthStencilState = nullptr;
	m_currentStencilRef = 0;
	m_pCurrentBlendState = nullptr;
	m_currentBlendFactor[0] = m_currentBlendFactor[1] = m_currentBlendFactor[2] = m_currentBlendFactor[3] = 0.0f;
	m_currentSampleMask = 0xFFFFFFFF;
	m_currentPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	m_pCurrentVertexShader = nullptr;
	m_pCurrentHullShader = nullptr;
	m_pCurrentDomainShader = nullptr;
	m_pCurrentGeometryShader = nullptr;
	m_pCurrentPixelShader = nullptr;
}

void RenderContext::SetInputLayout(ID3D11InputLayout* pInputLayout)
{
	if (m_pCurrentInputLayout == pInputLayout)
		return;

	m_pDeviceContext->IASetInputLayout(pInputLayout);
	m_pCurrentInputLayout = pInputLayout;
}

void RenderContext::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	if (m_currentPrimitiveTopology == primitiveTopology)
		return;

	m_pDeviceContext->IASetPrimitiveTopology(primitiveTopology);
	m_currentPrimitiveTopology = primitiveTopology;
}

void RenderContext::SetVertexShader(ID3D11VertexShader* pVertexShader)
{
	if (m_pCurrentVertexShader == pVertexShader)
		return;

	m_pDeviceContext->VSSetShader(pVertexShader, nullptr, 0);
	m_pCurrentVertexShader = pVertexShader;
}

void RenderContext::SetHullShader(ID3D11HullShader* pHullShader)
{
	if (m_pCurrentHullShader == pHullShader)
		return;

	m_pDeviceContext->HSSetShader(pHullShader, nullptr, 0);
	m_pCurrentHullShader = pHullShader;
}

void RenderContext::SetDomainShader(ID3D11DomainShader* pDomainShader)
{
	if (m_pCurrentDomainShader == pDomainShader)
		return;

	m_pDeviceContext->DSSetShader(pDomainShader, nullptr, 0);
	m_pCurrentDomainShader = pDomainShader;
}

void RenderContext::SetGeometryShader(ID3D11GeometryShader* pGeometryShader)
{
	if (m_pCurrentGeometryShader == pGeometryShader)
		return;

	m_pDeviceContext->GSSetShader(pGeometryShader, nullptr, 0);
	m_pCurrentGeometryShader = pGeometryShader;
}

void RenderContext::SetPixelShader(ID3D11PixelShader* pPixelShader)
{
	if (m_pCurrentPixelShader == pPixelShader)
		return;

	m_pDeviceContext->PSSetShader(pPixelShader, nullptr, 0);
	m_pCurrentPixelShader = pPixelShader;
}
