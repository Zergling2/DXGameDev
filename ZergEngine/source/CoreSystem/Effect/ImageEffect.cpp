#include <ZergEngine\CoreSystem\Effect\ImageEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

using namespace ze;

// Image Effect
// VertexShader:
// - ToHcsPNTTQuadForImage
// 
// PixelShader:
// - UnlitPT1Tex

void ImageEffect::Init()
{
	m_dirtyFlag = DirtyFlag::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VertexFormatType::PositionNormalTangentTexCoord);
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::ToHcsPNTTQuadForImage);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::UnlitPT1Tex);

	m_cbUIRender.Init(GraphicDevice::GetInstance()->GetDevice());
	m_cbPer2DQuad.Init(GraphicDevice::GetInstance()->GetDevice());
}

void ImageEffect::Release()
{
	m_cbUIRender.Release();
	m_cbPer2DQuad.Release();
}

void XM_CALLCONV ImageEffect::SetOrthoMatrix(FXMMATRIX m) noexcept
{
	XMStoreFloat4x4A(&m_cbUIRenderCache.m, ConvertToHLSLMatrix(m));

	m_dirtyFlag |= DirtyFlag::UpdateCBUIRender;
}

void ImageEffect::SetSize(FLOAT width, FLOAT height) noexcept
{
	m_cbPer2DQuadCache.size.x = width;
	m_cbPer2DQuadCache.size.y = height;

	m_dirtyFlag |= DirtyFlag::UpdateCBPer2DQuad;
}

void ImageEffect::SetUVScale(FLOAT sx, FLOAT sy) noexcept
{
	m_cbPer2DQuadCache.uvScale.x = sx;
	m_cbPer2DQuadCache.uvScale.y = sy;

	m_dirtyFlag |= DirtyFlag::UpdateCBPer2DQuad;
}

void ImageEffect::SetUVOffset(FLOAT x, FLOAT y) noexcept
{
	m_cbPer2DQuadCache.uvOffset.x = x;
	m_cbPer2DQuadCache.uvOffset.y = y;

	m_dirtyFlag |= DirtyFlag::UpdateCBPer2DQuad;
}

void ImageEffect::SetHCSPosition(const XMFLOAT2& pos) noexcept
{
	m_cbPer2DQuadCache.position = pos;

	m_dirtyFlag |= DirtyFlag::UpdateCBPer2DQuad;
}

void ImageEffect::SetImageTexture(const Texture2D& image) noexcept
{
	m_pTextureSRVArray[0] = image.GetSRV();
}

void ImageEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
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
		case DirtyFlag::ApplyCBUIRender:
			ApplyUIRenderConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBPer2DQuad:
			ApplyPer2DQuadConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::UpdateCBUIRender:
			m_cbUIRender.Update(pDeviceContext, &m_cbUIRenderCache);
			break;
		case DirtyFlag::UpdateCBPer2DQuad:
			m_cbPer2DQuad.Update(pDeviceContext, &m_cbPer2DQuadCache);
			break;
		default:
			*reinterpret_cast<int*>(0) = 0;		// Force crash
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}

	// 텍스처 리소스는 항상 재설정 (포인터를 들고 있어봤자 메모리 재사용 등으로 인해 이전 리소스 그대로인지 확신할 수 없음..)
	/*
	Texture2D tex2d_image : register(t0);
	*/
	pDeviceContext->PSSetShaderResources(0, _countof(m_pTextureSRVArray), m_pTextureSRVArray);

	ClearTextureSRVArray();
}

void ImageEffect::OnUnbindFromDeviceContext() noexcept
{
	m_dirtyFlag = DirtyFlag::ALL;

	const DWORD except =
		DirtyFlag::UpdateCBUIRender |
		DirtyFlag::UpdateCBPer2DQuad;

	m_dirtyFlag = m_dirtyFlag & ~except;
}

void ImageEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void ImageEffect::ApplyUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbUIRender.Get() };

	// UIRender 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 0;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}

void ImageEffect::ApplyPer2DQuadConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbPer2DQuad.Get() };

	// Per2DQuad 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 1;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}
