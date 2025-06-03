#include <ZergEngine\CoreSystem\Effect\PanelEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

void PanelEffect::Init()
{
	ID3D11Device* pDevice = GraphicDevice.GetDeviceComInterface();

	m_dirtyFlag = DIRTY_FLAG::ALL;

	m_pInputLayout = nullptr;
	m_pVertexShader = GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::TRANSFORM_PC_QUAD_TO_HCS);
	m_pPixelShader = GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::COLOR_PC_FRAGMENT);

	m_cbPerUIRender.Init(pDevice);
	m_cbPerPanel.Init(pDevice);
}

void PanelEffect::Release()
{
	m_cbPerUIRender.Release();
	m_cbPerPanel.Release();
}

void PanelEffect::SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept
{
	m_cbPerUIRenderCache.toNDCSpaceRatio = ratio;

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_UI_RENDER;
}

void XM_CALLCONV PanelEffect::SetColor(FXMVECTOR color) noexcept
{
	XMStoreFloat4A(&m_cbPerPanelCache.color, color);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_PANEL;
}

void XM_CALLCONV PanelEffect::SetSize(FXMVECTOR size) noexcept
{
	XMStoreFloat2(&m_cbPerPanelCache.size, size);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_PANEL;
}

void XM_CALLCONV PanelEffect::SetPreNDCPosition(FXMVECTOR position) noexcept
{
	XMStoreFloat2(&m_cbPerPanelCache.position, position);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_PANEL;
}

void PanelEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
{
	DWORD index;

	while (BitScanForward(&index, m_dirtyFlag))
	{
		switch (static_cast<DWORD>(1) << index)
		{
		case DIRTY_FLAG::PRIMITIVE_TOPOLOGY:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			break;
		case DIRTY_FLAG::INPUT_LAYOUT:
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			break;
		case DIRTY_FLAG::SHADER:
			ApplyShader(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_UI_RENDER:
			ApplyPerUIRenderConstantBuffer(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_PANEL:
			ApplyPerPanelConstantBuffer(pDeviceContext);
			break;
		default:
			assert(false);
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}
}

void PanelEffect::KickedOutOfDeviceContext() noexcept
{
	m_dirtyFlag = DIRTY_FLAG::ALL;
}

void PanelEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void PanelEffect::ApplyPerUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerUIRender.Update(pDeviceContext, &m_cbPerUIRenderCache);
	ID3D11Buffer* const cbs[] = { m_cbPerUIRender.GetComInterface() };

	// PerUIRender 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void PanelEffect::ApplyPerPanelConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerPanel.Update(pDeviceContext, &m_cbPerPanelCache);
	ID3D11Buffer* const cbs[] = { m_cbPerPanel.GetComInterface() };

	// PerPanel 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 1;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}
