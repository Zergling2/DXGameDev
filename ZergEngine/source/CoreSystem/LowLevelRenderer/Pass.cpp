#include <ZergEngine\CoreSystem\LowLevelRenderer\Pass.h>

using namespace ze;

void Pass::Apply(ID3D11DeviceContext* pDeviceContext)
{
	assert(pDeviceContext != nullptr);

	// ������Ƽ�� �������� ����
	pDeviceContext->IASetPrimitiveTopology(m_topology);

	// ��ǲ ���̾ƿ� ����
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// �����Ͷ����� ����
	pDeviceContext->RSSetState(m_pRasterizerState);

	// ���̴� ����
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(m_pHullShader, nullptr, 0);
	pDeviceContext->DSSetShader(m_pDomainShader, nullptr, 0);
	pDeviceContext->GSSetShader(m_pGeometryShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// �������ٽ� ��� ����
	pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, m_stencilRef);

	// ���� ����
	pDeviceContext->OMSetBlendState(m_pBlendState, m_blendFactor, m_mask);
}
