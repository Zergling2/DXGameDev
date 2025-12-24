#include <ZergEngine\CoreSystem\Effect\Shaded2DQuadEffect_deprecated.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

// Button Effect
// VertexShader:
// - ToHcsShaded2DQuad
// 
// PixelShader:
// - UnlitPC

void Shaded2DQuadEffect::Init()
{
	m_dirtyFlag = DirtyFlag::ALL;

	GraphicDevice* pGraphicDevice = GraphicDevice::GetInstance();

	m_pInputLayout = pGraphicDevice->GetILComInterface(VertexFormatType::Shaded2DQuad);
	m_pVertexShader = pGraphicDevice->GetVSComInterface(VertexShaderType::ToHcsShaded2DQuad);
	m_pPixelShader = pGraphicDevice->GetPSComInterface(PixelShaderType::UnlitPC);

	m_cb2DRender.Init(pGraphicDevice->GetDeviceComInterface());
	m_cbPerShaded2DQuad.Init(pGraphicDevice->GetDeviceComInterface());
}

void Shaded2DQuadEffect::Release()
{
	m_cb2DRender.Release();
	m_cbPerShaded2DQuad.Release();
}

void Shaded2DQuadEffect::SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept
{
	m_cb2DRenderCache.toNDCSpaceRatio = ratio;

	m_dirtyFlag |= DirtyFlag::CB2DRender;
}

void XM_CALLCONV Shaded2DQuadEffect::SetColor(FXMVECTOR color) noexcept
{
	XMStoreFloat4A(&m_cbPerShaded2DQuadCache.color, color);

	m_dirtyFlag |= DirtyFlag::CBPerShaded2DQuad;
}

void Shaded2DQuadEffect::SetColorWeightIndex(uint32_t index) noexcept
{
	assert(index <= SHADED_2DQUAD_COLOR_WEIGHT_INDEX_CONCAVE);

	m_cbPerShaded2DQuadCache.colorWeightIndex = index;

	m_dirtyFlag |= DirtyFlag::CBPerShaded2DQuad;
}

void Shaded2DQuadEffect::SetSize(FLOAT width, FLOAT height) noexcept
{
	m_cbPerShaded2DQuadCache.size.x = width;
	m_cbPerShaded2DQuadCache.size.y = height;

	m_dirtyFlag |= DirtyFlag::CBPerShaded2DQuad;
}

void Shaded2DQuadEffect::SetHCSPosition(const XMFLOAT2& pos) noexcept
{
	m_cbPerShaded2DQuadCache.position = pos;

	m_dirtyFlag |= DirtyFlag::CBPerShaded2DQuad;
}

void Shaded2DQuadEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
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
		case DirtyFlag::CB2DRender:
			ApplyPerUIRenderConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::CBPerShaded2DQuad:
			ApplyPerShaded2DQuadConstantBuffer(pDeviceContext);
			break;
		default:
			assert(false);
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}
}

void Shaded2DQuadEffect::OnUnbindFromDeviceContext() noexcept
{
	m_dirtyFlag = DirtyFlag::ALL;
}

void Shaded2DQuadEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void Shaded2DQuadEffect::ApplyPerUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cb2DRender.Update(pDeviceContext, &m_cb2DRenderCache);
	ID3D11Buffer* const cbs[] = { m_cb2DRender.GetComInterface() };

	// PerUIRender 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 0;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}

void Shaded2DQuadEffect::ApplyPerShaded2DQuadConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerShaded2DQuad.Update(pDeviceContext, &m_cbPerShaded2DQuadCache);
	ID3D11Buffer* const cbs[] = { m_cbPerShaded2DQuad.GetComInterface() };

	// PerShaded2DQuad 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 1;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}
