#include <ZergEngine\CoreSystem\Effect\CheckboxEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

// Button Effect
// VertexShader:
// - ToHcsCheckbox
// 
// PixelShader:
// - UnlitPC

void CheckboxEffect::Init()
{
	m_dirtyFlag = DirtyFlag::ALL;

	GraphicDevice* pGraphicDevice = GraphicDevice::GetInstance();

	m_pInputLayout = pGraphicDevice->GetILComInterface(VertexFormatType::Checkbox);
	m_pVertexShader = pGraphicDevice->GetVSComInterface(VertexShaderType::ToHcsCheckbox);
	m_pPixelShader = pGraphicDevice->GetPSComInterface(PixelShaderType::UnlitPC);

	m_cb2DRender.Init(pGraphicDevice->GetDevice());
	m_cbPerCheckbox.Init(pGraphicDevice->GetDevice());
}

void CheckboxEffect::Release()
{
	m_cb2DRender.Release();
	m_cbPerCheckbox.Release();
}

void CheckboxEffect::SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept
{
	m_cb2DRenderCache.toNDCSpaceRatio = ratio;

	m_dirtyFlag |= DirtyFlag::UpdateCB2DRender;
}

void XM_CALLCONV CheckboxEffect::SetBoxColor(FXMVECTOR color) noexcept
{
	XMStoreFloat4A(&m_cbPerCheckboxCache.boxColor, color);

	m_dirtyFlag |= DirtyFlag::UpdateCBPerCheckbox;
}

void XM_CALLCONV CheckboxEffect::SetCheckColor(FXMVECTOR color) noexcept
{
	XMStoreFloat4A(&m_cbPerCheckboxCache.checkColor, color);

	m_dirtyFlag |= DirtyFlag::UpdateCBPerCheckbox;
}

void CheckboxEffect::SetSize(FLOAT width, FLOAT height) noexcept
{
	m_cbPerCheckboxCache.size.x = width;
	m_cbPerCheckboxCache.size.y = height;

	m_dirtyFlag |= DirtyFlag::UpdateCBPerCheckbox;
}

void CheckboxEffect::SetHCSPosition(const XMFLOAT2& pos) noexcept
{
	m_cbPerCheckboxCache.position = pos;

	m_dirtyFlag |= DirtyFlag::UpdateCBPerCheckbox;
}

void CheckboxEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
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
		case DirtyFlag::ApplyCB2DRender:
			Apply2DRenderConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBPerCheckbox:
			ApplyPerCheckboxConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::UpdateCB2DRender:
			m_cb2DRender.Update(pDeviceContext, &m_cb2DRenderCache);
			break;
		case DirtyFlag::UpdateCBPerCheckbox:
			m_cbPerCheckbox.Update(pDeviceContext, &m_cbPerCheckboxCache);
			break;
		default:
			assert(false);
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}
}

void CheckboxEffect::OnUnbindFromDeviceContext() noexcept
{
	m_dirtyFlag = DirtyFlag::ALL;

	const DWORD except =
		DirtyFlag::UpdateCB2DRender |
		DirtyFlag::UpdateCBPerCheckbox;

	m_dirtyFlag = m_dirtyFlag & ~except;
}

void CheckboxEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void CheckboxEffect::Apply2DRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cb2DRender.Get() };

	// 2DRender 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 0;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}

void CheckboxEffect::ApplyPerCheckboxConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbPerCheckbox.Get() };

	// PerCheckbox 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 1;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}
