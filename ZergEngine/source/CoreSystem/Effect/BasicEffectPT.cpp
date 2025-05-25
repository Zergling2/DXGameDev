#include <ZergEngine\CoreSystem\Effect\BasicEffectPT.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>

using namespace ze;

// BasicPT Effect
// 1. VertexShader: VSTransformPTToHCS
// 2. PixelShader: PSColorPTFragment

void BasicEffectPT::Init()
{
	ID3D11Device* pDevice = GraphicDevice.GetDeviceComInterface();

	m_dirtyFlag = DIRTY_FLAG::ALL;

	m_pInputLayout = GraphicDevice.GetILComInterface(VERTEX_FORMAT_TYPE::POSITION_TEXCOORD);
	m_pVertexShader = GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::TRANSFORM_PT_TO_HCS);
	m_pPixelShader = GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT);

	m_cbPerCamera.Init(pDevice);
	m_cbPerMesh.Init(pDevice);
	m_cbPerSubset.Init(pDevice);

	ClearTextureSRVArray();
}

void BasicEffectPT::Release()
{
	m_cbPerCamera.Release();
	m_cbPerMesh.Release();
	m_cbPerSubset.Release();
}

void BasicEffectPT::SetCamera(const Camera* pCamera) noexcept
{
	const GameObject* pCameraOwner = pCamera->m_pGameObject;
	assert(pCameraOwner != nullptr);

	XMFLOAT4A frustumPlanes[6];
	XMFLOAT4X4A vp;
	XMStoreFloat4x4A(&vp, pCamera->GetViewMatrix() * pCamera->GetProjMatrix());
	Math::ExtractFrustumPlanesInWorldSpace(&vp, frustumPlanes);

	XMStoreFloat3(&m_cbPerCameraCache.cameraPosW, pCameraOwner->m_transform.GetWorldPosition());
	m_cbPerCameraCache.tessMinDist = pCamera->GetMinimumDistanceForTessellationToStart();
	m_cbPerCameraCache.tessMaxDist = pCamera->GetMaximumDistanceForTessellationToStart();
	m_cbPerCameraCache.minTessExponent = pCamera->GetMinimumTessellationExponent();
	m_cbPerCameraCache.maxTessExponent = pCamera->GetMaximumTessellationExponent();
	m_cbPerCameraCache.frustumPlane[0] = frustumPlanes[0];
	m_cbPerCameraCache.frustumPlane[1] = frustumPlanes[1];
	m_cbPerCameraCache.frustumPlane[2] = frustumPlanes[2];
	m_cbPerCameraCache.frustumPlane[3] = frustumPlanes[3];
	m_cbPerCameraCache.frustumPlane[4] = frustumPlanes[4];
	m_cbPerCameraCache.frustumPlane[5] = frustumPlanes[5];

	XMStoreFloat4x4A(&m_cbPerCameraCache.vp, XMMatrixTranspose(XMLoadFloat4x4A(&vp)));

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_CAMERA;
}

void XM_CALLCONV BasicEffectPT::SetWorldMatrix(FXMMATRIX w) noexcept
{
	XMStoreFloat4x4A(&m_cbPerMeshCache.w, ConvertToHLSLMatrix(w));			// HLSL 전치
	XMStoreFloat4x4A(&m_cbPerMeshCache.wInvTr, XMMatrixInverse(nullptr, w));	// 역행렬의 전치의 HLSL 전치

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_MESH;
}

void BasicEffectPT::UseMaterial(bool b) noexcept
{
	const bool oldMtlFlag = m_cbPerSubsetCache.mtl.mtlFlag & MATERIAL_FLAG::USE_MATERIAL;

	if (b)
		m_cbPerSubsetCache.mtl.mtlFlag |= MATERIAL_FLAG::USE_MATERIAL;
	else
		m_cbPerSubsetCache.mtl.mtlFlag = MATERIAL_FLAG::NONE;

	if (oldMtlFlag != b)
		m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void BasicEffectPT::SetLightMap(const Texture2D& lightMap) noexcept
{
	m_pTextureSRVArray[0] = lightMap.GetSRVComInterface();

	if (lightMap.GetSRVComInterface())
		m_cbPerSubsetCache.mtl.mtlFlag |= MATERIAL_FLAG::USE_LIGHT_MAP;
	else
		m_cbPerSubsetCache.mtl.mtlFlag &= ~MATERIAL_FLAG::USE_LIGHT_MAP;
}

void BasicEffectPT::SetDiffuseMap(const Texture2D& diffuseMap) noexcept
{
	m_pTextureSRVArray[1] = diffuseMap.GetSRVComInterface();

	if (diffuseMap.GetSRVComInterface())
		m_cbPerSubsetCache.mtl.mtlFlag |= MATERIAL_FLAG::USE_DIFFUSE_MAP;
	else
		m_cbPerSubsetCache.mtl.mtlFlag &= ~MATERIAL_FLAG::USE_DIFFUSE_MAP;
}

void BasicEffectPT::SetNormalMap(const Texture2D& normalMap) noexcept
{
	m_pTextureSRVArray[2] = normalMap.GetSRVComInterface();

	if (normalMap.GetSRVComInterface())
		m_cbPerSubsetCache.mtl.mtlFlag |= MATERIAL_FLAG::USE_NORMAL_MAP;
	else
		m_cbPerSubsetCache.mtl.mtlFlag &= ~MATERIAL_FLAG::USE_NORMAL_MAP;
}

void BasicEffectPT::SetSpecularMap(const Texture2D& specularMap) noexcept
{
	m_pTextureSRVArray[3] = specularMap.GetSRVComInterface();

	if (specularMap.GetSRVComInterface())
		m_cbPerSubsetCache.mtl.mtlFlag |= MATERIAL_FLAG::USE_SPECULAR_MAP;
	else
		m_cbPerSubsetCache.mtl.mtlFlag &= ~MATERIAL_FLAG::USE_SPECULAR_MAP;
}

void BasicEffectPT::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
{
	DWORD index;

	while (BitScanForward(&index, m_dirtyFlag))
	{
		switch (static_cast<DWORD>(1) << index)
		{
		case DIRTY_FLAG::PRIMITIVE_TOPOLOGY:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
		case DIRTY_FLAG::INPUT_LAYOUT:
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			break;
		case DIRTY_FLAG::SHADER:
			ApplyShader(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_CAMERA:
			ApplyPerCameraConstantBuffer(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_MESH:
			ApplyPerMeshConstantBuffer(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET:
			ApplyPerSubsetConstantBuffer(pDeviceContext);
			break;
		default:
			assert(false);
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}

	// 텍스처 리소스는 항상 재설정 (포인터를 들고 있어봤자 메모리 재사용 등으로 인해 이전 리소스 그대로인지 확신할 수 없음..)
	/*
	Texture2D tex2d_lightMap : register(t0);
	Texture2D tex2d_diffuseMap : register(t1);
	Texture2D tex2d_normalMap : register(t2);
	Texture2D tex2d_specularMap : register(t3);
	*/
	pDeviceContext->PSSetShaderResources(0, _countof(m_pTextureSRVArray), m_pTextureSRVArray);

	ClearTextureSRVArray();
}

void BasicEffectPT::KickedOutOfDeviceContext() noexcept
{
	ClearTextureSRVArray();

	m_dirtyFlag = DIRTY_FLAG::ALL;
}

void BasicEffectPT::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void BasicEffectPT::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
	ID3D11Buffer* const cbs[] = { m_cbPerCamera.GetComInterface() };

	// PerCamera 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void BasicEffectPT::ApplyPerMeshConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerMesh.Update(pDeviceContext, &m_cbPerMeshCache);
	ID3D11Buffer* const cbs[] = { m_cbPerMesh.GetComInterface() };

	// PerMesh 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 1;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void BasicEffectPT::ApplyPerSubsetConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerSubset.Update(pDeviceContext, &m_cbPerSubsetCache);
	ID3D11Buffer* const cbs[] = { m_cbPerSubset.GetComInterface() };

	// PerSubset 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->PSSetConstantBuffers(startSlot, 1, cbs);
}
