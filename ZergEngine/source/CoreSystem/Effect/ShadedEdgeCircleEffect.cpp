#include <ZergEngine\CoreSystem\Effect\ShadedEdgeCircleEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

using namespace ze;

// Image Effect
// VertexShader:
// - ToHcsPNTTQuadForShadedEdgeCircle
// 
// PixelShader:
// - ColorShadedEdgeCircle

void ShadedEdgeCircleEffect::Init()
{
	m_dirtyFlag = DirtyFlag::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VertexFormatType::PositionNormalTangentTexCoord);
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::ToHcsPNTTQuadForShadedEdgeCircle);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::ColorShadedEdgeCircle);

	m_cb2DRender.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	m_cbPerShadedEdgeCircle.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
}

void ShadedEdgeCircleEffect::Release()
{
	m_cb2DRender.Release();
	m_cbPerShadedEdgeCircle.Release();
}

void ShadedEdgeCircleEffect::SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept
{
	m_cb2DRenderCache.toNDCSpaceRatio = ratio;

	m_dirtyFlag |= DirtyFlag::UpdateCB2DRender;
}

void ShadedEdgeCircleEffect::SetRadius(FLOAT radius) noexcept
{
	assert(radius > 0);
	m_cbPerShadedEdgeCircleCache.radius = radius;

	m_dirtyFlag |= DirtyFlag::UpdateCBPerShadedEdgeCircle;
}

void ShadedEdgeCircleEffect::SetHCSPosition(const XMFLOAT2& pos) noexcept
{
	m_cbPerShadedEdgeCircleCache.position = pos;

	m_dirtyFlag |= DirtyFlag::UpdateCBPerShadedEdgeCircle;
}

void XM_CALLCONV ShadedEdgeCircleEffect::SetColor(FXMVECTOR color) noexcept
{
	XMStoreFloat4A(&m_cbPerShadedEdgeCircleCache.color, color);

	m_dirtyFlag |= DirtyFlag::UpdateCBPerShadedEdgeCircle;
}

void ShadedEdgeCircleEffect::SetColorWeight(FLOAT lt, FLOAT rb) noexcept
{
	m_cbPerShadedEdgeCircleCache.ltColorWeight = lt;
	m_cbPerShadedEdgeCircleCache.rbColorWeight = rb;

	m_dirtyFlag |= DirtyFlag::UpdateCBPerShadedEdgeCircle;
}

void ShadedEdgeCircleEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
{
	DWORD index;

	while (BitScanForward(&index, m_dirtyFlag))
	{
		switch (static_cast<DWORD>(1) << index)
		{
		case DirtyFlag::PrimitiveTopology:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			break;
		case DirtyFlag::InputLayout:
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			break;
		case DirtyFlag::Shader:
			ApplyShader(pDeviceContext);
			break;
		case DirtyFlag::ApplyCB2DRender:
			Apply2DRenderConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBPerShadedEdgeCircle:
			ApplyPerShadedEdgeCircleConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::UpdateCB2DRender:
			m_cb2DRender.Update(pDeviceContext, &m_cb2DRenderCache);
			break;
		case DirtyFlag::UpdateCBPerShadedEdgeCircle:
			m_cbPerShadedEdgeCircle.Update(pDeviceContext, &m_cbPerShadedEdgeCircleCache);
			break;
		default:
			*reinterpret_cast<int*>(0) = 0;		// Force crash
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}
}

void ShadedEdgeCircleEffect::OnUnbindFromDeviceContext() noexcept
{
	m_dirtyFlag = DirtyFlag::ALL;

	const DWORD except =
		DirtyFlag::UpdateCB2DRender |
		DirtyFlag::UpdateCBPerShadedEdgeCircle;

	m_dirtyFlag = m_dirtyFlag & ~except;
}

void ShadedEdgeCircleEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void ShadedEdgeCircleEffect::Apply2DRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cb2DRender.GetComInterface() };

	// 2DRender 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 0;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}

void ShadedEdgeCircleEffect::ApplyPerShadedEdgeCircleConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbPerShadedEdgeCircle.GetComInterface() };

	// Per2DQuad 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 1;
	constexpr UINT PS_SLOT = 0;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
	pDeviceContext->PSSetConstantBuffers(PS_SLOT, 1, cbs);
}
