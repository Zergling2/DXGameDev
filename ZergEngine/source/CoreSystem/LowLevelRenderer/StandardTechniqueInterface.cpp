#include <ZergEngine\CoreSystem\LowLevelRenderer\StandardTechniqueInterface.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

void IStandardTechnique::Apply(const D3D11_VIEWPORT* pViewport, ID3D11RenderTargetView* pRTVColorBuffer, ID3D11DepthStencilView* pDSVColorBuffer,
	RASTERIZER_FILL_MODE rfm, RASTERIZER_CULL_MODE rcm, TEXTURE_FILTERING_OPTION tfo)
{
	assert(this->GetDeviceContext() != nullptr);

	// ������Ƽ�� �������� ����
	this->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���� ������Ʈ ����
	this->GetDeviceContext()->RSSetViewports(1, pViewport);
	this->GetDeviceContext()->RSSetState(GraphicDevice::GetInstance().GetRSComInterface(rfm, rcm));

	// ���� Ÿ�� ����
	ID3D11RenderTargetView* const pRTVs[] = { pRTVColorBuffer };
	this->GetDeviceContext()->OMSetRenderTargets(_countof(pRTVs), pRTVs, pDSVColorBuffer);
	this->GetDeviceContext()->OMSetDepthStencilState(GraphicDevice::GetInstance().GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::STANDARD), 0);

	// ���÷� ����
	ID3D11SamplerState* const ss[] = { GraphicDevice::GetInstance().GetSSComInterface(tfo) };
	this->GetDeviceContext()->PSSetSamplers(0, _countof(ss), ss);

	// ���̴� ����
	this->GetDeviceContext()->CSSetShader(nullptr, nullptr, 0);
	this->GetDeviceContext()->HSSetShader(nullptr, nullptr, 0);
	this->GetDeviceContext()->DSSetShader(nullptr, nullptr, 0);
	this->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
}
