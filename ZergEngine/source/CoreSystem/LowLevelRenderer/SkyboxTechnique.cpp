#include <ZergEngine\CoreSystem\LowLevelRenderer\SkyboxTechnique.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Resource\Skybox.h>

using namespace ze;

void SkyboxTechnique::Apply(const D3D11_VIEWPORT* pViewport, ID3D11RenderTargetView* pRTVColorBuffer, ID3D11DepthStencilView* pDSVColorBuffer,
	TEXTURE_FILTERING_OPTION tfo)
{
	assert(this->GetDeviceContext() != nullptr);

	// 프리미티브 토폴로지 설정
	this->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 인풋 레이아웃 설정
	this->GetDeviceContext()->IASetInputLayout(nullptr);

	// 렌더 스테이트 설정
	this->GetDeviceContext()->RSSetViewports(1, pViewport);
	this->GetDeviceContext()->RSSetState(GraphicDevice::GetInstance().GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));

	// 렌더 타겟 설정
	ID3D11RenderTargetView* const pRTVs[] = { pRTVColorBuffer };
	this->GetDeviceContext()->OMSetRenderTargets(_countof(pRTVs), pRTVs, pDSVColorBuffer);
	this->GetDeviceContext()->OMSetDepthStencilState(GraphicDevice::GetInstance().GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::SKYBOX), 0);
	
	// 샘플러 설정
	ID3D11SamplerState* const ss[] = { GraphicDevice::GetInstance().GetSkyboxSamplerComInterface() };
	this->GetDeviceContext()->PSSetSamplers(SamplerState::SkyboxSampler::GetSlotNumber(), _countof(ss), ss);

	// 셰이더 설정
	this->GetDeviceContext()->VSSetShader(
		GraphicDevice::GetInstance().GetVSComInterface(VERTEX_SHADER_TYPE::SKYBOX_TRANSFORM), nullptr, 0
	);
	this->GetDeviceContext()->PSSetShader(
		GraphicDevice::GetInstance().GetPSComInterface(PIXEL_SHADER_TYPE::SKYBOX_COLORING), nullptr, 0
	);
	this->GetDeviceContext()->HSSetShader(nullptr, nullptr, 0);
	this->GetDeviceContext()->DSSetShader(nullptr, nullptr, 0);
	this->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
	this->GetDeviceContext()->CSSetShader(nullptr, nullptr, 0);
}

void SkyboxTechnique::Render(FXMVECTOR cameraPos, const Skybox* pSkybox)
{
	assert(m_pCbPerSkybox != nullptr);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// PerSkybox 상수버퍼 업데이트 및 바인딩
	ConstantBuffer::PerSkybox cbPerSkybox;

	const XMMATRIX w = XMMatrixTranslationFromVector(cameraPos);		// 스카이박스를 항상 카메라 위치로 이동
	// XMStoreFloat4x4A(&cbPerSkybox.cbpsky_w, ConvertToHLSLMatrix(w));		// 셰이더에서 미사용
	// XMStoreFloat4x4A(&cbPerSkybox.cbpsky_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));	// 셰이더에서 미사용
	XMStoreFloat4x4A(&cbPerSkybox.cbpsky_wvp, ConvertToHLSLMatrix(w * XMLoadFloat4x4A(&m_vp)));

	hr = this->GetDeviceContext()->Map(
		m_pCbPerSkybox,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
		return;
	memcpy(mapped.pData, &cbPerSkybox, sizeof(cbPerSkybox));
	this->GetDeviceContext()->Unmap(m_pCbPerSkybox, 0);

	ID3D11Buffer* const cbs[] = { m_pCbPerSkybox };
	this->GetDeviceContext()->VSSetConstantBuffers(ConstantBuffer::PerSkybox::GetSlotNumber(), _countof(cbs), cbs);

	// 셰이더 리소스 설정
	ID3D11ShaderResourceView* const srvs[] = { pSkybox->GetSRVComInterface() };
	this->GetDeviceContext()->PSSetShaderResources(ShaderResourceSlot::SkyboxCubemapSlot::GetSlotNumber(), _countof(srvs), srvs);

	// 드로우
	this->GetDeviceContext()->Draw(36, 0);
}
