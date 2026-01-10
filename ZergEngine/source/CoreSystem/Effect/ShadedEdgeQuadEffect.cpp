#include <ZergEngine\CoreSystem\Effect\ShadedEdgeQuadEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

using namespace ze;

// Image Effect
// VertexShader:
// - ToHcsPNTTQuadForShadedEdgeQuad
// 
// PixelShader:
// - ColorShadedEdgeQuad

void ShadedEdgeQuadEffect::Init()
{
	m_dirtyFlag = DirtyFlag::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VertexFormatType::PositionNormalTangentTexCoord);
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::ToHcsPNTTQuadForShadedEdgeQuad);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::ColorShadedEdgeQuad);

	m_cb2DRender.Init(GraphicDevice::GetInstance()->GetDevice());
	m_cbPerShadedEdgeQuad.Init(GraphicDevice::GetInstance()->GetDevice());
}

void ShadedEdgeQuadEffect::Release()
{
	m_cb2DRender.Release();
	m_cbPerShadedEdgeQuad.Release();
}

void ShadedEdgeQuadEffect::SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept
{
	m_cb2DRenderCache.toNDCSpaceRatio = ratio;

	m_dirtyFlag |= DirtyFlag::UpdateCB2DRender;
}

void ShadedEdgeQuadEffect::SetSize(FLOAT width, FLOAT height) noexcept
{
	m_cbPerShadedEdgeQuadCache.size.x = width;
	m_cbPerShadedEdgeQuadCache.size.y = height;

	m_dirtyFlag |= DirtyFlag::UpdateCBPerShadedEdgeQuad;
}

void ShadedEdgeQuadEffect::SetHCSPosition(const XMFLOAT2& pos) noexcept
{
	m_cbPerShadedEdgeQuadCache.position = pos;

	m_dirtyFlag |= DirtyFlag::UpdateCBPerShadedEdgeQuad;
}

void XM_CALLCONV ShadedEdgeQuadEffect::SetColor(FXMVECTOR color) noexcept
{
	XMStoreFloat4A(&m_cbPerShadedEdgeQuadCache.color, color);

	m_dirtyFlag |= DirtyFlag::UpdateCBPerShadedEdgeQuad;
}

void ShadedEdgeQuadEffect::SetColorWeight(FLOAT lt, FLOAT rb) noexcept
{
	m_cbPerShadedEdgeQuadCache.ltColorWeight = lt;
	m_cbPerShadedEdgeQuadCache.rbColorWeight = rb;

	m_dirtyFlag |= DirtyFlag::UpdateCBPerShadedEdgeQuad;
}

void ShadedEdgeQuadEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
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
		case DirtyFlag::ApplyCBPerShadedEdgeQuad:
			ApplyPerShadedEdgeQuadConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::UpdateCB2DRender:
			m_cb2DRender.Update(pDeviceContext, &m_cb2DRenderCache);
			break;
		case DirtyFlag::UpdateCBPerShadedEdgeQuad:
			m_cbPerShadedEdgeQuad.Update(pDeviceContext, &m_cbPerShadedEdgeQuadCache);
			break;
		default:
			*reinterpret_cast<int*>(0) = 0;		// Force crash
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}
}

void ShadedEdgeQuadEffect::OnUnbindFromDeviceContext() noexcept
{
	m_dirtyFlag = DirtyFlag::ALL;

	const DWORD except =
		DirtyFlag::UpdateCB2DRender |
		DirtyFlag::UpdateCBPerShadedEdgeQuad;

	m_dirtyFlag = m_dirtyFlag & ~except;
}

void ShadedEdgeQuadEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void ShadedEdgeQuadEffect::Apply2DRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cb2DRender.Get() };

	// 2DRender 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 0;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}

void ShadedEdgeQuadEffect::ApplyPerShadedEdgeQuadConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbPerShadedEdgeQuad.Get() };

	// Per2DQuad 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 1;
	constexpr UINT PS_SLOT = 0;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
	pDeviceContext->PSSetConstantBuffers(PS_SLOT, 1, cbs);
}
