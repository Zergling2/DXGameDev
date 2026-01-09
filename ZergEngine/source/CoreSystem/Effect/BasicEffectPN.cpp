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
	m_dirtyFlag = DirtyFlag::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VertexFormatType::PositionNormal);
	m_pVS = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::ToHcsPN);

	m_pPSUnlitPNNoMtl = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::UnlitPNNoMtl);
	m_pPSLitPN = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitPN);

	m_pCurrPS = m_pPSUnlitPNNoMtl;

	ID3D11Device* pDevice = GraphicDevice::GetInstance()->GetDeviceComInterface();
	m_cbPerFrame.Init(pDevice);
	m_cbPerCamera.Init(pDevice);
	m_cbPerMesh.Init(pDevice);
	m_cbMaterial.Init(pDevice);
}

void BasicEffectPN::Release()
{
	m_cbPerFrame.Release();
	m_cbPerCamera.Release();
	m_cbPerMesh.Release();
	m_cbMaterial.Release();
}

void XM_CALLCONV BasicEffectPN::SetAmbientLight(FXMVECTOR ambientLight) noexcept
{
	XMStoreFloat3(&m_cbPerFrameCache.ambientLight, ambientLight);

	m_dirtyFlag |= DirtyFlag::UpdateCBPerFrame;
}

void BasicEffectPN::SetDirectionalLight(const DirectionalLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.dlCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.dl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::UpdateCBPerFrame;
}

void BasicEffectPN::SetPointLight(const PointLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.plCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.pl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::UpdateCBPerFrame;
}

void BasicEffectPN::SetSpotLight(const SpotLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.slCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.sl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::UpdateCBPerFrame;
}

void BasicEffectPN::SetCamera(const Camera* pCamera) noexcept
{
	const GameObject* pCameraOwner = pCamera->m_pGameObject;
	assert(pCameraOwner != nullptr);

	XMMATRIX vp = pCamera->GetViewMatrix() * pCamera->GetProjMatrix();
	Math::ComputeFrustumPlanesFromViewProjMatrix(vp, m_cbPerCameraCache.worldSpaceFrustumPlane);

	XMStoreFloat3(&m_cbPerCameraCache.cameraPosW, pCameraOwner->m_transform.GetWorldPosition());
	m_cbPerCameraCache.tessMinDist = pCamera->GetMinDistanceTessellationToStart();
	m_cbPerCameraCache.tessMaxDist = pCamera->GetMaxDistanceTessellationToStart();
	m_cbPerCameraCache.minTessExponent = pCamera->GetMinimumTessellationExponent();
	m_cbPerCameraCache.maxTessExponent = pCamera->GetMaximumTessellationExponent();

	XMStoreFloat4x4A(&m_cbPerCameraCache.vp, ConvertToHLSLMatrix(vp));

	m_dirtyFlag |= DirtyFlag::UpdateCBPerCamera;
}

void XM_CALLCONV BasicEffectPN::SetWorldMatrix(FXMMATRIX w) noexcept
{
	XMStoreFloat4x4A(&m_cbPerMeshCache.w, ConvertToHLSLMatrix(w));			// HLSL 전치
	XMStoreFloat4x4A(&m_cbPerMeshCache.wInvTr, XMMatrixInverse(nullptr, w));	// 역행렬의 전치의 HLSL 전치

	m_dirtyFlag |= DirtyFlag::UpdateCBPerMesh;
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
		m_cbMaterialCache.mtl.diffuse = pMaterial->m_diffuse;
		m_cbMaterialCache.mtl.specular = pMaterial->m_specular;
		m_cbMaterialCache.mtl.reflect = pMaterial->m_reflect;

		m_dirtyFlag |= DirtyFlag::UpdateCBMaterial;
	}

	if (m_pCurrPS != pPS)
	{
		m_pCurrPS = pPS;
		m_dirtyFlag |= DirtyFlag::PixelShader;
	}
}

void BasicEffectPN::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
{
	DWORD index;

	while (BitScanForward(&index, m_dirtyFlag))
	{
		switch (static_cast<DWORD>(1) << index)
		{
		case DirtyFlag::PrimitiveTopology:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
		case DirtyFlag::InputLayout:
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			break;
		case DirtyFlag::Shader:
			ApplyShader(pDeviceContext);
			break;
		case DirtyFlag::PixelShader:
			ApplyPixelShader(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBPerFrame:
			ApplyPerFrameConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBPerCamera:
			ApplyPerCameraConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBPerMesh:
			ApplyPerMeshConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBMaterial:
			ApplyMaterialConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::UpdateCBPerFrame:
			m_cbPerFrame.Update(pDeviceContext, &m_cbPerFrameCache);
			break;
		case DirtyFlag::UpdateCBPerCamera:
			m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
			break;
		case DirtyFlag::UpdateCBPerMesh:
			m_cbPerMesh.Update(pDeviceContext, &m_cbPerMeshCache);
			break;
		case DirtyFlag::UpdateCBMaterial:
			m_cbMaterial.Update(pDeviceContext, &m_cbMaterialCache);
			break;
		default:
			assert(false);
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}
}

void BasicEffectPN::OnUnbindFromDeviceContext() noexcept
{
	m_dirtyFlag = DirtyFlag::ALL;

	const DWORD except =
		DirtyFlag::UpdateCBPerFrame |
		DirtyFlag::UpdateCBPerCamera |
		DirtyFlag::UpdateCBPerMesh |
		DirtyFlag::UpdateCBMaterial;

	m_dirtyFlag = m_dirtyFlag & ~except;
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
	ID3D11Buffer* const cbs[] = { m_cbPerFrame.GetComInterface() };

	// PerFrame 상수버퍼 사용 셰이더
	constexpr UINT PS_SLOT = 0;
	pDeviceContext->PSSetConstantBuffers(PS_SLOT, 1, cbs);
}

void BasicEffectPN::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbPerCamera.GetComInterface() };

	// PerCamera 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 0;
	constexpr UINT PS_SLOT = 1;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
	pDeviceContext->PSSetConstantBuffers(PS_SLOT, 1, cbs);
}

void BasicEffectPN::ApplyPerMeshConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbPerMesh.GetComInterface() };

	// PerMesh 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 1;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}

void BasicEffectPN::ApplyMaterialConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbMaterial.GetComInterface() };

	// Material 상수버퍼 사용 셰이더
	constexpr UINT PS_SLOT = 2;
	pDeviceContext->PSSetConstantBuffers(PS_SLOT, 1, cbs);
}
