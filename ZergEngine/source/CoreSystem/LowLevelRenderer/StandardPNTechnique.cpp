#include <ZergEngine\CoreSystem\LowLevelRenderer\StandardPNTechnique.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Resource\Mesh.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>

using namespace ze;

void StandardPNTechnique::Apply(const D3D11_VIEWPORT* pViewport, ID3D11RenderTargetView* pRTVColorBuffer, ID3D11DepthStencilView* pDSVColorBuffer,
	RASTERIZER_FILL_MODE rfm, RASTERIZER_CULL_MODE rcm, TEXTURE_FILTERING_OPTION tfo)
{
	this->IStandardTechnique::Apply(pViewport, pRTVColorBuffer, pDSVColorBuffer, rfm, rcm, tfo);

	// 인풋 레이아웃 설정
	this->GetDeviceContext()->IASetInputLayout(GraphicDevice::GetInstance().GetILComInterface(VERTEX_FORMAT_TYPE::POSITION_NORMAL));

	// 셰이더 설정
	this->GetDeviceContext()->VSSetShader(
		GraphicDevice::GetInstance().GetVSComInterface(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PN), nullptr, 0
	);
	this->GetDeviceContext()->PSSetShader(
		GraphicDevice::GetInstance().GetPSComInterface(PIXEL_SHADER_TYPE::STANDARD_COLORING_PN), nullptr, 0
	);
}

void StandardPNTechnique::Render(const MeshRenderer* pMeshRenderer)
{
	assert(m_pCbPerMesh != nullptr && m_pCbPerSubset != nullptr);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();

	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() != VERTEX_FORMAT_TYPE::UNKNOWN);

	// 버텍스 버퍼 설정
	const UINT stride[] = { sizeof(VertexFormat::PositionNormal) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	this->GetDeviceContext()->IASetVertexBuffers(0, _countof(vbs), vbs, stride, offset);

	// 인덱스 버퍼 설정
	this->GetDeviceContext()->IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerMesh 상수버퍼 업데이트 및 바인딩
	ConstantBuffer::PerMesh cbPerMesh;
	const GameObject* pGameObject = pMeshRenderer->GetGameObjectHandle().ToPtr();
	const Transform& transform = pGameObject->GetTransform();
	const XMMATRIX w = XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 순서 회전
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));

	
	XMStoreFloat4x4A(&cbPerMesh.cbpm_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(nullptr, w)));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wvp, ConvertToHLSLMatrix(w * XMLoadFloat4x4A(&m_vp)));
	hr = this->GetDeviceContext()->Map(
		m_pCbPerMesh,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
		return;
	memcpy(mapped.pData, &cbPerMesh, sizeof(cbPerMesh));
	this->GetDeviceContext()->Unmap(m_pCbPerMesh, 0);

	ID3D11Buffer* cbs[] = { m_pCbPerMesh };
	this->GetDeviceContext()->VSSetConstantBuffers(ConstantBuffer::PerMesh::GetSlotNumber(), _countof(cbs), cbs);

	// 서브셋들 순회하며 렌더링
	for (const auto& subset : pMesh->m_subsets)
	{
		// PerSubset 상수버퍼 업데이트 및 바인딩
		ConstantBuffer::PerSubset cbPerSubset;
		const Material* pMaterial = subset.m_material.get();

		cbPerSubset.cbps_subsetMtl.Initialize();
		if (pMaterial != nullptr)
		{
			cbPerSubset.cbps_subsetMtl.UseMaterial();
			cbPerSubset.cbps_subsetMtl.ambient = pMaterial->m_ambient;
			cbPerSubset.cbps_subsetMtl.diffuse = pMaterial->m_diffuse;
			cbPerSubset.cbps_subsetMtl.specular = pMaterial->m_specular;
		}
		hr = this->GetDeviceContext()->Map(
			m_pCbPerSubset,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
			continue;
		memcpy(mapped.pData, &cbPerSubset, sizeof(cbPerSubset));
		this->GetDeviceContext()->Unmap(m_pCbPerSubset, 0);

		ID3D11Buffer* const cbs[] = { m_pCbPerSubset };
		this->GetDeviceContext()->PSSetConstantBuffers(ConstantBuffer::PerSubset::GetSlotNumber(), _countof(cbs), cbs);

		// 드로우
		this->GetDeviceContext()->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}
