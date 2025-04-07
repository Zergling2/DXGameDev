#include <ZergEngine\CoreSystem\LowLevelRenderer\Pass.h>

using namespace ze;

void Pass::Apply(ID3D11DeviceContext* pDeviceContext)
{
	assert(pDeviceContext != nullptr);

	// 프리미티브 토폴로지 설정
	pDeviceContext->IASetPrimitiveTopology(m_topology);

	// 인풋 레이아웃 설정
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// 래스터라이저 설정
	pDeviceContext->RSSetState(m_pRasterizerState);

	// 셰이더 설정
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(m_pHullShader, nullptr, 0);
	pDeviceContext->DSSetShader(m_pDomainShader, nullptr, 0);
	pDeviceContext->GSSetShader(m_pGeometryShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// 뎁스스텐실 기능 설정
	pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, m_stencilRef);

	// 블렌딩 설정
	pDeviceContext->OMSetBlendState(m_pBlendState, m_blendFactor, m_mask);
}
