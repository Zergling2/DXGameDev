#include <ZergEngine\CoreSystem\Effect\BasicEffectPN.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>

using namespace ze;

// BasicPN Effect
// 1. VertexShader: VSTransformPNToHCS
// 2. PixelShader: PSColorPNFragment

void BasicEffectPN::Init()
{
	ID3D11Device* pDevice = GraphicDevice.GetDeviceComInterface();

	m_dirtyFlag = DIRTY_FLAG::ALL;

	m_pInputLayout = GraphicDevice.GetILComInterface(VERTEX_FORMAT_TYPE::POSITION_NORMAL);
	m_pVertexShader = GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::TRANSFORM_PN_TO_HCS);
	m_pPixelShader = GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::COLOR_PN_FRAGMENT);

	m_cbPerFrame.Init(pDevice);
	m_cbPerCamera.Init(pDevice);
	m_cbPerMesh.Init(pDevice);
	m_cbPerSubset.Init(pDevice);
}

void BasicEffectPN::Release()
{
	m_cbPerFrame.Release();
	m_cbPerCamera.Release();
	m_cbPerMesh.Release();
	m_cbPerSubset.Release();
}

void BasicEffectPN::SetDirectionalLight(const DirectionalLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.dlCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.dl[i] = pLights[i];

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_FRAME;
}

void BasicEffectPN::SetPointLight(const PointLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.plCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.pl[i] = pLights[i];

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_FRAME;
}

void BasicEffectPN::SetSpotLight(const SpotLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.slCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.sl[i] = pLights[i];

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_FRAME;
}

void BasicEffectPN::SetCamera(const Camera* pCamera) noexcept
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

void XM_CALLCONV BasicEffectPN::SetWorldMatrix(XMMATRIX w) noexcept
{
	XMStoreFloat4x4A(&m_cbPerMeshCache.w, ConvertToHLSLMatrix(w));			// HLSL 전치
	XMStoreFloat4x4A(&m_cbPerMeshCache.wInvTr, XMMatrixInverse(nullptr, w));	// 역행렬의 전치의 HLSL 전치

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_MESH;
}

void BasicEffectPN::UseMaterial(bool b) noexcept
{
	const bool oldMtlFlag = m_cbPerSubsetCache.mtl.mtlFlag & MATERIAL_FLAG::USE_MATERIAL;

	if (b)
		m_cbPerSubsetCache.mtl.mtlFlag |= MATERIAL_FLAG::USE_MATERIAL;
	else
		m_cbPerSubsetCache.mtl.mtlFlag &= ~MATERIAL_FLAG::USE_MATERIAL;

	if (oldMtlFlag != b)
		m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void XM_CALLCONV BasicEffectPN::SetAmbientColor(XMVECTOR ambient) noexcept
{
	XMStoreFloat4A(&m_cbPerSubsetCache.mtl.ambient, ambient);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void XM_CALLCONV BasicEffectPN::SetDiffuseColor(XMVECTOR diffuse) noexcept
{
	XMStoreFloat4A(&m_cbPerSubsetCache.mtl.diffuse, diffuse);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void XM_CALLCONV BasicEffectPN::SetSpecularColor(XMVECTOR specular) noexcept
{
	XMStoreFloat4A(&m_cbPerSubsetCache.mtl.specular, specular);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void XM_CALLCONV BasicEffectPN::SetReflection(XMVECTOR reflect) noexcept
{
	XMStoreFloat4A(&m_cbPerSubsetCache.mtl.reflect, reflect);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void BasicEffectPN::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
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
		case DIRTY_FLAG::CONSTANTBUFFER_PER_FRAME:
			ApplyPerFrameConstantBuffer(pDeviceContext);
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
}

void BasicEffectPN::KickedFromDeviceContext() noexcept
{
	m_dirtyFlag = DIRTY_FLAG::ALL;
}

void BasicEffectPN::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void BasicEffectPN::ApplyPerFrameConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerFrame.Update(pDeviceContext, &m_cbPerFrameCache);
	ID3D11Buffer* const cbArr[] = { m_cbPerFrame.GetComInterface() };

	// PerFrame 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->PSSetConstantBuffers(startSlot, 1, cbArr);
}

void BasicEffectPN::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
	ID3D11Buffer* const cbArr[] = { m_cbPerCamera.GetComInterface() };

	// PerCamera 상수버퍼 사용 셰이더
	constexpr UINT vsStartSlot = 0;
	constexpr UINT psStartSlot = 1;
	pDeviceContext->VSSetConstantBuffers(vsStartSlot, 1, cbArr);
	pDeviceContext->PSSetConstantBuffers(psStartSlot, 1, cbArr);
}

void BasicEffectPN::ApplyPerMeshConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerMesh.Update(pDeviceContext, &m_cbPerMeshCache);
	ID3D11Buffer* const cbArr[] = { m_cbPerMesh.GetComInterface() };

	// PerMesh 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 1;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbArr);
}

void BasicEffectPN::ApplyPerSubsetConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerSubset.Update(pDeviceContext, &m_cbPerSubsetCache);
	ID3D11Buffer* const cbArr[] = { m_cbPerSubset.GetComInterface() };

	// PerSubset 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 2;
	pDeviceContext->PSSetConstantBuffers(startSlot, 1, cbArr);
}
