#include <ZergEngine\CoreSystem\LowLevelRenderer\StandardPTechnique.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Resource\Mesh.h>

using namespace ze;

void StandardPTechnique::Apply(const D3D11_VIEWPORT* pViewport, ID3D11RenderTargetView* pRTVColorBuffer, ID3D11DepthStencilView* pDSVColorBuffer,
	RASTERIZER_FILL_MODE rfm, RASTERIZER_CULL_MODE rcm, TEXTURE_FILTERING_OPTION tfo)
{
	this->IStandardTechnique::Apply(pViewport, pRTVColorBuffer, pDSVColorBuffer, rfm, rcm, tfo);

	// ��ǲ ���̾ƿ� ����
	this->GetDeviceContext()->IASetInputLayout(GraphicDevice::GetInstance().GetILComInterface(VERTEX_FORMAT_TYPE::POSITION));

	// ���̴� ����
	this->GetDeviceContext()->VSSetShader(
		GraphicDevice::GetInstance().GetVSComInterface(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_P), nullptr, 0
	);
	this->GetDeviceContext()->PSSetShader(
		GraphicDevice::GetInstance().GetPSComInterface(PIXEL_SHADER_TYPE::STANDARD_COLORING_P), nullptr, 0
	);
}

void StandardPTechnique::Render(const MeshRenderer* pMeshRenderer)
{
	assert(m_pCbPerMesh != nullptr && m_pCbPerSubset != nullptr);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();

	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() != VERTEX_FORMAT_TYPE::UNKNOWN);

	// ���ؽ� ���� ����
	const UINT stride[] = { sizeof(VertexFormat::Position) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	this->GetDeviceContext()->IASetVertexBuffers(0, _countof(vbs), vbs, stride, offset);

	// �ε��� ���� ����
	this->GetDeviceContext()->IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerMesh ������� ������Ʈ �� ���ε�
	ConstantBuffer::PerMesh cbPerMesh;
	const GameObject* pGameObject = pMeshRenderer->GetGameObjectHandle().ToPtr();
	const Transform& transform = pGameObject->GetTransform();
	const XMMATRIX w = XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) ���� ȸ��
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

	// ����µ� ��ȸ�ϸ� ������
	for (const auto& subset : pMesh->m_subsets)
		this->GetDeviceContext()->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
}
