#include <ZergEngine\CoreSystem\Effect\ButtonEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

// Button Effect
// VertexShader:
// - ToHcsShaded2DQuad
// 
// PixelShader:
// - UnlitPC

void ButtonEffect::Init()
{
	m_dirtyFlag = DirtyFlag::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VertexFormatType::Shaded2DQuad);
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::ToHcsShaded2DQuad);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::UnlitPC);

	m_cbPerUIRender.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	m_cbPerButton.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
}

void ButtonEffect::Release()
{
	m_cbPerUIRender.Release();
	m_cbPerButton.Release();
}

void ButtonEffect::SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept
{
	m_cbPerUIRenderCache.toNDCSpaceRatio = ratio;

	m_dirtyFlag |= DirtyFlag::CBPerUIRender;
}

void XM_CALLCONV ButtonEffect::SetColor(FXMVECTOR color) noexcept
{
	XMStoreFloat4A(&m_cbPerButtonCache.color, color);

	m_dirtyFlag |= DirtyFlag::CONSTANTBUFFER_PER_BUTTON;
}

void ButtonEffect::SetPressed(bool pressed) noexcept
{
	if (pressed)
		m_cbPerButtonCache.shadeIndex = 1;
	else
		m_cbPerButtonCache.shadeIndex = 0;

	m_dirtyFlag |= DirtyFlag::CONSTANTBUFFER_PER_BUTTON;
}

void XM_CALLCONV ButtonEffect::SetSize(FXMVECTOR size) noexcept
{
	XMStoreFloat2(&m_cbPerButtonCache.size, size);

	m_dirtyFlag |= DirtyFlag::CONSTANTBUFFER_PER_BUTTON;
}

void XM_CALLCONV ButtonEffect::SetPreNDCPosition(FXMVECTOR position) noexcept
{
	XMStoreFloat2(&m_cbPerButtonCache.position, position);

	m_dirtyFlag |= DirtyFlag::CONSTANTBUFFER_PER_BUTTON;
}

void ButtonEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
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
		case DirtyFlag::CBPerUIRender:
			ApplyPerUIRenderConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::CONSTANTBUFFER_PER_BUTTON:
			ApplyPerButtonConstantBuffer(pDeviceContext);
			break;
		default:
			assert(false);
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}
}

void ButtonEffect::KickedOutOfDeviceContext() noexcept
{
	m_dirtyFlag = DirtyFlag::ALL;
}

void ButtonEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void ButtonEffect::ApplyPerUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerUIRender.Update(pDeviceContext, &m_cbPerUIRenderCache);
	ID3D11Buffer* const cbs[] = { m_cbPerUIRender.GetComInterface() };

	// PerUIRender 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void ButtonEffect::ApplyPerButtonConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerButton.Update(pDeviceContext, &m_cbPerButtonCache);
	ID3D11Buffer* const cbs[] = { m_cbPerButton.GetComInterface() };

	// PerButton 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 1;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}
