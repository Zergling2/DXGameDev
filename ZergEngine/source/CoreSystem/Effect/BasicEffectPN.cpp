#include <ZergEngine\CoreSystem\Effect\BasicEffectPN.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>

using namespace ze;

// BasicPN Effect
// VertexShader:
// - ToHcsPN
// 
// PixelShader:
// - UnlitPNNoMtl
// - LitPN

void BasicEffectPN::Init()
{
	m_dirtyFlag = DIRTY_FLAG::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VertexFormatType::PositionNormal);
	m_pVS = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::ToHcsPN);

	m_pPSUnlitPNNoMtl = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::UnlitPNNoMtl);
	m_pPSLitPN = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitPN);

	m_pCurrPS = m_pPSUnlitPNNoMtl;

	ID3D11Device* pDevice = GraphicDevice::GetInstance()->GetDeviceComInterface();
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

void XM_CALLCONV BasicEffectPN::SetAmbientLight(FXMVECTOR ambientLight) noexcept
{
	XMStoreFloat3(&m_cbPerFrameCache.ambientLight, ambientLight);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_FRAME;
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

	XMMATRIX vp = pCamera->GetViewMatrix() * pCamera->GetProjMatrix();
	Math::CalcFrustumPlanesFromViewProjMatrix(vp, m_cbPerCameraCache.worldSpaceFrustumPlane);

	XMStoreFloat3(&m_cbPerCameraCache.cameraPosW, pCameraOwner->m_transform.GetWorldPosition());
	m_cbPerCameraCache.tessMinDist = pCamera->GetMinDistanceTessellationToStart();
	m_cbPerCameraCache.tessMaxDist = pCamera->GetMaxDistanceTessellationToStart();
	m_cbPerCameraCache.minTessExponent = pCamera->GetMinimumTessellationExponent();
	m_cbPerCameraCache.maxTessExponent = pCamera->GetMaximumTessellationExponent();

	XMStoreFloat4x4A(&m_cbPerCameraCache.vp, ConvertToHLSLMatrix(vp));

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_CAMERA;
}

void XM_CALLCONV BasicEffectPN::SetWorldMatrix(FXMMATRIX w) noexcept
{
	XMStoreFloat4x4A(&m_cbPerMeshCache.w, ConvertToHLSLMatrix(w));			// HLSL 전치
	XMStoreFloat4x4A(&m_cbPerMeshCache.wInvTr, XMMatrixInverse(nullptr, w));	// 역행렬의 전치의 HLSL 전치

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_MESH;
}

void BasicEffectPN::SetMaterial(const Material* pMaterial)
{
	ID3D11PixelShader* pPS;

	if (!pMaterial)
	{
		pPS = m_pPSUnlitPNNoMtl;
	}
	else
	{
		pPS = m_pPSLitPN;

		// 재질 값 설정
		m_cbPerSubsetCache.mtl.diffuse = pMaterial->m_diffuse;
		m_cbPerSubsetCache.mtl.specular = pMaterial->m_specular;
		m_cbPerSubsetCache.mtl.reflect = pMaterial->m_reflect;

		m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
	}

	if (m_pCurrPS != pPS)
	{
		m_pCurrPS = pPS;
		m_dirtyFlag |= DIRTY_FLAG::PIXEL_SHADER;
	}
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
		case DIRTY_FLAG::PIXEL_SHADER:
			ApplyPixelShader(pDeviceContext);
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

void BasicEffectPN::KickedOutOfDeviceContext() noexcept
{
	m_dirtyFlag = DIRTY_FLAG::ALL;
}

void BasicEffectPN::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pCurrPS, nullptr, 0);
}

void BasicEffectPN::ApplyPixelShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->PSSetShader(m_pCurrPS, nullptr, 0);
}

void BasicEffectPN::ApplyPerFrameConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerFrame.Update(pDeviceContext, &m_cbPerFrameCache);
	ID3D11Buffer* const cbs[] = { m_cbPerFrame.GetComInterface() };

	// PerFrame 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->PSSetConstantBuffers(startSlot, 1, cbs);
}

void BasicEffectPN::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
	ID3D11Buffer* const cbs[] = { m_cbPerCamera.GetComInterface() };

	// PerCamera 상수버퍼 사용 셰이더
	constexpr UINT vsStartSlot = 0;
	constexpr UINT psStartSlot = 1;
	pDeviceContext->VSSetConstantBuffers(vsStartSlot, 1, cbs);
	pDeviceContext->PSSetConstantBuffers(psStartSlot, 1, cbs);
}

void BasicEffectPN::ApplyPerMeshConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerMesh.Update(pDeviceContext, &m_cbPerMeshCache);
	ID3D11Buffer* const cbs[] = { m_cbPerMesh.GetComInterface() };

	// PerMesh 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 1;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void BasicEffectPN::ApplyPerSubsetConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerSubset.Update(pDeviceContext, &m_cbPerSubsetCache);
	ID3D11Buffer* const cbs[] = { m_cbPerSubset.GetComInterface() };

	// PerSubset 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 2;
	pDeviceContext->PSSetConstantBuffers(startSlot, 1, cbs);
}
