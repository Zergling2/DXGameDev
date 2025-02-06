#include <ZergEngine\System\Shader\Shader.h>
#include <ZergEngine\System\Graphics.h>
#include <ZergEngine\System\GameObject.h>
#include <ZergEngine\System\Material.h>
#include <ZergEngine\System\Texture.h>
#include <ZergEngine\System\Mesh.h>
#include <ZergEngine\System\Skybox.h>
#include <ZergEngine\System\Terrain.h>
#include <ZergEngine\System\Component\Transform.h>
#include <ZergEngine\System\Component\MeshRenderer.h>
#include <ZergEngine\System\Debug.h>

using namespace zergengine;

static LPCWSTR const THIS_FILE_NAME = L"Graphics.cpp";

// Constant buffers
ComPtr<ID3D11Buffer> ConstantBuffer::PerFrame::s_cb;
ComPtr<ID3D11Buffer> ConstantBuffer::PerCamera::s_cb;
ComPtr<ID3D11Buffer> ConstantBuffer::PerMesh::s_cb;
ComPtr<ID3D11Buffer> ConstantBuffer::PerSubset::s_cb;
ComPtr<ID3D11Buffer> ConstantBuffer::PerRenderingResultMerge::s_cb;

// Vertex shaders
ComPtr<ID3D11VertexShader> ShaderComponent::VSSkyboxTransform::s_vs;
ComPtr<ID3D11VertexShader> ShaderComponent::VSTerrainTransform::s_vs;
ComPtr<ID3D11VertexShader> ShaderComponent::VSStandardTransformP::s_vs;
ComPtr<ID3D11VertexShader> ShaderComponent::VSStandardTransformPC::s_vs;
ComPtr<ID3D11VertexShader> ShaderComponent::VSStandardTransformPN::s_vs;
ComPtr<ID3D11VertexShader> ShaderComponent::VSStandardTransformPT::s_vs;
ComPtr<ID3D11VertexShader> ShaderComponent::VSRenderingResultMerge::s_vs;
ComPtr<ID3D11VertexShader> ShaderComponent::VSStandardTransformPNT::s_vs;

// Hull shaders
ComPtr<ID3D11HullShader> ShaderComponent::HSTerrainRendering::s_hs;

// Domain shaders
ComPtr<ID3D11DomainShader> ShaderComponent::DSTerrainRendering::s_ds;

// Pixel shaders
ComPtr<ID3D11PixelShader> ShaderComponent::PSSkyboxColoring::s_ps;
ComPtr<ID3D11PixelShader> ShaderComponent::PSTerrainColoring::s_ps;
ComPtr<ID3D11PixelShader> ShaderComponent::PSStandardColoringP::s_ps;
ComPtr<ID3D11PixelShader> ShaderComponent::PSStandardColoringPC::s_ps;
ComPtr<ID3D11PixelShader> ShaderComponent::PSStandardColoringPN::s_ps;
ComPtr<ID3D11PixelShader> ShaderComponent::PSStandardColoringPT::s_ps;
ComPtr<ID3D11PixelShader> ShaderComponent::PSMSRenderingResultMerge::s_ps;
ComPtr<ID3D11PixelShader> ShaderComponent::PSStandardColoringPNT::s_ps;

void XM_CALLCONV StandardPEffect::Render(FXMMATRIX vp, const MeshRenderer* pMeshRenderer)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr;

	// #############################
	// PerMesh 상수버퍼 업데이트
	const Transform& transform = pMeshRenderer->GetGameObject()->GetTransform();

	const XMMATRIX w =
		XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 순서 회전
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));

	hr = Graphics::GetImmediateContext()->Map(
		ConstantBuffer::PerMesh::s_cb.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	XMVECTOR det;
	ConstantBuffer::PerMesh* pCBPerMesh = reinterpret_cast<ConstantBuffer::PerMesh*>(mapped.pData);
	XMStoreFloat4x4A(&pCBPerMesh->cb_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&pCBPerMesh->cb_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));
	XMStoreFloat4x4A(&pCBPerMesh->cb_wvp, ConvertToHLSLMatrix(w * vp));
	Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerMesh::s_cb.Get(), 0);
	Graphics::GetImmediateContext()->VSSetConstantBuffers(ConstantBuffer::PerMesh::SLOT_NUM, 1, ConstantBuffer::PerMesh::s_cb.GetAddressOf());
	// #############################

	const Mesh* const pMesh = pMeshRenderer->m_mesh.get();
	// #############################
	// 버텍스 버퍼 설정
	const UINT stride[] = { sizeof(VertexFormat::Position) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vb[] = { pMesh->GetVertexBufferInterface() };
	Graphics::GetImmediateContext()->IASetVertexBuffers(0, 1, vb, stride, offset);
	// #############################

	// #############################
	// 인덱스 버퍼 설정
	Graphics::GetImmediateContext()->IASetIndexBuffer(pMesh->GetIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);
	// #############################

	// #############################
	// 인풋 레이아웃 설정
	assert(pMesh->GetVertexFormatType() != VERTEX_FORMAT_TYPE::UNKNOWN);
	Graphics::GetImmediateContext()->IASetInputLayout(InputLayout::GetInputLayout(pMesh->GetVertexFormatType()));
	// #############################

	// #############################
	// 프리미티브 토폴로지 설정
	// Graphics::GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// #############################

	// #############################
	// 버텍스 셰이더 및 픽셀 셰이더 설정
	Graphics::GetImmediateContext()->VSSetShader(ShaderComponent::VSStandardTransformP::GetInterface(), nullptr, 0);
	Graphics::GetImmediateContext()->PSSetShader(ShaderComponent::PSStandardColoringP::GetInterface(), nullptr, 0);
	// #############################

	for (const auto& subset : pMesh->m_subsets)
		Graphics::GetImmediateContext()->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
}

void XM_CALLCONV StandardPCEffect::Render(FXMMATRIX vp, const MeshRenderer* pMeshRenderer)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr;

	// #############################
	// PerMesh 상수버퍼 업데이트
	const Transform& transform = pMeshRenderer->GetGameObject()->GetTransform();

	const XMMATRIX w =
		XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 순서 회전
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));

	hr = Graphics::GetImmediateContext()->Map(
		ConstantBuffer::PerMesh::s_cb.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	XMVECTOR det;
	ConstantBuffer::PerMesh* pCBPerMesh = reinterpret_cast<ConstantBuffer::PerMesh*>(mapped.pData);
	XMStoreFloat4x4A(&pCBPerMesh->cb_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&pCBPerMesh->cb_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));
	XMStoreFloat4x4A(&pCBPerMesh->cb_wvp, ConvertToHLSLMatrix(w * vp));
	Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerMesh::s_cb.Get(), 0);
	Graphics::GetImmediateContext()->VSSetConstantBuffers(ConstantBuffer::PerMesh::SLOT_NUM, 1, ConstantBuffer::PerMesh::s_cb.GetAddressOf());
	// #############################

	const Mesh* const pMesh = pMeshRenderer->m_mesh.get();
	// #############################
	// 버텍스 버퍼 설정
	const UINT stride[] = { sizeof(VertexFormat::PositionColor) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vb[] = { pMesh->GetVertexBufferInterface() };
	Graphics::GetImmediateContext()->IASetVertexBuffers(0, 1, vb, stride, offset);
	// #############################

	// #############################
	// 인덱스 버퍼 설정
	Graphics::GetImmediateContext()->IASetIndexBuffer(pMesh->GetIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);
	// #############################

	// #############################
	// 인풋 레이아웃 설정
	assert(pMesh->GetVertexFormatType() != VERTEX_FORMAT_TYPE::UNKNOWN);
	Graphics::GetImmediateContext()->IASetInputLayout(InputLayout::GetInputLayout(pMesh->GetVertexFormatType()));
	// #############################

	// #############################
	// 프리미티브 토폴로지 설정
	// Graphics::GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// #############################
	
	// #############################
	// 버텍스 셰이더 및 픽셀 셰이더 설정
	Graphics::GetImmediateContext()->VSSetShader(ShaderComponent::VSStandardTransformPC::GetInterface(), nullptr, 0);
	Graphics::GetImmediateContext()->PSSetShader(ShaderComponent::PSStandardColoringPC::GetInterface(), nullptr, 0);
	// #############################

	for (const auto& subset : pMesh->m_subsets)
		Graphics::GetImmediateContext()->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
}

void XM_CALLCONV StandardPNEffect::Render(FXMMATRIX vp, const MeshRenderer* pMeshRenderer)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr;

	// #############################
	// PerMesh 상수버퍼 업데이트
	const Transform& transform = pMeshRenderer->GetGameObject()->GetTransform();

	const XMMATRIX w =
		XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 순서 회전
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));

	hr = Graphics::GetImmediateContext()->Map(
		ConstantBuffer::PerMesh::s_cb.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	XMVECTOR det;
	ConstantBuffer::PerMesh* pCBPerMesh = reinterpret_cast<ConstantBuffer::PerMesh*>(mapped.pData);
	XMStoreFloat4x4A(&pCBPerMesh->cb_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&pCBPerMesh->cb_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));
	XMStoreFloat4x4A(&pCBPerMesh->cb_wvp, ConvertToHLSLMatrix(w * vp));
	Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerMesh::s_cb.Get(), 0);
	Graphics::GetImmediateContext()->VSSetConstantBuffers(ConstantBuffer::PerMesh::SLOT_NUM, 1, ConstantBuffer::PerMesh::s_cb.GetAddressOf());
	// #############################

	const Mesh* const pMesh = pMeshRenderer->m_mesh.get();
	// #############################
	// 버텍스 버퍼 설정
	const UINT stride[] = { sizeof(VertexFormat::PositionNormal) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vb[] = { pMesh->GetVertexBufferInterface() };
	Graphics::GetImmediateContext()->IASetVertexBuffers(0, 1, vb, stride, offset);
	// #############################

	// #############################
	// 인덱스 버퍼 설정
	Graphics::GetImmediateContext()->IASetIndexBuffer(pMesh->GetIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);
	// #############################

	// #############################
	// 인풋 레이아웃 설정
	assert(pMesh->GetVertexFormatType() != VERTEX_FORMAT_TYPE::UNKNOWN);
	Graphics::GetImmediateContext()->IASetInputLayout(InputLayout::GetInputLayout(pMesh->GetVertexFormatType()));
	// #############################

	// #############################
	// 프리미티브 토폴로지 설정
	// Graphics::GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// #############################

	// #############################
	// 버텍스 셰이더 및 픽셀 셰이더 설정
	Graphics::GetImmediateContext()->VSSetShader(ShaderComponent::VSStandardTransformPN::GetInterface(), nullptr, 0);
	Graphics::GetImmediateContext()->PSSetShader(ShaderComponent::PSStandardColoringPN::GetInterface(), nullptr, 0);
	// #############################

	for (const auto& subset : pMesh->m_subsets)
	{
		hr = Graphics::GetImmediateContext()->Map(
			ConstantBuffer::PerSubset::s_cb.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return;
		}

		const Material* pMaterial = subset.m_material.get();

		ConstantBuffer::PerSubset* pCBPerSubset = reinterpret_cast<ConstantBuffer::PerSubset*>(mapped.pData);
		pCBPerSubset->cb_subsetMtl.InitializeMtlFlag();
		if (pMaterial)
		{
			pCBPerSubset->cb_subsetMtl.UseMaterial();
			pCBPerSubset->cb_subsetMtl.ambient = pMaterial->m_ambient;
			pCBPerSubset->cb_subsetMtl.diffuse = pMaterial->m_diffuse;
			pCBPerSubset->cb_subsetMtl.specular = pMaterial->m_specular;
		}
		Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerSubset::s_cb.Get(), 0);
		Graphics::GetImmediateContext()->PSSetConstantBuffers(ConstantBuffer::PerSubset::SLOT_NUM, 1, ConstantBuffer::PerSubset::s_cb.GetAddressOf());

		Graphics::GetImmediateContext()->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void XM_CALLCONV StandardPTEffect::Render(FXMMATRIX vp, const MeshRenderer* pMeshRenderer)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr;

	// #############################
	// PerMesh 상수버퍼 업데이트
	const Transform& transform = pMeshRenderer->GetGameObject()->GetTransform();

	const XMMATRIX w =
		XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 순서 회전
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));

	hr = Graphics::GetImmediateContext()->Map(
		ConstantBuffer::PerMesh::s_cb.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	XMVECTOR det;
	ConstantBuffer::PerMesh* pCBPerMesh = reinterpret_cast<ConstantBuffer::PerMesh*>(mapped.pData);
	XMStoreFloat4x4A(&pCBPerMesh->cb_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&pCBPerMesh->cb_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));
	XMStoreFloat4x4A(&pCBPerMesh->cb_wvp, ConvertToHLSLMatrix(w * vp));
	Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerMesh::s_cb.Get(), 0);
	Graphics::GetImmediateContext()->VSSetConstantBuffers(ConstantBuffer::PerMesh::SLOT_NUM, 1, ConstantBuffer::PerMesh::s_cb.GetAddressOf());
	// #############################

	const Mesh* const pMesh = pMeshRenderer->m_mesh.get();
	// #############################
	// 버텍스 버퍼 설정
	const UINT stride[] = { sizeof(VertexFormat::PositionTexCoord) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vb[] = { pMesh->GetVertexBufferInterface() };
	Graphics::GetImmediateContext()->IASetVertexBuffers(0, 1, vb, stride, offset);
	// #############################

	// #############################
	// 인덱스 버퍼 설정
	Graphics::GetImmediateContext()->IASetIndexBuffer(pMesh->GetIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);
	// #############################

	// #############################
	// 인풋 레이아웃 설정
	assert(pMesh->GetVertexFormatType() != VERTEX_FORMAT_TYPE::UNKNOWN);
	Graphics::GetImmediateContext()->IASetInputLayout(InputLayout::GetInputLayout(pMesh->GetVertexFormatType()));
	// #############################

	// #############################
	// 프리미티브 토폴로지 설정
	// Graphics::GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// #############################

	// #############################
	// 버텍스 셰이더 및 픽셀 셰이더 설정
	Graphics::GetImmediateContext()->VSSetShader(ShaderComponent::VSStandardTransformPT::GetInterface(), nullptr, 0);
	Graphics::GetImmediateContext()->PSSetShader(ShaderComponent::PSStandardColoringPT::GetInterface(), nullptr, 0);
	// #############################

	for (const auto& subset : pMesh->m_subsets)
	{
		hr = Graphics::GetImmediateContext()->Map(
			ConstantBuffer::PerSubset::s_cb.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return;
		}

		const Material* pMaterial = subset.m_material.get();

		ConstantBuffer::PerSubset* pCBPerSubset = reinterpret_cast<ConstantBuffer::PerSubset*>(mapped.pData);
		pCBPerSubset->cb_subsetMtl.InitializeMtlFlag();
		if (pMaterial)
		{
			pCBPerSubset->cb_subsetMtl.UseMaterial();
			pCBPerSubset->cb_subsetMtl.ambient = pMaterial->m_ambient;
			pCBPerSubset->cb_subsetMtl.diffuse = pMaterial->m_diffuse;
			pCBPerSubset->cb_subsetMtl.specular = pMaterial->m_specular;
			if (pMaterial->m_baseMap)
				pCBPerSubset->cb_subsetMtl.UseBaseMap();
		}
		Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerSubset::s_cb.Get(), 0);
		Graphics::GetImmediateContext()->PSSetConstantBuffers(ConstantBuffer::PerSubset::SLOT_NUM, 1, ConstantBuffer::PerSubset::s_cb.GetAddressOf());
		if (pMaterial != nullptr && pMaterial->m_baseMap)
		{
			ID3D11ShaderResourceView* srvs[] = { subset.m_material->m_baseMap->GetShaderResourceViewInterface() };
			Graphics::GetImmediateContext()->PSSetShaderResources(TextureSlotNumber::BASE_MAP_SLOT_NUM, 1, srvs);
		}

		Graphics::GetImmediateContext()->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void XM_CALLCONV StandardPNTEffect::Render(FXMMATRIX vp, const MeshRenderer* pMeshRenderer)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr;

	// #############################
	// PerMesh 상수버퍼 업데이트
	const Transform& transform = pMeshRenderer->GetGameObject()->GetTransform();

	const XMMATRIX w =
		XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 순서 회전
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));

	hr = Graphics::GetImmediateContext()->Map(
		ConstantBuffer::PerMesh::s_cb.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	XMVECTOR det;
	ConstantBuffer::PerMesh* pCBPerMesh = reinterpret_cast<ConstantBuffer::PerMesh*>(mapped.pData);
	XMStoreFloat4x4A(&pCBPerMesh->cb_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&pCBPerMesh->cb_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));
	XMStoreFloat4x4A(&pCBPerMesh->cb_wvp, ConvertToHLSLMatrix(w * vp));
	Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerMesh::s_cb.Get(), 0);
	Graphics::GetImmediateContext()->VSSetConstantBuffers(ConstantBuffer::PerMesh::SLOT_NUM, 1, ConstantBuffer::PerMesh::s_cb.GetAddressOf());
	// #############################

	const Mesh* const pMesh = pMeshRenderer->m_mesh.get();
	// #############################
	// 버텍스 버퍼 설정
	const UINT stride[] = { sizeof(VertexFormat::PositionNormalTexCoord) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vb[] = { pMesh->GetVertexBufferInterface() };
	Graphics::GetImmediateContext()->IASetVertexBuffers(0, 1, vb, stride, offset);
	// #############################

	// #############################
	// 인덱스 버퍼 설정
	Graphics::GetImmediateContext()->IASetIndexBuffer(pMesh->GetIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);
	// #############################

	// #############################
	// 인풋 레이아웃 설정
	assert(pMesh->GetVertexFormatType() != VERTEX_FORMAT_TYPE::UNKNOWN);
	Graphics::GetImmediateContext()->IASetInputLayout(InputLayout::GetInputLayout(pMesh->GetVertexFormatType()));
	// #############################

	// #############################
	// 버텍스 셰이더 및 픽셀 셰이더 설정
	Graphics::GetImmediateContext()->VSSetShader(ShaderComponent::VSStandardTransformPNT::GetInterface(), nullptr, 0);
	Graphics::GetImmediateContext()->PSSetShader(ShaderComponent::PSStandardColoringPNT::GetInterface(), nullptr, 0);
	// #############################

	for (const auto& subset : pMesh->m_subsets)
	{
		hr = Graphics::GetImmediateContext()->Map(
			ConstantBuffer::PerSubset::s_cb.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return;
		}

		const Material* pMaterial = subset.m_material.get();

		ConstantBuffer::PerSubset* pCBPerSubset = reinterpret_cast<ConstantBuffer::PerSubset*>(mapped.pData);
		pCBPerSubset->cb_subsetMtl.InitializeMtlFlag();
		if (pMaterial)
		{
			pCBPerSubset->cb_subsetMtl.UseMaterial();
			pCBPerSubset->cb_subsetMtl.ambient = pMaterial->m_ambient;
			pCBPerSubset->cb_subsetMtl.diffuse = pMaterial->m_diffuse;
			pCBPerSubset->cb_subsetMtl.specular = pMaterial->m_specular;
			if (pMaterial->m_baseMap)
				pCBPerSubset->cb_subsetMtl.UseBaseMap();
		}
		Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerSubset::s_cb.Get(), 0);
		Graphics::GetImmediateContext()->PSSetConstantBuffers(ConstantBuffer::PerSubset::SLOT_NUM, 1, ConstantBuffer::PerSubset::s_cb.GetAddressOf());

		if (pMaterial != nullptr && pMaterial->m_baseMap)
		{
			ID3D11ShaderResourceView* srvs[] = { subset.m_material->m_baseMap->GetShaderResourceViewInterface() };
			Graphics::GetImmediateContext()->PSSetShaderResources(TextureSlotNumber::BASE_MAP_SLOT_NUM, 1, srvs);
		}
		/*
		else	// D3D11 ERROR 오류 메시지 방지
		{
			ID3D11ShaderResourceView* srvs[] = { nullptr };
			Graphics::GetImmediateContext()->PSSetShaderResources(TextureSlotNumber::BASE_MAP_SLOT_NUM, 1, srvs);
		}
		*/

		OutputDebugStringW(L"Begin StandardPNTEffect DrawIndexed\n");
		Graphics::GetImmediateContext()->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
		OutputDebugStringW(L"End StandardPNTEffect DrawIndexed\n");
	}
}

void XM_CALLCONV SkyboxEffect::Render(FXMMATRIX vp, FXMVECTOR camPos, const Skybox* pSkybox)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr;

	Graphics::GetImmediateContext()->OMSetDepthStencilState(DepthStencilState::GetState(DepthStencilState::DEPTH_STENCIL_STATE_TYPE::SKYBOX).GetInterface(), 0);

	// #############################
	// PerSkybox 상수버퍼 업데이트
	const XMMATRIX w = XMMatrixTranslationFromVector(camPos);		// 스카이박스를 항상 카메라 위치로 이동

	hr = Graphics::GetImmediateContext()->Map(
		ConstantBuffer::PerSkybox::s_cb.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	ConstantBuffer::PerSkybox* pCBPerSkybox = reinterpret_cast<ConstantBuffer::PerSkybox*>(mapped.pData);
	// XMStoreFloat4x4A(&pCBPerSkybox->cb_w, ConvertToHLSLMatrix(w));		// 셰이더에서 미사용
	// XMStoreFloat4x4A(&pCBPerSkybox->cb_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));	// 셰이더에서 미사용
	XMStoreFloat4x4A(&pCBPerSkybox->cb_wvp, ConvertToHLSLMatrix(w * vp));
	Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerSkybox::s_cb.Get(), 0);
	Graphics::GetImmediateContext()->VSSetConstantBuffers(ConstantBuffer::PerSkybox::SLOT_NUM, 1, ConstantBuffer::PerSkybox::s_cb.GetAddressOf());
	// #############################
	
	// #############################
	// 버텍스 버퍼 설정
	const UINT stride[] = { sizeof(VertexFormat::Position) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vb[] = { SkyboxResource::GetVertexBufferInterface() };
	Graphics::GetImmediateContext()->IASetVertexBuffers(0, 1, vb, stride, offset);
	// #############################

	// #############################
	// 인덱스 버퍼 설정
	Graphics::GetImmediateContext()->IASetIndexBuffer(SkyboxResource::GetIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);
	// #############################

	// #############################
	// 인풋 레이아웃 설정
	Graphics::GetImmediateContext()->IASetInputLayout(InputLayout::GetInputLayout(VERTEX_FORMAT_TYPE::POSITION));
	// #############################

	// #############################
	// 프리미티브 토폴로지 설정
	Graphics::GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// #############################

	// #############################
	// 버텍스 셰이더 및 픽셀 셰이더 설정
	Graphics::GetImmediateContext()->VSSetShader(ShaderComponent::VSSkyboxTransform::GetInterface(), nullptr, 0);
	Graphics::GetImmediateContext()->CSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->GSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->DSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->HSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->PSSetShader(ShaderComponent::PSSkyboxColoring::GetInterface(), nullptr, 0);
	// #############################

	// #############################
	// 셰이더 리소스 설정
	ID3D11ShaderResourceView* srvs[] = { pSkybox->GetShaderResourceView() };
	Graphics::GetImmediateContext()->PSSetShaderResources(TextureSlotNumber::SKYBOX_CUBE_SLOT_NUM, 1, srvs);
	// #############################

	OutputDebugStringW(L"Begin SkyboxEffect DrawIndexed\n");
	Graphics::GetImmediateContext()->DrawIndexed(36, 0, 0);
	OutputDebugStringW(L"End SkyboxEffect DrawIndexed\n");
}

void XM_CALLCONV TerrainEffect::Render(FXMMATRIX vp, FXMVECTOR camPos, const Terrain* pTerrain)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// PerMesh 상수버퍼 업데이트
	const XMMATRIX w = XMMatrixTranslationFromVector(camPos);		// 스카이박스를 항상 카메라 위치로 이동

	hr = Graphics::GetImmediateContext()->Map(
		ConstantBuffer::PerMesh::s_cb.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	ConstantBuffer::PerMesh* pCBPerMesh = reinterpret_cast<ConstantBuffer::PerMesh*>(mapped.pData);
	// XMStoreFloat4x4A(&pCBPerMesh->cb_w, ConvertToHLSLMatrix(w));		// 셰이더에서 미사용
	// XMStoreFloat4x4A(&pCBPerMesh->cb_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));	// 셰이더에서 미사용
	XMStoreFloat4x4A(&pCBPerMesh->cb_wvp, ConvertToHLSLMatrix(vp));	// 월드변환은 생략 (지형이 이미 월드 공간에서 정의되었다고 가정하므로)
	Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerSkybox::s_cb.Get(), 0);
	Graphics::GetImmediateContext()->DSSetConstantBuffers(ConstantBuffer::PerSkybox::SLOT_NUM, 1, ConstantBuffer::PerSkybox::s_cb.GetAddressOf());

	// #############################
	// 버텍스 버퍼 설정
	const UINT stride[] = { sizeof(VertexFormat::Terrain) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vb[] = { pTerrain->GetPatchControlPointBufferInterface() };
	Graphics::GetImmediateContext()->IASetVertexBuffers(0, 1, vb, stride, offset);
	// #############################

	// #############################
	// 인덱스 버퍼 설정
	Graphics::GetImmediateContext()->IASetIndexBuffer(pTerrain->GetPatchControlPointIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);
	// #############################

	// #############################
	// 인풋 레이아웃 설정
	Graphics::GetImmediateContext()->IASetInputLayout(InputLayout::GetInputLayout(VERTEX_FORMAT_TYPE::TERRAIN));
	// #############################

	// #############################
	// 프리미티브 토폴로지 설정
	Graphics::GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	// #############################

	// #############################
	// 버텍스 셰이더 및 픽셀 셰이더 설정
	Graphics::GetImmediateContext()->VSSetShader(ShaderComponent::VSTerrainTransform::GetInterface(), nullptr, 0);
	Graphics::GetImmediateContext()->CSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->GSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->DSSetShader(ShaderComponent::DSTerrainRendering::GetInterface(), nullptr, 0);
	Graphics::GetImmediateContext()->HSSetShader(ShaderComponent::HSTerrainRendering::GetInterface(), nullptr, 0);
	Graphics::GetImmediateContext()->PSSetShader(ShaderComponent::PSTerrainColoring::GetInterface(), nullptr, 0);
	// #############################

	// #############################
	// 셰이더 리소스 설정
	ID3D11ShaderResourceView* srvs[] = { pTerrain->GetHeightMapShaderResourceView() };
	Graphics::GetImmediateContext()->VSSetShaderResources(TextureSlotNumber::HEIGHT_MAP_SLOT_NUM, 1, srvs);
	Graphics::GetImmediateContext()->DSSetShaderResources(TextureSlotNumber::HEIGHT_MAP_SLOT_NUM, 1, srvs);
	Graphics::GetImmediateContext()->PSSetShaderResources(TextureSlotNumber::HEIGHT_MAP_SLOT_NUM, 1, srvs);
	// #############################

	OutputDebugStringW(L"Begin TerrainEffect DrawIndexed\n");
	Graphics::GetImmediateContext()->DrawIndexed(pTerrain->GetPatchControlPointIndexCount(), 0, 0);
	OutputDebugStringW(L"End TerrainEffect DrawIndexed\n");
}
