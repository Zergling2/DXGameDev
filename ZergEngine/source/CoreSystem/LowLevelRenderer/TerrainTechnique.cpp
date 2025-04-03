#include <ZergEngine\CoreSystem\LowLevelRenderer\TerrainTechnique.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>

using namespace ze;

void TerrainTechnique::Apply(const D3D11_VIEWPORT* pViewport, ID3D11RenderTargetView* pRTVColorBuffer, ID3D11DepthStencilView* pDSVColorBuffer,
	RASTERIZER_FILL_MODE rfm, RASTERIZER_CULL_MODE rcm, TEXTURE_FILTERING_OPTION tfo)
{
	assert(this->GetDeviceContext() != nullptr);

	// 프리미티브 토폴로지 설정
	this->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	// 인풋 레이아웃 설정
	this->GetDeviceContext()->IASetInputLayout(GraphicDevice::GetInstance().GetILComInterface(VERTEX_FORMAT_TYPE::TERRAIN_CONTROL_POINT));

	// 렌더 스테이트 설정
	this->GetDeviceContext()->RSSetViewports(1, pViewport);
	this->GetDeviceContext()->RSSetState(GraphicDevice::GetInstance().GetRSComInterface(rfm, rcm));

	// 렌더 타겟 설정
	ID3D11RenderTargetView* const pRTVs[] = { pRTVColorBuffer };
	this->GetDeviceContext()->OMSetRenderTargets(_countof(pRTVs), pRTVs, pDSVColorBuffer);
	this->GetDeviceContext()->OMSetDepthStencilState(GraphicDevice::GetInstance().GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::STANDARD), 0);

	// 샘플러 설정
	ID3D11SamplerState* const vsdsss[] = { GraphicDevice::GetInstance().GetHeightmapSamplerComInterface() };
	ID3D11SamplerState* const psss[] = { GraphicDevice::GetInstance().GetSSComInterface(tfo), GraphicDevice::GetInstance().GetHeightmapSamplerComInterface() };
	this->GetDeviceContext()->VSSetSamplers(SamplerState::HeightmapSampler::GetSlotNumber(), _countof(vsdsss), vsdsss);
	this->GetDeviceContext()->DSSetSamplers(SamplerState::HeightmapSampler::GetSlotNumber(), _countof(vsdsss), vsdsss);
	this->GetDeviceContext()->PSSetSamplers(SamplerState::TerrainTextureSampler::GetSlotNumber(), _countof(psss), psss);

	// 셰이더 설정
	this->GetDeviceContext()->VSSetShader(
		GraphicDevice::GetInstance().GetVSComInterface(VERTEX_SHADER_TYPE::TERRAIN_TRANSFORM), nullptr, 0
	);
	this->GetDeviceContext()->PSSetShader(
		GraphicDevice::GetInstance().GetPSComInterface(PIXEL_SHADER_TYPE::TERRAIN_COLORING), nullptr, 0
	);
	this->GetDeviceContext()->HSSetShader(
		GraphicDevice::GetInstance().GetHSComInterface(HULL_SHADER_TYPE::TERRAIN_RENDERING), nullptr, 0
	);
	this->GetDeviceContext()->DSSetShader(
		GraphicDevice::GetInstance().GetDSComInterface(DOMAIN_SHADER_TYPE::TERRAIN_RENDERING), nullptr, 0
	);
	this->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
	this->GetDeviceContext()->CSSetShader(nullptr, nullptr, 0);
}

void TerrainTechnique::Render(const Terrain* pTerrain)
{
	/*
	assert(m_pCbPerTerrain != nullptr);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// 버텍스 버퍼 설정
	const UINT stride[] = { sizeof(VertexFormat::TerrainControlPoint) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vb[] = { pTerrain->GetPatchControlPointBufferInterface() };
	this->GetDeviceContext()->IASetVertexBuffers(0, 1, vb, stride, offset);

	// 인덱스 버퍼 설정
	this->GetDeviceContext()->IASetIndexBuffer(pTerrain->GetPatchControlPointIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerTerrain 상수버퍼 업데이트 및 바인딩
	ConstantBuffer::PerTerrain cbPerTerrain;
	
	// const XMMATRIX w = XMMatrixTranslationFromVector(camPos);
	// XMStoreFloat4x4A(&cbPerTerrain.cbpt_w, ConvertToHLSLMatrix(w));		// 셰이더에서 미사용
	// XMStoreFloat4x4A(&cbPerTerrain.cbpt_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));	// 셰이더에서 미사용
	XMStoreFloat4x4A(&cbPerTerrain.cbpt_wvp, ConvertToHLSLMatrix(XMLoadFloat4x4A(&m_vp)));	// 월드변환은 생략 (지형이 이미 월드 공간에서 정의되었다고 가정하므로)
	cbPerTerrain.cbpt_terrainTextureTiling = pTerrain->GetTextureScale();
	cbPerTerrain.cbpt_terrainCellSpacing = pTerrain->GetCellSpacing();
	cbPerTerrain.cbpt_terrainTexelSpacingU = pTerrain->GetTexelSpacingU();
	cbPerTerrain.cbpt_terrainTexelSpacingV = pTerrain->GetTexelSpacingV();
	hr = this->GetDeviceContext()->Map(
		m_pCbPerTerrain,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
		return;
	memcpy(mapped.pData, &cbPerTerrain, sizeof(cbPerTerrain));
	this->GetDeviceContext()->Unmap(m_pCbPerTerrain, 0);

	ID3D11Buffer* const cbs[] = { m_pCbPerTerrain };
	this->GetDeviceContext()->DSSetConstantBuffers(ConstantBuffer::PerTerrain::GetSlotNumber(), _countof(cbs), cbs);
	this->GetDeviceContext()->PSSetConstantBuffers(ConstantBuffer::PerTerrain::GetSlotNumber(), _countof(cbs), cbs);

	// 셰이더 리소스 설정
	ID3D11ShaderResourceView* const srvs[] = { pTerrain->GetHeightMapSRV() };
	this->GetDeviceContext()->VSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);
	this->GetDeviceContext()->DSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);
	this->GetDeviceContext()->PSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);

	// 드로우
	this->GetDeviceContext()->DrawIndexed(pTerrain->GetPatchControlPointIndexCount(), 0, 0);
	*/
}
