#include <ZergEngine\CoreSystem\Effect\DebugLinesEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>

using namespace ze;

// BasicEffectPC Effect
// VertexShader:
// - ToHcsDebugLines
// 
// PixelShader:
// - UnlitPC

void DebugLinesEffect::Init()
{
	m_dirtyFlag = DirtyFlag::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VertexFormatType::PositionColor);
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::ToHcsDebugLines);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::UnlitPC);

	m_cbPerCamera.Init(GraphicDevice::GetInstance()->GetDevice());
}

void DebugLinesEffect::Release()
{
	m_cbPerCamera.Release();
}

void DebugLinesEffect::SetCamera(const Camera* pCamera) noexcept
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

void DebugLinesEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
{
	DWORD index;

	while (BitScanForward(&index, m_dirtyFlag))
	{
		switch (static_cast<DWORD>(1) << index)
		{
		case DirtyFlag::PrimitiveTopology:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			break;
		case DirtyFlag::InputLayout:
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			break;
		case DirtyFlag::Shader:
			ApplyShader(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBPerCamera:
			ApplyPerCameraConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::UpdateCBPerCamera:
			m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
			break;
		default:
			*reinterpret_cast<int*>(0) = 0;	// Force crash
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}
}

void DebugLinesEffect::OnUnbindFromDeviceContext() noexcept
{
	m_dirtyFlag = DirtyFlag::ALL;

	const DWORD except =
		DirtyFlag::UpdateCBPerCamera;

	m_dirtyFlag = m_dirtyFlag & ~except;
}

void DebugLinesEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void DebugLinesEffect::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbPerCamera.Get() };

	// PerCamera 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 0;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}
